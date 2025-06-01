#ifndef PRINTER_H
#define PRINTER_H

#include "sceneObject.h"
#include "Bed.h"
#include "Material.h"
#include "scene.h"

class Printer
{
private:
	std::unique_ptr<Bed> bed;
	SceneObject* printerRootObject;

	void cutCoordinatesToBox(float & x, float & y, float & z);
public:
	Printer(SceneObject*);
	Printer* setNozzlePosition(float x, float y, float z);
	Printer* setPrinterPosition(glm::vec3);
	Printer* setPrinterRotation(float, glm::vec3);
	Printer* setPrinterMaterial(Material*);
	Printer* setPrinterScale(glm::vec3);
	Printer* addToScene(Scene*);

};
#endif