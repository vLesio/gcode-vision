#include "crow.h"
#include <iostream>
#include <fstream>

int main() {
    crow::SimpleApp app;

    // Endpoint to print received data
    CROW_ROUTE(app, "/log").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            std::cout << "Received: " << req.body << std::endl;
            return crow::response("Logged: " + req.body);
        });

    // Endpoint to handle file uploads
    CROW_ROUTE(app, "/upload").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            if (req.body.empty()) {
                return crow::response(400, "No file data received");
            }

            std::ofstream outFile("uploaded_file", std::ios::binary);
            outFile.write(req.body.data(), req.body.size());
            outFile.close();

            std::cout << "File uploaded successfully." << std::endl;
            return crow::response("File uploaded successfully");
        });

    app.port(18080).multithreaded().run();

    return 0;
}
