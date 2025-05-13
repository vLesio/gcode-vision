#ifndef SIMULATION_CONTEXT_H
#define SIMULATION_CONTEXT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "GCodeParser.h"
#include "InstancedObject.h"

struct SimulationContext {
    // G-code
    std::string loadedFilename;
    std::vector<PrintStep> printSteps;
    size_t printStepsCount = 0;

    // Visualisation objects
    InstancedObject* filamentObject = nullptr;       
    InstancedObject* tempSegmentObject = nullptr;     

    // Printer config
    std::string printerName;
    float extrusionResolution = 0.2f;
    float nozzleDiameter = 1.75f;
    float layerHeight = 0.2f;
    bool retractionEnabled = false;
    float temperatureBed = 0.0f;
    float temperatureExtruder = 0.0f;
    float simulationSpeed = 1.0f;
    float simulationScale = 0.1f;

    // Runtime state
    glm::vec3 headPosition = glm::vec3(0.0f);
    size_t currentStepIndex = 0;
    float simulationTime = 0.0f;

    void clear() {
        loadedFilename.clear();
        printSteps.clear();
        printStepsCount = 0;
        filamentObject = nullptr;
        tempSegmentObject = nullptr;
        extrusionResolution = 0.2f;
        currentStepIndex = 0;
        simulationTime = 0.0f;
        headPosition = glm::vec3(0.0f);
        printerName.clear();
        nozzleDiameter = 1.75f;
        layerHeight = 0.2f;
        retractionEnabled = false;
        temperatureBed = 0.0f;
        temperatureExtruder = 0.0f;
        simulationSpeed = 1.0f;
    }

    void resetRuntime() {
        currentStepIndex = 0;
        simulationTime = 0.0f;
        headPosition = glm::vec3(0.0f);
    }
};

#endif // SIMULATION_CONTEXT_H
