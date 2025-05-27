#include "FilamentSimulator.h"
#include "camera.h"
#include <iostream>
#include <cfloat>

FilamentSimulator::FilamentSimulator(InstancedObject* target)
    : target(target) {
}

void FilamentSimulator::attachTemporaryObject(InstancedObject* temp) {
    tempSegmentObject = temp;
}

void FilamentSimulator::addSegment(glm::vec3 position, glm::vec3 scale, glm::quat rotation) {
    if (target)
        target->appendDynamicInstance(position, scale, rotation);
}

void FilamentSimulator::setTemporarySegment(glm::vec3 position, glm::vec3 scale, glm::quat rotation) {
    if (!tempSegmentObject) return;

    tempSegmentObject->clear();
	tempSegmentObject->appendDynamicInstance(position, scale, rotation);
    hasTempSegment = true;
}

void FilamentSimulator::clearTemporarySegment() {
    if (!tempSegmentObject) return;

    tempSegmentObject->clear();
    hasTempSegment = false;
}

void FilamentSimulator::clear() {
    if (target)
        target->clear();

    if (tempSegmentObject)
        tempSegmentObject->clear();

    hasTempSegment = false;
}


