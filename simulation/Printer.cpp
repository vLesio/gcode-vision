#include "Printer.h"
#include <stdio.h>

Printer::Printer(SceneObject* printerObject)
{
	printerRootObject = printerObject;
	bed = std::make_unique<Bed>(printerRootObject->children[0]);
	setNozzlePosition(0, 0, 0);
}

Printer* Printer::setNozzlePosition(float x, float y, float z)
{
	cutCoordinatesToBox(x, y, z);
	float xOffset = 92;
	//float yOffset = 113;
	float yOffset = 106.5;
	float zOffset = 89.5;
	bed->setNozzlePosition(x-xOffset,y-yOffset,z-zOffset);
	return this;
}

Printer* Printer::setPrinterPosition(glm::vec3 vec)
{
	printerRootObject->localTransform.translate(vec); // Center the model
	return this;
}

Printer* Printer::setPrinterRotation(float rad, glm::vec3 vec)
{
	printerRootObject->localTransform.rotate(rad, vec); // Rotate to match the ground plane
	return this;
}

Printer* Printer::setPrinterMaterial(Material* material)
{
	printerRootObject->setMaterialApplyToChildren(material);
	return this;
}

Printer* Printer::setPrinterScale(glm::vec3 vec)
{
	printerRootObject->localTransform.scale(vec); // Scale down the model
	return this;
}

Printer* Printer::addToScene(Scene* scene)
{
	scene->add(printerRootObject);
	return this;
}

void Printer::cutCoordinatesToBox(float& x, float& y, float& z) {
	float xMin = 0;
	float xMax = 240;
	float yMin = 0;
	float yMax = 200;
	float zMin = 0;
	float zMax = 230;
	if (x < xMin) {
		x = xMin;
	}
	if (y < yMin) {
		y = yMin;
	}	
	if (z < zMin) {
		z = zMin;
	}
	if (x > xMax) {
		x = xMax;
	}
	if (y > yMax) {
		y = yMax;
	}	
	if (z > zMax) {
		z = zMax;
	}
}
