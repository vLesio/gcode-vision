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
    float extrusionResolution = 0.2f;

    /// Config parameters from json
    std::string printerName;
    float extruderWidth = 0.4f;
    bool retractionEnabled = false;
    float temperatureBed = 0.0f;
    float temperatureExtruder = 0.0f;
    float userSpeed = 60.0f;

    void clear() {
        loadedFilename.clear();
        printSteps.clear();
        filamentObject = nullptr;
        extrusionResolution = 0.2f;
    }
};

#endif