#ifndef ISIMULATION_MODE_H
#define ISIMULATION_MODE_H

#include <vector>
#include "GCodeTypes.h"
#include "filamentSimulator.h"
#include "simulationContext.h"

class ISimulationMode {
public:
    virtual void simulate(const SimulationContext& context, FilamentSimulator& simulator) = 0;
    virtual void simulateStep(const SimulationContext& context, FilamentSimulator& sim, const PrintStep& step) = 0;
    virtual ~ISimulationMode() = default;
    virtual std::string getName() const = 0;
};

#endif
