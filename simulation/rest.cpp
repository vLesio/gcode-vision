﻿#include "rest.h"
#include "crow.h"
#include <iostream>
#include <atomic>
#include <fstream>
#include <regex>
#include <sstream>
#include <filesystem>

#include "camera.h"
#include "filamentSimulator.h"
#include "GCodeParser.h"
#include "simulationContext.h"
#include "simulationManager.h"
#include "simulationModeFactory.h"

namespace fs = std::filesystem;

// Global variables for controlling the simulation
extern std::atomic<bool> simulation_running;
extern void start_simulation();
extern void stop_simulation();

void save_file(const std::string& filename, const std::string& data) {
    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(data.c_str(), data.size());
    outFile.close();
}

crow::response json_response(int code, const std::string& message) {
    crow::response res(code);
    res.set_header("Content-Type", "application/json");
    res.write("{\"message\": \"" + message + "\"}");
    return res;
}

template<typename Func>
crow::response safe_camera_action(Func action, const std::string& successMessage) {
    try {
        action();
        return json_response(200, successMessage);
    }
    catch (const std::exception& e) {
        return json_response(400, e.what());
    }
}


void run_rest_api(int port) {
    crow::SimpleApp app;

    // === SIMULATION CONTROL ENDPOINTS ===

    CROW_ROUTE(app, "/simulation/start").methods(crow::HTTPMethod::POST)([] {
        if (!simulation_running) {
            start_simulation();
            return json_response(200, "Simulation started");
        }
        return json_response(400, "Simulation is already running");
    });
	// Deprecated: Uncomment if you want to stop the simulation
 /*   CROW_ROUTE(app, "/simulation/stop").methods(crow::HTTPMethod::POST)([] {
        if (simulation_running) {
            stop_simulation();
            return json_response(200, "Simulation stopped");
        }
        return json_response(400, "Simulation is not running");
    });*/

    CROW_ROUTE(app, "/simulation/pause").methods(crow::HTTPMethod::POST)([] {
        if (!simulation_running) return json_response(400, "Simulation is not running");

        SimulationManager::get().pauseSimulation();
        return json_response(200, "Simulation paused");
        });

    CROW_ROUTE(app, "/simulation/resume").methods(crow::HTTPMethod::POST)([] {
        if (!simulation_running) return json_response(400, "Simulation is not running");

        SimulationManager::get().resumeSimulation();
        return json_response(200, "Simulation resumed");
        });

    CROW_ROUTE(app, "/simulation/step/forward").methods(crow::HTTPMethod::POST)([] {
        if (!simulation_running) return json_response(400, "Simulation is not running");

        SimulationManager::get().stepForward();
        return json_response(200, "Simulation stepped forward");
        });

    CROW_ROUTE(app, "/simulation/step/backward").methods(crow::HTTPMethod::POST)([] {
        if (!simulation_running) return json_response(400, "Simulation is not running");

        SimulationManager::get().stepBackward();
        return json_response(200, "Simulation stepped backward");
        });

    // === CAMERA CONTROL ENDPOINTS ===

    CROW_ROUTE(app, "/camera/zoom/in").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().zoomIn();
            }, "Camera zoomed in");
        });

    CROW_ROUTE(app, "/camera/zoom/out").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().zoomOut();
            }, "Camera zoomed out");
        });

    CROW_ROUTE(app, "/camera/rotate/up").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().up();
            }, "Camera rotated up");
        });

    CROW_ROUTE(app, "/camera/rotate/down").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().down();
            }, "Camera rotated down");
        });

    CROW_ROUTE(app, "/camera/rotate/left").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().left();
            }, "Camera rotated left");
        });

    CROW_ROUTE(app, "/camera/rotate/right").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().right();
            }, "Camera rotated right");
        });

    CROW_ROUTE(app, "/camera/reset").methods(crow::HTTPMethod::POST)([] {
        return safe_camera_action([] {
            Camera::getInstance().reset();
            }, "Camera reset successfully");
        });

	CROW_ROUTE(app, "/camera/mode").methods(crow::HTTPMethod::POST)([] {
		return safe_camera_action([] {
			Camera::getInstance().toggleMode();
			}, "Camera mode toggled");
		});

    CROW_ROUTE(app, "/camera/target").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        return safe_camera_action([&req] {

            if (!req.url_params.get("x") || !req.url_params.get("y") || !req.url_params.get("z")) {
                throw std::invalid_argument("Missing x, y, or z parameter");
            }

            float x = std::stof(req.url_params.get("x"));
            float y = std::stof(req.url_params.get("y"));
            float z = std::stof(req.url_params.get("z"));

            Camera::getInstance().setTarget(glm::vec3(x, y, z));
            }, 
            "Camera target set successfully");
		});
            

    // === FILE UPLOAD ENDPOINT ===

    CROW_ROUTE(app, "/gcode/upload").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            const std::string content_type = req.get_header_value("Content-Type");

            if (content_type.empty() || content_type.find("multipart/form-data") == std::string::npos) {
                return json_response(400, "Invalid content type. Expected multipart/form-data.");
            }

            size_t filename_pos = req.body.find("filename=");
            if (filename_pos == std::string::npos) {
                return json_response(400, "No filename found in multipart body.");
            }

            size_t start = req.body.find("\"", filename_pos) + 1;
            size_t end = req.body.find("\"", start);
            if (start == std::string::npos || end == std::string::npos || end <= start) {
                return json_response(400, "Failed to extract filename.");
            }

            std::string filename = req.body.substr(start, end - start);

            size_t file_data_start = req.body.find("\r\n\r\n", end);
            if (file_data_start == std::string::npos) {
                return json_response(400, "Invalid file data format.");
            }
            file_data_start += 4;

            size_t file_data_end = req.body.rfind("\r\n--");
            if (file_data_end == std::string::npos || file_data_end <= file_data_start) {
                return json_response(400, "Invalid multipart boundary.");
            }

            std::string file_data = req.body.substr(file_data_start, file_data_end - file_data_start);
            save_file(filename, file_data);

            return json_response(200, "File uploaded successfully as: " + filename);
        });

    // === GCODE LIST & DELETE ENDPOINTS ===

    CROW_ROUTE(app, "/gcode").methods(crow::HTTPMethod::GET)([] {
        std::vector<std::string> files;
        for (const auto& entry : fs::directory_iterator(".")) {
            if (entry.path().extension() == ".gcode") {
                files.push_back(entry.path().filename().string());
            }
        }

        std::ostringstream json;
        json << "{ \"files\": [";
        for (size_t i = 0; i < files.size(); ++i) {
            json << "\"" << files[i] << "\"";
            if (i + 1 < files.size()) json << ", ";
        }
        json << "] }";

        crow::response res{json.str()};
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/gcode/<string>").methods(crow::HTTPMethod::Delete)(
        [](const std::string& filename) {
            if (!fs::exists(filename)) {
                return json_response(404, "File with specified name not found");
            }

            fs::remove(filename);
            return json_response(200, "File deleted successfully");
    });

    // === GET AVAILABLE PRINTERS ===

    CROW_ROUTE(app, "/simulation/printers").methods(crow::HTTPMethod::GET)([] {
        std::vector<std::string> printers = {
            "Creality Ender 3 V2",
            "Creality Ender 3 Neo"
        };

        std::ostringstream json;
        json << "{ \"printers\": [";
        for (size_t i = 0; i < printers.size(); ++i) {
            json << "\"" << printers[i] << "\"";
            if (i + 1 < printers.size()) json << ", ";
        }
        json << "] }";

        crow::response res{json.str()};
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        return res;
    });

    // === CREATE SIMULATION ===

    CROW_ROUTE(app, "/simulation/create").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            try {
                auto body = crow::json::load(req.body);
                if (!body) return json_response(400, "Invalid JSON payload");

                std::string mode = "fixed";
                if (body.has("mode")) mode = std::string(body["mode"].s());
                std::string printer = body["printer"].s();
                std::string gcodeFile = body["gcodeFile"].s();
                float simulationSpeed = static_cast<float>(body["simulationSpeed"].d());
                float nozzleDiameter = static_cast<float>(body["nozzleDiameter"].d());
				float layerHeight = static_cast<float>(body["layerHeight"].d());
                bool retraction = body["retraction"].b();
                float bedTemp = static_cast<float>(body["temperatures"]["bed"].d());
                float extruderTemp = static_cast<float>(body["temperatures"]["extruder"].d());
                float extrusionResolution = static_cast<float>(body["extrusionResolution"].d());

                SimulationManager& manager = SimulationManager::get();

                if (!manager.initializeSimulation(gcodeFile, extrusionResolution, printer, nozzleDiameter, layerHeight, retraction, bedTemp, extruderTemp, simulationSpeed)) {
                    return json_response(400, "Failed to initialize simulation");
                }

                ISimulationMode* strategy = SimulationModeFactory::createMode(mode);
                if (!strategy) {
                    return json_response(400, "Unknown simulation mode: " + mode);
                }

                manager.setSimulationMode(strategy);
                return json_response(200, "Simulation initialized with mode: " + mode);
            }
            catch (const std::exception& e) {
                return json_response(500, std::string("Error: ") + e.what());
            }
        });

    // === SWAGGER UI ===

    CROW_ROUTE(app, "/docs")([] {
        std::ifstream file("swagger-ui/index.html");
        if (!file.is_open())
            return json_response(404, "Swagger UI not found");

        std::stringstream buffer;
        buffer << file.rdbuf();

        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "text/html");
        res.body = buffer.str();
        return res;
    });

    CROW_ROUTE(app, "/swagger.yaml")([] {
        std::ifstream file("swagger.yaml", std::ios::binary);
        if (!file.is_open())
            return json_response(404, "swagger.yaml not found");

        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string contents = buffer.str();

        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "application/yaml");
        res.set_header("Content-Length", std::to_string(contents.size()));
        res.body = contents;
        return res;
    });

    CROW_ROUTE(app, "/swagger-ui/<string>")([](const crow::request&, std::string filename) {
        std::string path = "swagger-ui/" + filename;

        if (!fs::exists(path))
            return json_response(404, "File not found");

        std::ifstream file(path, std::ios::binary);
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string contents = buffer.str();

        std::string mime = "text/plain";
        auto ext = fs::path(path).extension().string();
        if (ext == ".css") mime = "text/css";
        else if (ext == ".js") mime = "application/javascript";
        else if (ext == ".html") mime = "text/html";
        else if (ext == ".yaml" || ext == ".yml") mime = "application/yaml";
        else if (ext == ".png") mime = "image/png";

        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", mime);
        res.set_header("Content-Length", std::to_string(contents.size()));
        res.body = contents;
        return res;
    });

	// === TEST ENDPOINTS ===
CROW_ROUTE(app, "/simulation/test-parse").methods(crow::HTTPMethod::GET)(
    []() {
        try {
            GCodeParser parser;
            if (!parser.loadFile("example.gcode")) {
                return json_response(500, "Could not load that file");
            }

            const auto& steps = parser.getPrintSteps();
            size_t totalSteps = steps.size();
            size_t extrudingSteps = std::count_if(steps.begin(), steps.end(), [](const PrintStep& s) {
                return s.extruding;
            });

            std::ofstream logfile("parsed_steps.log", std::ios::out);

            if (!logfile.is_open()) {
                return json_response(500, "Could not open log file");
            }

            for (const auto& step : steps) {
                std::string stepStr = step.toString();
                //std::cout << stepStr << std::endl;
                logfile << stepStr << std::endl;
            }

            logfile << "\nTotal Steps: " << totalSteps << std::endl;
            logfile << "Extruding Steps: " << extrudingSteps << std::endl;

            logfile.close();

            crow::json::wvalue result;
            result["totalSteps"] = static_cast<int>(totalSteps);
            result["extrudingSteps"] = static_cast<int>(extrudingSteps);
            result["status"] = "success";
            result["message"] = "File parsed successfully";

            return crow::response{ result };
        }
        catch (const std::exception& e) {
            return json_response(500, std::string("Błąd podczas analizy G-code: ") + e.what());
        }
    });
    
    // === START APP ===
    std::cout << "CROW:::: Starting REST API on port " << port << std::endl;
    app.port(port).multithreaded().run();
}
