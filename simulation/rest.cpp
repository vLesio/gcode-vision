#include "rest.h"
#include "simulation.h"
#include "crow.h"
#include <iostream>
#include <atomic>

// Global variables for controlling the simulation
extern std::atomic<bool> simulation_running;
extern void start_simulation();
extern void stop_simulation();

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
