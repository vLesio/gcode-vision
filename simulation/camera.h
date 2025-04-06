#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"

class Camera
{
public:
    // Singleton access
    static void init(int width, int height, glm::vec3 target = glm::vec3(0.0f),
		float yaw = 0.0f, float pitch = 0.0f, float distance = 5.0f, float zoomSpeed = 0.01f, float rotateSpeed = 0.1f);
    static Camera& getInstance();

	// Camera control
    void rotate(float yawOffset, float pitchOffset);
    void zoom(float offset);
    void setTarget(const glm::vec3& newTarget);
    void reset();

    //Camera helper control
    void left();
	void right();
	void up();
	void down();
    void zoomIn();
	void zoomOut();

	// Shader upload and camera matrix computation
    void computeCameraMatrix(float FOVdeg, float nearPlane, float farPlane);
    void uploadToShader(Shader& shader, const char* uniform);
    void applyToShader(Shader& shader, const char* uniform, float FOVdeg, float nearPlane, float farPlane);

	void keyboardInputs(GLFWwindow* window);

private:
    Camera(int width, int height, glm::vec3 target);

	// Block copy and assignment
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    void updatePosition();

    int width, height;

    glm::vec3 target;
    glm::vec3 Position;
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = 0.0f;
    float pitch = 0.0f;
    float distance = 5.0f;


    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    glm::vec3 defaultTarget;
    float defaultYaw = 0.0f;
    float defaultPitch = 0.0f;
    float defaultDistance = 5.0f;

	float zoomSpeed = 0.01f;
	float rotateSpeed = 0.1f;
};

#endif
