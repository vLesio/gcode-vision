#include "SimulationModeFactory.h"
#include "FixedSizeSimulation.h"
#include "AdaptiveScaleSimulation.h"

ISimulationMode* SimulationModeFactory::createMode(const std::string& name) {
    if (name == "fixed") {
        return new FixedSizeSimulation();
    }
    else if (name == "adaptive") {
        return new AdaptiveScaleSimulation();
    }
    else {
        return nullptr;
    }
}
