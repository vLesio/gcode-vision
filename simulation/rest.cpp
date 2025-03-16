#include "rest.h"
#include "crow.h"
#include <iostream>
#include <atomic>

extern std::atomic<bool> simulation_running;

void run_rest_api(int port) {
    crow::SimpleApp app;

    // Start Simulation
    CROW_ROUTE(app, "/start").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            if (!simulation_running) {
                simulation_running = true;
                std::cout << "Simulation started." << std::endl;
                return crow::response(200, "Simulation started");
            }
            return crow::response(400, "Simulation already running");
        });

    // Stop Simulation
    CROW_ROUTE(app, "/stop").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            if (simulation_running) {
                simulation_running = false;
                std::cout << "Simulation stopping..." << std::endl;
                return crow::response(200, "Simulation stopped");
            }
            return crow::response(400, "Simulation not running");
        });

    std::cout << "Starting REST API on port " << port << std::endl;

    CROW_ROUTE(app, "/upload").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            if (req.body.empty()) {
                return crow::response(400, "No file data received");
            }

            std::ofstream outFile("uploaded_file", std::ios::binary);
            outFile.write(req.body.data(), req.body.size());
            outFile.close();

            return crow::response("File uploaded successfully");
        });

    app.port(port).multithreaded().run();
}
