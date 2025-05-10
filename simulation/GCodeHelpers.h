#ifndef GCODE_HELPERS_H
#define GCODE_HELPERS_H

#include "GCodeTypes.h"
#include <vector>


void interpolateArc(MachineState& state, const GCodeCommand& command, bool clockwise, std::vector<PrintStep>& steps);
void addHomingStep(MachineState& state, const GCodeCommand& command, std::vector<PrintStep>& steps);
void moveStep(MachineState& state, const GCodeCommand& command, std::vector<PrintStep>& steps);


#endif // GCODE_HELPERS_H