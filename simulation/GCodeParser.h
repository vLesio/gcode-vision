#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include "GCodeTypes.h"


// GCode parser class
class GCodeParser {
	public:
		bool loadFile(const std::string& filename); // Load GCode file
		std::vector<PrintStep>& getPrintSteps(); // Get parsed print steps
	private:
		MachineState machineState; // Current state of the machine
		std::vector<PrintStep> printSteps; // List of parsed print steps
};
#endif // GCODE_PARSER_H