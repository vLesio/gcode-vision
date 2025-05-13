#ifndef ISIMULATION_MODE_H
#define ISIMULATION_MODE_H

#include <vector>
#include <string>
#include "filamentSimulator.h"
#include "GCodeTypes.h"
#include "SimulationContext.h"


class ISimulationMode {
public:
    virtual ~ISimulationMode() = default;
    virtual void simulate(const SimulationContext& context, FilamentSimulator& simulator) = 0;
    virtual void simulateStep(const SimulationContext& context, FilamentSimulator& simulator, const PrintStep& step) = 0;
    virtual void simulatePartialStep(const SimulationContext& context, FilamentSimulator& simulator, const PrintStep& step, float progress) = 0;
	virtual void simulateStepsInRange(const SimulationContext& context, FilamentSimulator& simulator, size_t count) = 0;

    virtual std::string getName() const = 0;
};

#endif // ISIMULATION_MODE_H
