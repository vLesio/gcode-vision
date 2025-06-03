#include "rest.h"
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
#include "simulationState.h"

namespace fs = std::filesystem;

// Global variables for controlling the simulation
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

bool isStateOneOf(SimulationState current, std::initializer_list<SimulationState> validStates) {
    return std::find(validStates.begin(), validStates.end(), current) != validStates.end();
}

void run_rest_api(int port) {
    crow::SimpleApp app;

    // === SIMULATION CONTROL ENDPOINTS ===

    // === CREATE SIMULATION ===
    CROW_ROUTE(app, "/simulation/create").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            try {
                auto& manager = SimulationManager::get();
				// Terminate any existing simulation
                if (opengl_running && manager.getState() != SimulationState::Uninitialized) {
                    {
                        std::unique_lock<std::mutex> lock(manager.terminateMutex);
                        manager.terminateDone = false;
                        manager.enqueueEvent(SimulationEvent::Terminate);
                        manager.terminateCV.wait(lock, [&manager] {
                            return manager.terminateDone;
                            });
                    }
                }

				// Parse JSON body
                auto body = crow::json::load(req.body);
                if (!body) return json_response(400, "Invalid JSON payload");

                std::string printer = body["printer"].s();
                std::string gcodeFile = body["gcodeFile"].s();
                float simulationSpeed = static_cast<float>(body["simulationSpeed"].d());
				float maxPrinterSpeed = static_cast<float>(body["maxPrinterSpeed"].d());
                float nozzleDiameter = static_cast<float>(body["nozzleDiameter"].d());

				// Run initialization
                if (!manager.initializeSimulation(gcodeFile, printer, nozzleDiameter, maxPrinterSpeed, simulationSpeed)) {
                    return json_response(400, "Failed to initialize simulation");
                }

                return json_response(200, "Simulation initialized");
            }
            catch (const std::exception& e) {
                return json_response(500, std::string("Error: ") + e.what());
            }
        });


    // BEGIN (resume/start/restart depending on state)
    CROW_ROUTE(app, "/simulation/begin").methods(crow::HTTPMethod::POST)([] {
        if (!opengl_running)
            return json_response(400, "OpenGL is not running. Use /simulation/launch first.");

        auto& sim = SimulationManager::get();
        if (!isStateOneOf(sim.getState(), {
            SimulationState::Initialized,
            SimulationState::Prepared,
            SimulationState::Paused,
            SimulationState::Completed
            })) {
            return json_response(400, "Simulation is in invalid state for begin.");
        }

        sim.enqueueEvent(SimulationEvent::Begin);
        return json_response(200, "Begin enqueued.");
        });

	// LAUNCH
	CROW_ROUTE(app, "/simulation/launch").methods(crow::HTTPMethod::POST)([] {
		if (opengl_running) return json_response(400, "OpenGL is already running");
		start_simulation();
		return json_response(200, "OpenGL launched");
		});

    // PAUSE
    CROW_ROUTE(app, "/simulation/pause").methods(crow::HTTPMethod::POST)([] {
        if (!opengl_running) return json_response(400, "Simulation is not running");

        auto& sim = SimulationManager::get();
        if (sim.getState() != SimulationState::Running)
            return json_response(400, "Simulation must be running to pause.");

        sim.enqueueEvent(SimulationEvent::Pause);
        return json_response(200, "Pause enqueued");
        });

    // RESUME
    CROW_ROUTE(app, "/simulation/resume").methods(crow::HTTPMethod::POST)([] {
        if (!opengl_running) return json_response(400, "Simulation is not running");

        auto& sim = SimulationManager::get();
        if (sim.getState() != SimulationState::Paused)
            return json_response(400, "Simulation must be paused to resume.");

        sim.enqueueEvent(SimulationEvent::Resume);
        return json_response(200, "Resume enqueued");
        });

    // STEP FORWARD
    CROW_ROUTE(app, "/simulation/step/forward").methods(crow::HTTPMethod::POST)([] {
        if (!opengl_running) return json_response(400, "Simulation is not running");

        auto& sim = SimulationManager::get();
        if (!isStateOneOf(sim.getState(), {SimulationState::Paused}))
        {
            return json_response(400, "Can only step forward when simulation is paused.");
        }

        sim.enqueueEvent(SimulationEvent::StepForward);
        return json_response(200, "Step forward enqueued");
        });

    // STEP BACKWARD
    CROW_ROUTE(app, "/simulation/step/backward").methods(crow::HTTPMethod::POST)([] {
        if (!opengl_running) return json_response(400, "Simulation is not running");

        auto& sim = SimulationManager::get();
        if (!isStateOneOf(sim.getState(), {SimulationState::Paused,}))
        {
            return json_response(400, "Can only step backward when simulation is paused.");
        }
        sim.enqueueEvent(SimulationEvent::StepBackward);
        return json_response(200, "Step backward enqueued");
        });

    // RESET
    CROW_ROUTE(app, "/simulation/reset").methods(crow::HTTPMethod::POST)([] {
        if (!opengl_running) return json_response(400, "Simulation is not running");

        auto& sim = SimulationManager::get();
        if (!isStateOneOf(sim.getState(), {
            SimulationState::Running,
            SimulationState::Paused,
            SimulationState::Completed
            })) {
            return json_response(400, "Simulation must be running, paused, or completed to reset.");
        }

        sim.enqueueEvent(SimulationEvent::Reset);
        return json_response(200, "Reset enqueued");
        });

	// CHANGE SIMULATION SPEED
    CROW_ROUTE(app, "/simulation/speed").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        if (!opengl_running) return json_response(400, "Simulation is not running");
        auto& sim = SimulationManager::get();
        if (!isStateOneOf(sim.getState(), {
            SimulationState::Running,
            SimulationState::Paused, })) {
            return json_response(400, "Simulation must be running or paused to change speed.");
        }
		if (!req.url_params.get("speed")) {
			return json_response(400, "Missing 'speed' parameter");
		}
		float speed = std::stof(req.url_params.get("speed"));

        sim.setSpeed(speed);
		sim.enqueueEvent(SimulationEvent::ChangeSpeed);
        return json_response(200, "Simulation speed changed to " + std::to_string(speed));
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
