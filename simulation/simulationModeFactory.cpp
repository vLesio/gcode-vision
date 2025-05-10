#include "SimulationModeFactory.h"
#include "FixedSizeSimulation.h"
#include "AdaptiveScaleSimulation.h"

ISimulationMode* SimulationModeFactory::createMode(const std::string& name, float resolution) {
    if (name == "fixed") {
        return new FixedSizeSimulation(resolution);
    }
    else if (name == "adaptive") {
        return new AdaptiveScaleSimulation();
    }
    else {
        return nullptr;
    }
}
