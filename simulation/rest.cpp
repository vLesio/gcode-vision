#include "rest.h"
#include "simulation.h"
#include "crow.h"
#include <iostream>
#include <atomic>
#include <fstream>
#include <regex>

#include <sstream>
#include <filesystem>
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

    CROW_ROUTE(app, "/simulation/stop").methods(crow::HTTPMethod::POST)([] {
        if (simulation_running) {
            stop_simulation();
            return json_response(200, "Simulation stopped");
        }
        return json_response(400, "Simulation is not running");
        });

    CROW_ROUTE(app, "/simulation/pause").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Simulation paused");
        });

    CROW_ROUTE(app, "/simulation/resume").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Simulation resumed");
        });

    CROW_ROUTE(app, "/simulation/step/forward").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Simulation stepped forward");
        });

    CROW_ROUTE(app, "/simulation/step/backward").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Simulation stepped backward");
        });

    // === CAMERA CONTROL ENDPOINTS ===

    CROW_ROUTE(app, "/camera/zoom/in").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Camera zoomed in");
        });

    CROW_ROUTE(app, "/camera/zoom/out").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Camera zoomed out");
        });

    CROW_ROUTE(app, "/camera/rotate/left").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Camera rotated left");
        });

    CROW_ROUTE(app, "/camera/rotate/right").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Camera rotated right");
        });

    CROW_ROUTE(app, "/camera/rotate/up").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Camera rotated up");
        });

    CROW_ROUTE(app, "/camera/rotate/down").methods(crow::HTTPMethod::POST)([] {
        return json_response(200, "Camera rotated down");
        });

    // === FILE UPLOAD ENDPOINT ===


    CROW_ROUTE(app, "/simulation/upload").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            const std::string content_type = req.get_header_value("Content-Type");

            if (content_type.empty() || content_type.find("multipart/form-data") == std::string::npos) {
                return json_response(400, "Invalid content type. Expected multipart/form-data.");
            }

            // Try to extract filename from the content
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

            // Try to extract file data
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

            // Save to file
            save_file(filename, file_data);

            return json_response(200, "File uploaded successfully as: " + filename);
        });



    // === SWAGGER UI ===

   // Serve Swagger UI main page (docs)
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


    // === START APP ===
    std::cout << "CROW:::: Starting REST API on port " << port << std::endl;
    app.port(port).multithreaded().run();
}
