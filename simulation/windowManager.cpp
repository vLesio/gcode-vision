#include "WindowManager.h"
#include <iostream>
#include <unordered_map>
#include "camera.h"

// Singleton instance
static std::unique_ptr<WindowManager> managerInstance = nullptr;

WindowManager::WindowManager(int width, int height, const char* title)
    : window(nullptr), isFullscreen(false), title(title),
    windowedWidth(width), windowedHeight(height) {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    windowedPosX = 100;
    windowedPosY = 100;

    createWindow(width, height, title, false);
}

void WindowManager::init(int width, int height, const char* title)
{
	if (!managerInstance) {
		managerInstance.reset(new WindowManager(width, height, title));
	}
    else
    {
		std::cerr << "[WindowManager] Warning: Already initialized!" << std::endl;
    }
}

WindowManager& WindowManager::getInstance()
{
	if (!managerInstance)
	{
		throw std::runtime_error("[WindowManager] Tried to use before initialization!");
	}
	return *managerInstance;
}

WindowManager::~WindowManager() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void WindowManager::createWindow(int width, int height, const char* title, bool fullscreen) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, fullscreen ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, fullscreen ? GLFW_FALSE : GLFW_TRUE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    glfwSwapInterval(1); // VSync
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (fullscreen) {
        glfwSetWindowPos(window, 0, 0);
        glfwSetWindowSize(window, mode->width, mode->height);
    }
    else {
        glfwSetWindowPos(window, windowedPosX, windowedPosY);
    }

    setCallbacks();
}
///////// Callbacks

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (Camera::getInstance().getMode() == CameraMode::Free) {
        Camera::getInstance().processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void WindowManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
        getInstance().toggleFullscreen();
    }
}

void WindowManager::setCallbacks() {
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
        });

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        });

    glfwSetKeyCallback(window, key_callback);
}

void WindowManager::toggleFullscreen() {
    isFullscreen = !isFullscreen;

    if (isFullscreen) {
        glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowPos(window, 0, 0);
        glfwSetWindowSize(window, mode->width, mode->height);
    }
    else {
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
        glfwSetWindowPos(window, windowedPosX, windowedPosY);
        glfwSetWindowSize(window, windowedWidth, windowedHeight);
    }
}


void WindowManager::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

GLFWwindow* WindowManager::getWindow() const {
    return window;
}

void WindowManager::updateDisplay() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool WindowManager::shouldClose() const {
    return glfwWindowShouldClose(window);
}

std::pair<int, int> WindowManager::getWindowSize() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return { width, height };
}
