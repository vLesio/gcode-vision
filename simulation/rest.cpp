#include "rest.h"
#include "simulation.h"
#include "crow.h"
#include <iostream>
#include <atomic>
#include <fstream>
#include <regex>

// Global variables for controlling the simulation
extern std::atomic<bool> simulation_running;
extern void start_simulation();
extern void stop_simulation();


void save_file(const std::string& filename, const std::string& data) {
    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(data.c_str(), data.size());
    outFile.close();
}

std::string get_filename(const std::string& content_disposition) {
    std::regex filename_regex(R"(filename="(.+?)");
        std::smatch match;
    if (std::regex_search(content_disposition, match, filename_regex)) {
        return match[1].str();
    }
    return "uploaded_file";
}


void run_rest_api(int port) {
    crow::SimpleApp app;

    // Endpoint to start the simulation
    CROW_ROUTE(app, "/start").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            if (!simulation_running) {
                start_simulation();
                return crow::response(200, "Simulation started");
            }
            return crow::response(400, "Simulation already running");
        });

    // Endpoint to stop the simulation
    CROW_ROUTE(app, "/stop").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            if (simulation_running) {
                stop_simulation();
                return crow::response(200, "Simulation stopped");
            }
            return crow::response(400, "Simulation not running");
        });

    std::cout << "Starting REST API on port " << port << std::endl;

    CROW_ROUTE(app, "/upload").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            const std::string boundary = req.get_header_value("Content-Type");
            if (boundary.empty() || boundary.find("multipart/form-data") == std::string::npos) {
                return crow::response(400, "Invalid content type");
            }

      
            size_t filename_pos = req.body.find("filename=");
            if (filename_pos == std::string::npos) {
                return crow::response(400, "No filename found");
            }

     
            size_t start = req.body.find("\"", filename_pos) + 1;
            size_t end = req.body.find("\"", start);
            std::string filename = req.body.substr(start, end - start);

        
            size_t file_data_start = req.body.find("\r\n\r\n", end) + 4;
            size_t file_data_end = req.body.rfind("--");  // Koniec danych pliku
            if (file_data_start == std::string::npos || file_data_end == std::string::npos) {
                return crow::response(400, "Invalid file data");
            }

            std::string file_data = req.body.substr(file_data_start, file_data_end - file_data_start);

            save_file(filename, file_data);

            return crow::response(200, "File uploaded successfully: " + filename);
        });

    app.port(port).multithreaded().run();
}
