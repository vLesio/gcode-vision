#ifndef PRINTHEAD_ANIMATOR_H
#define PRINTHEAD_ANIMATOR_H

#include "SceneObject.h"
#include "Scene.h"
#include "Printer.h"

class PrintheadAnimator {
public:
    PrintheadAnimator(float simulationScale, glm::vec3 startPosition = glm::vec3(0.0f));
    ~PrintheadAnimator();

    void initialize();
    void attachToScene(Scene* scene);
    void removeFromScene(Scene* scene);

    void update(const glm::vec3& position);
    void updateInterpolated(const glm::vec3& from, const glm::vec3& to, float progress);
	void step(const PrintStep& step);

    void reset();
    glm::vec3 getCurrentPosition() const { return currentPosition; }
	void setSimulationScale(float scale) { simulationScale = scale; }
    void attachPrinter(std::shared_ptr<Printer> printer);
private:
    SceneObject* nozzleObject = nullptr;
    Scene* currentScene = nullptr;
    float simulationScale;
    glm::vec3 currentPosition;
	glm::vec3 startPosition;
	bool isInitialized = false;
    std::shared_ptr<Printer> printer;
};

#endif
