#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

// Camera control modes
enum class CameraMode {
    Orbit,
    Free
};

class Camera
{
public:
    // Singleton access
    static void init(glm::vec3 target = glm::vec3(0.0f),
        float yaw = 0.0f, float pitch = 0.0f, float distance = 5.0f, float zoomSpeed = 0.01f, float rotateSpeed = 0.1f);
    static Camera& getInstance();

    // Camera control
    void rotate(float yawOffset, float pitchOffset);
    void zoom(float offset);
    void setTarget(const glm::vec3& newTarget);
    void setOnlyTarget(const glm::vec3& newTarget);
    void moveCameraWithBed(float offset);
    void reset();

    // Helper control
    void left();
    void right();
    void up();
    void down();
    void zoomIn();
    void zoomOut();

    // Shader upload and camera matrix computation
    void computeCameraMatrix(float FOVdeg, float nearPlane, float farPlane, int width, int height);
    void uploadToShader(Shader& shader);
    void applyToShader(Shader& shader, float FOVdeg, float nearPlane, float farPlane, int screenWidth, int screenHeight);

	// Helper function to read inputs from keyboard
    void keyboardInputs(GLFWwindow* window);

    // Free camera movement
    void toggleMode();                        // Switch Orbit/Free mode
    void processFreeMovement(GLFWwindow* window, float deltaTime);
    void processMouseMovement(float xpos, float ypos);
    CameraMode getMode() const;


	float minY = 0.05f; // Minimum Y position to prevent camera from going below ground level
	float maxY = 30.0f; // Maximum Y position 
	float minX = -30.0f; // Minimum X position
	float maxX = 30.0f; // Maximum X position
	float minZ = -30.0f; // Minimum Z position
	float maxZ = 30.0f; // Maximum Z position

    void setAll(glm::vec3 target, float yaw, float pitch, float distance, float zoomSpeed, float rotateSpeed);


private:
    Camera(glm::vec3 target);

    // Block copy and assignment
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    void updatePositionOrbit();
	glm::vec3 getForwardVector() const;
	void faceTarget();

    glm::vec3 target;
    glm::vec3 position;
    glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = 0.0f;
    float pitch = 0.0f;
    float distance = 5.0f;

    glm::mat4 cameraMatrix = glm::mat4(1.0f);

	// Default values, used for resetting the camera
    glm::vec3 defaultTarget;
    float defaultYaw = 0.0f;
    float defaultPitch = 0.0f;
    float defaultDistance = 5.0f;

	// Last values, used when switching modes back to Orbit
    glm::vec3 lastOrbitTarget;
    float lastOrbitYaw;
    float lastOrbitPitch;
    float lastOrbitDistance;

    float zoomSpeed = 0.01f;
    float rotateSpeed = 0.1f;

    // Free camera mode
    CameraMode mode = CameraMode::Orbit;
    float freeSpeed = 2.0f;
    bool firstMouse = true;
    float lastX = 0.0f, lastY = 0.0f;
    float mouseSensitivity = 0.1f;

    void ensureOrbitMode() const;
};

#endif
