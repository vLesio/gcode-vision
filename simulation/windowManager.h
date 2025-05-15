#ifndef  WINDOW_MANAGER_H
#define  WINDOW_MANAGER_H

#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class WindowManager {
public:
    WindowManager(int width, int height, const char* title);
    ~WindowManager();

    void toggleFullscreen();
    bool isKeyPressedOnce(int key);
    void processInput();
    GLFWwindow* getWindow() const;
    void updateDisplay();
    bool shouldClose() const;
    std::pair<int, int> getWindowSize() const;

private:
    void createWindow(int width, int height, const char* title, bool fullscreen);
    void setCallbacks();

    GLFWwindow* window;
    GLFWmonitor* monitor;
    const GLFWvidmode* mode;

    int windowedWidth, windowedHeight;
    int windowedPosX, windowedPosY;
    bool isFullscreen;
    const char* title;
};

#endif

