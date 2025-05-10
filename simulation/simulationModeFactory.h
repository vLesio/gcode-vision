#ifndef SIMULATION_MODE_FACTORY_H
#define SIMULATION_MODE_FACTORY_H

#include <string>
#include "ISimulationMode.h"

class SimulationModeFactory {
public:
    static ISimulationMode* createMode(const std::string& name);
};

#endif
