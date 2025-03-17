#include "rest.h"
#include "simulation.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// Global flag for controlling the simulation
std::atomic<bool> simulation_running{ false };
std::thread simulation_thread; // Thread for the OpenGL simulation

// Function to start the simulation in a separate thread
void start_simulation() {
    if (!simulation_running) {
        simulation_running = true;
        simulation_thread = std::thread(run_opengl);
        std::cout << "Simulation started in a separate thread.\n";
    }
}

// Function to stop the simulation and close the OpenGL window
void stop_simulation() {
    if (simulation_running) {
        simulation_running = false;
        if (simulation_thread.joinable()) {
            simulation_thread.join();
        }
        std::cout << "Simulation stopped.\n";
    }
}

int main() {
    // Start the REST API in a separate thread
    std::thread rest_thread(run_rest_api, 18080);

    rest_thread.join();

    // Ensure the simulation is stopped before exiting the program
    stop_simulation();

    return 0;
}
