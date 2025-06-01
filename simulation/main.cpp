#include "rest.h"
#include "simulation.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include "SimulationManager.h"
#include "simulationState.h"

// Global flag for controlling the simulation
std::thread simulation_thread; // Thread for the OpenGL simulation

// Function to start the simulation in a separate thread
void start_simulation() {
    if (!opengl_running) {
        if (SimulationManager::get().getState() == SimulationState::Uninitialized)
        {
			std::cerr << "Simulation not initialized. Cannot start OpenGL.\n";
			return;
        }
        simulation_thread = std::thread(run_opengl);
        opengl_running = true;
        std::cout << "Simulation started in a separate thread.\n";
    }
}

// Function to stop the simulation and close the OpenGL window
void stop_simulation() {
    if (opengl_running) {
        opengl_running = false;
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
