#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

//Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


// Global variable for controlling the simulation
extern std::atomic<bool> simulation_running;

// Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void run_opengl() {
    /// Init , viewport and libraries ///
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGL();
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    /// /// /// ///

    simulation_running = true;

    /// Load shaders and textures ///
    ShaderLoader::load("default", "default.vert", "default.frag");
    ShaderLoader::load("filament", "filament.vert", "filament.frag");

	Shader* filamentShader = ShaderLoader::get("filament");
	Shader* defaultShader = ShaderLoader::get("default");

    TextureLoader::load("brick", "br.png");
    TextureLoader::bindToShader("brick", *defaultShader, "tex0", 0);

	Texture* brickTexture = TextureLoader::get("brick");

    /// /// /// ///

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    Camera::init(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.4f, 0.0f), 0.0f, 0.0f, 5.0f, 1.0f, 5.0f);
    Camera& camera = Camera::getInstance();

    /// Create a scene and add objects
    Scene scene;
    SceneObject* cube = Primitives::createUnitCube();
    scene.add(cube);
	cube->setTexture(brickTexture);

	SceneObject* cube2 = Primitives::createUnitCube();
	cube2->localTransform.translate(glm::vec3(1.0f, 0.0f, 0.0f));

	scene.add(cube2);
    ///

    while (!glfwWindowShouldClose(window)) {
        if (!simulation_running)
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
        camera.keyboardInputs(window);
        camera.applyToShader(*defaultShader, "camMatrix", 45.0f, 0.1f, 100.0f);
        camera.applyToShader(*filamentShader, "camMatrix", 45.0f, 0.1f, 100.0f);


		scene.Draw(*defaultShader, *filamentShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TextureLoader::clear();
    ShaderLoader::clear();
    glfwDestroyWindow(window);
    glfwTerminate();
}

