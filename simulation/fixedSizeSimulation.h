#ifndef FIXED_SIZE_SIMULATION_H
#define FIXED_SIZE_SIMULATION_H

#include "ISimulationMode.h"

class FixedSizeSimulation : public ISimulationMode {
public:
    virtual void simulate(const SimulationContext& context, FilamentSimulator& simulator) override;
	virtual void simulateStep(const SimulationContext& context, FilamentSimulator& sim, const PrintStep& step) override;
	std::string getName() const override { return "fixed"; }
};

#endif
