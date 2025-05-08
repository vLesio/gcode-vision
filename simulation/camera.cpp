#include "camera.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <chrono>

// Singleton instance
static std::unique_ptr<Camera> cameraInstance = nullptr;

void Camera::init(int width, int height, glm::vec3 target, float yaw, float pitch, float distance, float zoomSpeed, float rotateSpeed)
{
    if (!cameraInstance)
    {
        cameraInstance.reset(new Camera(width, height, target));
        cameraInstance->yaw = yaw;
        cameraInstance->pitch = pitch;
        cameraInstance->distance = distance;

        cameraInstance->defaultYaw = yaw;
        cameraInstance->defaultPitch = pitch;
        cameraInstance->defaultDistance = distance;
        cameraInstance->defaultTarget = target;

        cameraInstance->zoomSpeed = zoomSpeed;
        cameraInstance->rotateSpeed = rotateSpeed;

        cameraInstance->updatePosition();
    }
    else
    {
        std::cerr << "[Camera] Warning: Already initialized!" << std::endl;
    }
}

Camera& Camera::getInstance()
{
    if (!cameraInstance)
    {
        throw std::runtime_error("[Camera] Tried to use before initialization!");
    }
    return *cameraInstance;
}

Camera::Camera(int width, int height, glm::vec3 target)
{
    this->width = width;
    this->height = height;
    this->target = target;
    updatePosition();
}

void Camera::updatePosition()
{
    pitch = std::min(pitch, 89.0f);
    pitch = std::max(pitch, -89.0f);

    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    Position.x = target.x + distance * cos(pitchRad) * sin(yawRad);
    Position.y = target.y + distance * sin(pitchRad);
    Position.z = target.z + distance * cos(pitchRad) * cos(yawRad);
}

void Camera::reset()
{
    yaw = defaultYaw;
    pitch = defaultPitch;
    distance = defaultDistance;
    target = defaultTarget;
    updatePosition();
}

void Camera::setTarget(const glm::vec3& newTarget)
{
    target = newTarget;
    updatePosition();
}

void Camera::rotate(float yawOffset, float pitchOffset)
{
    yaw += yawOffset;
    pitch += pitchOffset;
    updatePosition();
}

void Camera::zoom(float offset)
{
    distance -= offset;
    distance = std::max(distance, 1.0f);
    distance = std::min(distance, 100.0f);
    updatePosition();
}

void Camera::computeCameraMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    glm::mat4 view = glm::lookAt(Position, target, Up);
    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
    cameraMatrix = projection * view;
}

void Camera::uploadToShader(Shader& shader, const char* uniform)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::applyToShader(Shader& shader, const char* uniform, float FOVdeg, float nearPlane, float farPlane)
{
    shader.Activate();
    computeCameraMatrix(FOVdeg, nearPlane, farPlane);
    uploadToShader(shader, uniform);
}

void Camera::up() { rotate(0.0f, rotateSpeed); }
void Camera::down() { rotate(0.0f, -rotateSpeed); }
void Camera::right() { rotate(rotateSpeed, 0.0f); }
void Camera::left() { rotate(-rotateSpeed, 0.0f); }
void Camera::zoomIn() { zoom(zoomSpeed); }
void Camera::zoomOut() { zoom(-zoomSpeed); }

void Camera::toggleMode() {
    mode = (mode == CameraMode::Orbit) ? CameraMode::Free : CameraMode::Orbit;
}

CameraMode Camera::getMode() const {
    return mode;
}

void Camera::processFreeMovement(GLFWwindow* window, float deltaTime) {
    glm::vec3 forward = glm::normalize(target - Position);
    glm::vec3 right = glm::normalize(glm::cross(forward, Up));
    glm::vec3 movement(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement -= forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement += right;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) movement += Up;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) movement -= Up;

    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
        float speed = freeSpeed * deltaTime;
        Position += movement * speed;
        target += movement * speed;
    }
}

void Camera::keyboardInputs(GLFWwindow* window)
{
    static bool tabPressedLastFrame = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (!tabPressedLastFrame) {
            toggleMode();
            std::cout << "[Camera] Switched to " << (mode == CameraMode::Orbit ? "Orbit" : "Free") << " mode.\n";
        }
        tabPressedLastFrame = true;
    } else {
        tabPressedLastFrame = false;
    }

    if (mode == CameraMode::Orbit) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) rotate(0.0f, 0.05f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) rotate(0.0f, -0.05f);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) rotate(-0.05f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) rotate(0.05f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) zoom(-0.01f);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) zoom(0.01f);
    } else if (mode == CameraMode::Free) {
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        processFreeMovement(window, deltaTime);
    }
}
