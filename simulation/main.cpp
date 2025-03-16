#include "rest.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// Global variable for communication between threads
std::atomic<bool> simulation_running{ false };

// Simulation function
void run_simulation() {
    std::cout << "Simulation thread started.\n";

    while (true) {
        if (simulation_running) {
            std::cout << "Simulation running...\n";
        }
        else {
            std::cout << "Simulation stopped.\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    // Start the REST API in a separate thread
    std::thread rest_thread(run_rest_api, 18080);

    // Start the simulation in the main thread
    run_simulation();

    // Join threads (this won't be reached due to infinite loop in simulation)
    rest_thread.join();

    return 0;
}
