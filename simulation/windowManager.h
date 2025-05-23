#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class WindowManager {
public:
	//Singleton access
	static void init(int width, int height, const char* title);
    static WindowManager& getInstance();

    ~WindowManager();

    void toggleFullscreen();
    void processInput();
    GLFWwindow* getWindow() const;
    void updateDisplay();
    bool shouldClose() const;
    std::pair<int, int> getWindowSize() const;

private:
    WindowManager(int width, int height, const char* title);
    // Block copy and assignment
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    void createWindow(int width, int height, const char* title, bool fullscreen);
    void setCallbacks();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* window;
    GLFWmonitor* monitor;
    const GLFWvidmode* mode;

    int windowedWidth, windowedHeight;
    int windowedPosX, windowedPosY;
    bool isFullscreen;
    const char* title;
};

#endif
