#ifndef SIMULATION_CONTEXT_H
#define SIMULATION_CONTEXT_H

#include <string>
#include <vector>
#include "GCodeParser.h"
#include "InstancedObject.h"

struct SimulationContext {
    std::string loadedFilename;
    std::vector<PrintStep> printSteps;
    InstancedObject* filamentObject = nullptr;

    // Configuration
    std::string printerName;
    float extrusionResolution = 0.2f;
    float nozzleDiameter = 1.75f;
    float layerHeight = 0.2f;
    bool retractionEnabled = false;
    float temperatureBed = 0.0f;
    float temperatureExtruder = 0.0f;
    float simulationSpeed = 1.0f;

    // Runtime state
    glm::vec3 headPosition = glm::vec3(0.0f);
    size_t currentStepIndex = 0;
    float simulationTime = 0.0f;

    void clear() {
        loadedFilename.clear();
        printSteps.clear();
        filamentObject = nullptr;
        extrusionResolution = 0.2f;
        currentStepIndex = 0;
        simulationTime = 0.0f;
        headPosition = glm::vec3(0.0f);
    }

    void resetRuntime() {
        currentStepIndex = 0;
        simulationTime = 0.0f;
        headPosition = glm::vec3(0.0f);
    }
};


#endif