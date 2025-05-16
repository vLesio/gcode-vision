#include <iostream>
#include <glad/glad.h>
#include <atomic>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "scene.h"
#include "sceneObject.h"
#include "texture.h"
#include "primitives.h"
#include "shaderLoader.h"
#include "textureLoader.h"
#include "SimulationManager.h"
#include "windowManager.h"

// Window size
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// Global variable for controlling the simulation
extern std::atomic<bool> opengl_running;

// Global pointer to scene
Scene* scene = nullptr;


void run_opengl() {

    // Set the window context, callbacks,
	WindowManager windowManager(SCR_WIDTH, SCR_HEIGHT, "3D Printer Simulator");

    opengl_running = true;

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
    Camera::init(glm::vec3(0.0f, 0.4f, 0.0f), 0.0f, 0.0f, 5.0f, 1.0f, 5.0f);
    Camera& camera = Camera::getInstance();

    // Scene init
    Scene localScene;
    scene = &localScene;

    // Register scene in simulation manager
    SimulationManager& sim = SimulationManager::get();
    sim.setScene(scene);

    // Add ground plane
    SceneObject* ground = Primitives::createTexturedPlane(10.0f);
    scene->add(ground);

    // Main render loop
    while (!windowManager.shouldClose()) {
        if (!opengl_running)
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.keyboardInputs(windowManager.getWindow());

        // Process REST API events
        sim.processEvents();

        // Update simulation
        sim.tickSimulation();

		// Update camera
        auto [width, height] = windowManager.getWindowSize();
        camera.applyToShader(*defaultShader, "camMatrix", 45.0f, 0.1f, 100.0f, width, height);
        camera.applyToShader(*filamentShader, "camMatrix", 45.0f, 0.1f, 100.0f, width, height);

        // Render scene
        scene->Draw(*defaultShader, *filamentShader);

        windowManager.processInput();
		windowManager.updateDisplay();
    }

    // Cleanup
	Camera::getInstance().reset();
    TextureLoader::clear();
    ShaderLoader::clear();
	scene->Delete();

	opengl_running = false;
}
