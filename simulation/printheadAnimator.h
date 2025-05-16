#ifndef PRINTHEAD_ANIMATOR_H
#define PRINTHEAD_ANIMATOR_H

#include "SceneObject.h"
#include "Scene.h"

class PrintheadAnimator {
public:
    PrintheadAnimator(float simulationScale, glm::vec3 startPosition = glm::vec3(0.0f));
    ~PrintheadAnimator();

    void initialize();
    void attachToScene(Scene* scene);
    void removeFromScene(Scene* scene);

    void update(const glm::vec3& position);
    void updateInterpolated(const glm::vec3& from, const glm::vec3& to, float progress);

    void reset();
    glm::vec3 getCurrentPosition() const { return currentPosition; }
	void setSimulationScale(float scale) { simulationScale = scale; }
private:
    SceneObject* nozzleObject = nullptr;
    Scene* currentScene = nullptr;
    float simulationScale;
    glm::vec3 currentPosition;
	glm::vec3 startPosition;
	bool isInitialized = false;
};

#endif
