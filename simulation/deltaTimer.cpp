#include "DeltaTimer.h"
#include <GLFW/glfw3.h>

DeltaTimer::DeltaTimer() {
    lastTime = glfwGetTime();
}

float DeltaTimer::tick() {
    if (paused)
        return 0.0f;

    double currentTime = glfwGetTime();
    float delta = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    return delta;
}

void DeltaTimer::pause() {
    if (!paused) {
        pauseTime = glfwGetTime();
        paused = true;
    }
}

void DeltaTimer::resume() {
    if (paused) {
        double currentTime = glfwGetTime();
        lastTime += (currentTime - pauseTime); 
        paused = false;
    }
}

bool DeltaTimer::isPaused() const {
    return paused;
}

void DeltaTimer::reset() {
    lastTime = glfwGetTime();
    paused = false;
    pauseTime = 0.0;
}
