#include "GCodeHelpers.h"

#include <algorithm>

constexpr float M_PI = 3.14159265358979323846f;


void moveStep(MachineState& state, const GCodeCommand& command, std::vector<PrintStep>& steps) {
    float newX = state.X;
    float newY = state.Y;
    float newZ = state.Z;
    float newE = state.E;

    if (command.parameters.count('X')) {
        float val = command.parameters.at('X');
        newX = state.absolutePositioning ? val : state.X + val;
    }
    if (command.parameters.count('Y')) {
        float val = command.parameters.at('Y');
        newY = state.absolutePositioning ? val : state.Y + val;
    }
    if (command.parameters.count('Z')) {
        float val = command.parameters.at('Z');
        newZ = state.absolutePositioning ? val : state.Z + val;
    }
    if (command.parameters.count('E')) {
        float val = command.parameters.at('E');
        newE = state.absoluteExtrusion ? val : state.E + val;
    }

    float extrusionAmount = newE - state.prevE;
    bool isG1 = command.command == "G1";
    bool extruding = isG1 && (extrusionAmount > 0.0f);

    PrintStep step;
    step.startPosition = { state.X, state.Y, state.Z };
    step.endPosition = { newX, newY, newZ };
    step.extrusionAmount = extruding ? extrusionAmount : 0.0f;
    step.speed = command.parameters.count('F') ? command.parameters.at('F') : 0.0f;
    step.extruding = extruding;
    step.snapshot = state;
    step.type = isG1 ? PrintStepType::Linear : PrintStepType::Rapid;

    steps.push_back(step);

    state.X = newX;
    state.Y = newY;
    state.Z = newZ;
    state.E = newE;
    state.prevE = newE;
}


void addHomingStep(MachineState& state, const GCodeCommand& command, std::vector<PrintStep>& steps) {
    glm::vec3 start = { state.X, state.Y, state.Z };
    glm::vec3 end = start;
    MachineState stateBefore = state;

    if (!command.parameters.count('X') && !command.parameters.count('Y') && !command.parameters.count('Z')) {
        end = { 0.0f, 0.0f, 0.0f };
        state.X = state.Y = state.Z = 0.0f;
    }
    else {
        if (command.parameters.count('X')) { end.x = 0.0f; state.X = 0.0f; }
        if (command.parameters.count('Y')) { end.y = 0.0f; state.Y = 0.0f; }
        if (command.parameters.count('Z')) { end.z = 0.0f; state.Z = 0.0f; }
    }

    state.prevE = state.E;

    PrintStep step;
    step.startPosition = start;
    step.endPosition = end;
    step.extrusionAmount = 0.0f;
    step.speed = 0.0f;
    step.extruding = false;
    step.snapshot = stateBefore;
    step.type = PrintStepType::Rapid;

    steps.push_back(step);
}

void interpolateArc(MachineState& state, const GCodeCommand& command, bool clockwise, std::vector<PrintStep>& steps) {
    float x = state.X;
    float y = state.Y;
    float z = state.Z;
    float eStart = state.E;

    float xEnd = command.parameters.count('X') ? command.parameters.at('X') : x;
    float yEnd = command.parameters.count('Y') ? command.parameters.at('Y') : y;
    float i = command.parameters.count('I') ? command.parameters.at('I') : 0.0f;
    float j = command.parameters.count('J') ? command.parameters.at('J') : 0.0f;

    float centerX = x + i;
    float centerY = y + j;

    float radius = std::hypot(x - centerX, y - centerY);
    float startAngle = std::atan2(y - centerY, x - centerX);
    float endAngle = std::atan2(yEnd - centerY, xEnd - centerX);

    if (clockwise && endAngle > startAngle) endAngle -= 2 * M_PI;
    if (!clockwise && endAngle < startAngle) endAngle += 2 * M_PI;

    int segments = static_cast<int>(std::ceil(std::abs(endAngle - startAngle) * 180.0f / M_PI));
    segments = std::max(segments, 1);

    float angleStep = (endAngle - startAngle) / segments;
    float eEnd = command.parameters.count('E') ? command.parameters.at('E') : eStart;

    float prevNe = eStart;

    for (int i = 1; i <= segments; ++i) {
        float angle = startAngle + angleStep * i;
        float nx = centerX + radius * std::cos(angle);
        float ny = centerY + radius * std::sin(angle);
        float nz = command.parameters.count('Z') ? command.parameters.at('Z') : z;
        float ne = eStart + (eEnd - eStart) * (static_cast<float>(i) / segments);

        PrintStep step;
        step.startPosition = { x, y, z };
        step.endPosition = { nx, ny, nz };
        step.speed = command.parameters.count('F') ? command.parameters.at('F') : 0.0f;

        if (command.parameters.count('E')) {
            step.extrusionAmount = ne - prevNe;
            step.extruding = step.extrusionAmount > 0.0f;
        }
        else {
            step.extrusionAmount = 0.0f;
            step.extruding = false;
        }

		step.snapshot = state; // To keep things simple, we use the same snapshot for all steps
        step.type = PrintStepType::Arc;

        steps.push_back(step);
        x = nx;
        y = ny;
        z = nz;
        prevNe = ne;
    }

    state.X = xEnd;
    state.Y = yEnd;
    if (command.parameters.count('Z')) state.Z = command.parameters.at('Z');
    if (command.parameters.count('E')) {
        state.E = eEnd;
        state.prevE = eEnd;
    }
}


