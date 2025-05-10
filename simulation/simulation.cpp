#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "FixedSizeSimulation.h"
#include "shader.h"
#include "mesh.h"
#include "scene.h"
#include "sceneObject.h"
#include "texture.h"
#include "primitives.h"
#include "shaderLoader.h"
#include "textureLoader.h"
#include "SimulationManager.h"

// Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// Global variable for controlling the simulation
extern std::atomic<bool> simulation_running;

// Global pointer to scene
Scene* scene = nullptr;

// Callback for resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Callback for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (Camera::getInstance().getMode() == CameraMode::Free) {
        Camera::getInstance().processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void run_opengl() {
    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Printer Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

	// Set the context, callbacks, and load GLAD
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gladLoadGL();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    simulation_running = true;


    // Load shaders
    ShaderLoader::load("default", "default.vert", "default.frag");
    ShaderLoader::load("filament", "filament.vert", "filament.frag");

    Shader* defaultShader = ShaderLoader::get("default");
    Shader* filamentShader = ShaderLoader::get("filament");

    // Load texture
    TextureLoader::load("brick", "br.png");
    TextureLoader::bindToShader("brick", *defaultShader, "tex0", 0);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Init camera
    Camera::init(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.4f, 0.0f), 0.0f, 0.0f, 5.0f, 1.0f, 5.0f);
    Camera& camera = Camera::getInstance();

    // Scene init
    Scene localScene;
    scene = &localScene;


    SimulationManager& sim = SimulationManager::get();

	// Fullprint simulation
    /*if (sim.isReady() && !sim.wasAlreadySimulated()) {
        sim.createSimulation(); 
        scene->addInstanced(sim.getContext().filamentObject); 
        sim.simulateFullPrint();
    }*/

	// Step by step simulation
    if (sim.isReady() && !sim.wasAlreadySimulated()) {
        sim.createSimulation();
        scene->addInstanced(sim.getContext().filamentObject);
        sim.startSimulation(); 
    }

    SceneObject* ground = Primitives::createTexturedPlane(10.0f);
	scene->add(ground);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        if (!simulation_running)
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.keyboardInputs(window);

        sim.tickSimulation();

        camera.applyToShader(*defaultShader, "camMatrix", 45.0f, 0.1f, 100.0f);
        camera.applyToShader(*filamentShader, "camMatrix", 45.0f, 0.1f, 100.0f);

        scene->Draw(*defaultShader, *filamentShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    TextureLoader::clear();
    ShaderLoader::clear();
    glfwDestroyWindow(window);
    glfwTerminate();
	simulation_running = false;
}
