#include "camera.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <chrono>

#include "exceptions.h"

// Singleton instance
static std::unique_ptr<Camera> cameraInstance = nullptr;

void Camera::init(glm::vec3 target, float yaw, float pitch, float distance, float zoomSpeed, float rotateSpeed)
{
    if (!cameraInstance)
    {
        cameraInstance.reset(new Camera(target));
        cameraInstance->yaw = yaw;
        cameraInstance->pitch = pitch;
        cameraInstance->distance = distance;

        cameraInstance->defaultYaw = yaw;
        cameraInstance->defaultPitch = pitch;
        cameraInstance->defaultDistance = distance;
        cameraInstance->defaultTarget = target;

        cameraInstance->zoomSpeed = zoomSpeed;
        cameraInstance->rotateSpeed = rotateSpeed;

        cameraInstance->updatePositionOrbit();
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

Camera::Camera(glm::vec3 target)
{
    this->target = target;
    updatePositionOrbit();
}

void Camera::updatePositionOrbit()
{
    pitch = std::min(pitch, 89.0f);
    pitch = std::max(pitch, -89.0f);

    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    Position.x = target.x + distance * cos(pitchRad) * sin(yawRad);
    Position.y = target.y + distance * sin(pitchRad);
    Position.z = target.z + distance * cos(pitchRad) * cos(yawRad);
}

void Camera::ensureOrbitMode() const
{
    if (mode != CameraMode::Orbit) {
        throw InvalidCameraModeException("[Camera]: Operation only valid in Orbit mode.");
    }
}

void Camera::reset()
{
	mode = CameraMode::Orbit;
    yaw = defaultYaw;
    pitch = defaultPitch;
    distance = defaultDistance;
    target = defaultTarget;
    updatePositionOrbit();
}

void Camera::setTarget(const glm::vec3& newTarget) {
    target = newTarget;
    if (mode == CameraMode::Orbit) {
        lastOrbitTarget = newTarget;
        lastOrbitYaw = yaw;
        lastOrbitPitch = pitch;
        lastOrbitDistance = distance;
    }
    updatePositionOrbit();
}

void Camera::rotate(float yawOffset, float pitchOffset)
{
    yaw += yawOffset;
    pitch += pitchOffset;
    updatePositionOrbit();
}

void Camera::zoom(float offset)
{
    distance -= offset;
    distance = std::max(distance, 0.01f);
    distance = std::min(distance, 100.0f);
    updatePositionOrbit();
}

void Camera::computeCameraMatrix(float FOVdeg, float nearPlane, float farPlane, int width, int height)
{
    glm::mat4 view;

    if (mode == CameraMode::Free) {
        glm::vec3 forward = getForwardVector();
        view = glm::lookAt(Position, Position + forward, Up);
    }
    else {
        view = glm::lookAt(Position, target, Up);
    }

    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
    cameraMatrix = projection * view;
}

glm::vec3 Camera::getForwardVector() const {
    glm::vec3 forward;
    forward.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    forward.y = sin(glm::radians(pitch));
    forward.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    return glm::normalize(forward);
}

void Camera::uploadToShader(Shader& shader, const char* uniform)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::applyToShader(Shader& shader, const char* uniform, float FOVdeg, float nearPlane, float farPlane, int screenWidth, int screenHeight)
{
    shader.Activate();
	computeCameraMatrix(FOVdeg, nearPlane, farPlane, screenWidth, screenHeight);
	uploadToShader(shader, uniform);
}

// Helper functions for camera control - used in REST API
void Camera::up() { ensureOrbitMode(); rotate(0.0f, rotateSpeed); }
void Camera::down() { ensureOrbitMode(); rotate(0.0f, -rotateSpeed); }
void Camera::right() { ensureOrbitMode(); rotate(rotateSpeed, 0.0f); }
void Camera::left() { ensureOrbitMode(); rotate(-rotateSpeed, 0.0f); }
void Camera::zoomIn() { ensureOrbitMode(); zoom(zoomSpeed); }
void Camera::zoomOut() { ensureOrbitMode(); zoom(-zoomSpeed); }

void Camera::toggleMode() {
    if (mode == CameraMode::Free) {
        target = lastOrbitTarget;
        yaw = lastOrbitYaw;
        pitch = lastOrbitPitch;
        distance = lastOrbitDistance;
        updatePositionOrbit();
    }
    else if (mode == CameraMode::Orbit) {
        lastOrbitTarget = target;
        lastOrbitYaw = yaw;
        lastOrbitPitch = pitch;
        lastOrbitDistance = distance;

        faceTarget();
    }
    mode = (mode == CameraMode::Orbit) ? CameraMode::Free : CameraMode::Orbit;
}

CameraMode Camera::getMode() const {
    return mode;
}

void Camera::processFreeMovement(GLFWwindow* window, float deltaTime) {
    glm::vec3 forward = getForwardVector();
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
    }
}

void Camera::keyboardInputs(GLFWwindow* window)
{
    static bool tabPressedLastFrame = false;

	// Delta time for frame-rate independent movement
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

	// switch mode on tab press
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (!tabPressedLastFrame) {
            toggleMode();
            std::cout << "[Camera] Switched to " << (mode == CameraMode::Orbit ? "Orbit" : "Free") << " mode.\n";
        }
        tabPressedLastFrame = true;
    }
    else {
        tabPressedLastFrame = false;
    }

    if (mode == CameraMode::Orbit) {
		float rotationSpeed = 90.0f; // degrees per second
		float zoomSpeed = 5.0f; // units per second

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) rotate(0.0f, rotationSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) rotate(0.0f, -rotationSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) rotate(-rotationSpeed * deltaTime, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) rotate(rotationSpeed * deltaTime, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) zoom(-zoomSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) zoom(zoomSpeed * deltaTime);
    }
    else if (mode == CameraMode::Free) {
        processFreeMovement(window, deltaTime);
    }
}

void Camera::processMouseMovement(float xpos, float ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::clamp(pitch, -89.0f, 89.0f);
}

void Camera::faceTarget() {
    glm::vec3 direction = glm::normalize(target - Position);
    yaw = glm::degrees(atan2(direction.x, direction.z));
    pitch = glm::degrees(asin(direction.y));
}