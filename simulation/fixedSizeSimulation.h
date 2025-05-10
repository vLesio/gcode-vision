#ifndef FIXED_SIZE_SIMULATION_H
#define FIXED_SIZE_SIMULATION_H

#include "ISimulationMode.h"

class FixedSizeSimulation : public ISimulationMode {
public:
    explicit FixedSizeSimulation(float resolution);

    void simulate(const std::vector<PrintStep>& steps, FilamentSimulator& simulator) override;
	std::string getName() const override { return "fixed"; }

private:
    float resolution;
};

#endif
