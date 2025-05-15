#include "WindowManager.h"
#include <iostream>
#include <unordered_map>
#include "camera.h"

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (Camera::getInstance().getMode() == CameraMode::Free) {
        Camera::getInstance().processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
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

bool WindowManager::isKeyPressedOnce(int key) {
    static std::unordered_map<int, bool> keyStates;
    bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool wasPressed = keyStates[key];
    keyStates[key] = isPressed;
    return isPressed && !wasPressed;
}

void WindowManager::processInput() {
    if (isKeyPressedOnce(GLFW_KEY_F11)) {
        toggleFullscreen();
    }

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
