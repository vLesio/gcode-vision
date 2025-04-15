#ifndef GCODE_TYPES_H
#define GCODE_TYPES_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <vector>

struct PrintStep;
struct MachineState;

// Parsed GCode command structure in unprocessed form
struct GCodeCommand {
	std::string command;
	std::map<char, float> parameters;

	static GCodeCommand parse(const std::string& line);

	// Debug
	std::string toString() const {
		std::string result = command + " ";
		for (const auto& param : parameters) {
			result += param.first + std::to_string(param.second) + " ";
		}
		return result;
	}
};

// Structure to hold the state of the machine updated by the GCode commands
struct MachineState
{
	// Current position of the 3D printer nozzle
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;

	float E = 0.0f; // Current extrusion amount

	bool absolutePositioning = true; // Flag for absolute positioning
	bool absoluteExtrusion = true; // Flag for absolute extrusion

	float prevE = 0.0f; // Previous extrusion amount - used for calculating extrusion length at each step

	void updateFromCommand(const GCodeCommand& command, std::vector<PrintStep>& steps);

	// Debug
	std::string toString() const {
		return "/ ---- Machine State ---- \\\nX: " + std::to_string(X) + ", Y: " + std::to_string(Y) + ", Z: " + std::to_string(Z) +
			"\nE: " + std::to_string(E) + ", absolutePositioning: " + std::to_string(absolutePositioning) +
			", absoluteExtrusion: " + std::to_string(absoluteExtrusion) + "\n\\ ---- Machine State ---- /\n";
	}
};

enum class PrintStepType {
	Rapid,      // G0 - Fastest movement (no extrusion) (G28 homing too)
	Linear,     // G1 - Linear movement
	Arc,        // G2 / G3 - Arc movement
	Other       // For any other command that doesn't fit the above categories
};

// Structure to hold the parsed GCode commands in a form that can be used for display
struct PrintStep {
	glm::vec3 startPosition;
	glm::vec3 endPosition;
	float extrusionAmount;
	float speed;
	bool extruding;

	PrintStepType type; // Type of the print step (e.g., rapid, linear, arc, etc.)
	MachineState snapshot; // Snapshot of the machine state before this step

	//Debug
	std::string toString() const {
		return
		"/== PRINT STEP ==\\\n\nStart: (X:" + std::to_string(startPosition.x) + ", Y:" + std::to_string(startPosition.y) + ", Z:" + std::to_string(startPosition.z) +
			")\nEnd: (X:" + std::to_string(endPosition.x) + ", Y:" + std::to_string(endPosition.y) + ", Z:" + std::to_string(endPosition.z) +
			")\nExtrusion Amount (mm): " + std::to_string(extrusionAmount) +
			"\nCurrently extruding: " + std::to_string(extruding) +
			"\nSpeed: " + std::to_string(speed) +
			"\nMovement type (0 - Rapid, 1 - Linear, 2 - Arc, 3 - Other): " + std::to_string(static_cast<int>(type)) + 
			"\nMachine state before this step: \n" + snapshot.toString() +
			"\n\\== PRINT STEP ==/\n";
	}
};


#endif
