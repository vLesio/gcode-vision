#ifndef ADAPTIVESCALESIMULATION_H
#define ADAPTIVESCALESIMULATION_H

#include "ISimulationMode.h"

class AdaptiveScaleSimulation : public ISimulationMode {
public:
	virtual void simulate(const SimulationContext& context, FilamentSimulator& simulator) override;
	virtual void simulateStep(const SimulationContext& context, FilamentSimulator& sim, const PrintStep& step) override;
	std::string getName() const override { return "adaptive"; }
};

#endif