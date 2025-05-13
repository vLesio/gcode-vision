#ifndef FILAMENT_SIMULATOR_H
#define FILAMENT_SIMULATOR_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "InstancedObject.h"
#include "GCodeTypes.h"

class FilamentSimulator {
public:
    FilamentSimulator(InstancedObject* target);

    void addSegment(glm::vec3 position, glm::vec3 scale, glm::quat rotation);

    void clear();
    void resetSimulation();

    void attachTemporaryObject(InstancedObject* temp);
    void setTemporarySegment(glm::vec3 position, glm::vec3 scale, glm::quat rotation);
    void clearTemporarySegment();

    glm::vec3 computeCentroid(const std::vector<PrintStep>& steps);

private:
    InstancedObject* target = nullptr;
    InstancedObject* tempSegmentObject = nullptr;
    bool hasTempSegment = false;
};

#endif
