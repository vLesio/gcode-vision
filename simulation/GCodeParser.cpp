#include "GCodeParser.h"
#include "GCodeHelpers.h"

#include <filesystem>
#include <iostream>
#include <set>
#include <sstream>
#include <fstream>


GCodeCommand GCodeCommand::parse(const std::string& line)
{
	GCodeCommand command;
	std::string trimmedLine = line;
	// Remove comments
	size_t commentPos = trimmedLine.find(';');
	if (commentPos != std::string::npos) {
		trimmedLine = trimmedLine.substr(0, commentPos);
	}

	if (trimmedLine.empty()) return command; // Skip empty lines

	std::istringstream iss(trimmedLine);
	std::string currentToken;

	const std::set<char> allowedParams = { 'X', 'Y', 'Z', 'E', 'F', 'I', 'J' };


	while (iss >> currentToken) {
		if (currentToken.empty()) continue; // Skip empty tokens

		char prefix = std::toupper(currentToken[0]); // Command or parameter prefix e.e. G, M / X, Y, Z, E, F, I, J
		std::string value = currentToken.substr(1); // Value after the prefix

		if (prefix == 'M' || prefix == 'G')
		{
			command.command = prefix + value; // Store the command
		}
		else if (allowedParams.count(prefix) > 0)
		{
			try {
				float paramValue = std::stof(value); // Convert the value to float
				command.parameters[prefix] = paramValue; // Store the parameter
			}
			catch (const std::invalid_argument&) {
				std::cerr << "Invalid parameter value: " << value << std::endl;
			}
			catch (const std::out_of_range&) {
				std::cerr << "Parameter value out of range: " << value << std::endl;
			}
		}
		else
		{
			std::cerr << "Parsing failed, unknown command or parameter: " << currentToken << std::endl;
		}
	}

	return command;
}


bool GCodeParser::loadFile(const std::string& filename) {
	std::ifstream infile(filename);
	if (!infile.is_open()) return false;

	std::string line;
	while (std::getline(infile, line)) {
		try {
			GCodeCommand cmd = GCodeCommand::parse(line);
			machineState.updateFromCommand(cmd, printSteps);
		}
		catch (const std::exception& ex) {
			std::cerr << "Error while parsing GCode line " << line << "\n" << ex.what() << std::endl;
		}
	}
	return true;
}

std::vector<PrintStep>& GCodeParser::getPrintSteps()
{
	return printSteps;
}

void MachineState::updateFromCommand(const GCodeCommand& command, std::vector<PrintStep>& steps)
{
	if (command.command.empty()) return; // Skip empty commands

	/// Machine control commands

	if (command.command == "G90") absolutePositioning = true;				// Set absolute positioning
	else if (command.command == "G91") absolutePositioning = false;			// Set relative positioning
	else if (command.command == "M82") absoluteExtrusion = true;			// Set absolute extrusion
	else if (command.command == "M83") absoluteExtrusion = false;			// Set relative extrusion
	else if (command.command == "G92") {									// Force set position - it does not move the machine but tells it where it is (it could be different from the real position)
		if (command.parameters.count('X')) X = command.parameters.at('X');
		if (command.parameters.count('Y')) Y = command.parameters.at('Y');
		if (command.parameters.count('Z')) Z = command.parameters.at('Z');
		if (command.parameters.count('E')) {
			E = command.parameters.at('E');
			prevE = E;
		}
		return;
	}
	else if (command.command == "G0" || command.command == "G1") {						// Linear movement
		moveStep(*this, command, steps);
	}
	else if (command.command == "G28")												// Return to home position
	{
		addHomingStep(*this, command, steps);
	}
	else if (command.command == "G2" || command.command == "G3") {					// Arc movement
		interpolateArc(*this, command, command.command == "G2", steps);
	}
	else
	{
		std::cerr << "This command is no currently supported: " << command.command << std::endl;
	}
}

