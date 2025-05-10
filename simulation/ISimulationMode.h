#ifndef ISIMULATION_MODE_H
#define ISIMULATION_MODE_H

#include <vector>
#include "GCodeTypes.h"
#include "FilamentSimulator.h"

class ISimulationMode {
public:
    virtual void simulate(const std::vector<PrintStep>& steps, FilamentSimulator& simulator) = 0;
    virtual ~ISimulationMode() = default;
    virtual std::string getName() const = 0;
};

#endif
