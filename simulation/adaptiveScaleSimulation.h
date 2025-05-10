#ifndef ADAPTIVESCALESIMULATION_H
#define ADAPTIVESCALESIMULATION_H

#include "ISimulationMode.h"

class AdaptiveScaleSimulation : public ISimulationMode {
public:
    void simulate(const std::vector<PrintStep>& steps, FilamentSimulator& simulator) override;
	std::string getName() const override { return "adaptive"; }
};

#endif