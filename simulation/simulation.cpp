#include <iostream>
#include <glad/glad.h>
#include <atomic>
#include <random>
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
#include "lightObject.h"
#include "materialManager.h"
#include "modelLoader.h"

// Window size
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// Global variable for controlling the simulation
extern std::atomic<bool> opengl_running;

// Global pointer to scene
Scene* scene = nullptr;

//Camera settings
float FOVdeg = 45.0f;
float nearPlane = 0.05f;
float farPlane = 20.0f;

void funny(bool yes);

void run_opengl() {

    // Set the window context, callbacks,
	WindowManager::init(SCR_WIDTH, SCR_HEIGHT, "3D Printer Simulator");
	WindowManager& windowManager = WindowManager::getInstance();

    opengl_running = true;

    // Load shaders
    ShaderLoader::load("default", "phong.vert", "phong.frag");
    ShaderLoader::load("filament", "filament.vert", "filament.frag");
	ShaderLoader::load("unlit", "unlit.vert", "unlit.frag");

    Shader* defaultShader = ShaderLoader::get("default");
    Shader* filamentShader = ShaderLoader::get("filament");
	Shader* unlitShader = ShaderLoader::get("unlit");

    // Load texture
    TextureLoader::load("brick", "br.png");
    TextureLoader::bindToShader("brick", *defaultShader, "tex0", 0);
	TextureLoader::load("e", "e.png");
	TextureLoader::bindToShader("e", *defaultShader, "tex1", 1);

    // Make materials
    MaterialManager::init();

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
    SceneObject* ground = Primitives::createPlane(100.0f);
	ground->setMaterial(MaterialManager::get("ground"));
    scene->add(ground);

    SceneObject* model = ModelLoader::loadModel("ReadyDrukara.gltf");
	model->localTransform.scale(glm::vec3(0.01f)); // Scale down the model
	model->localTransform.rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate to match the ground plane
    model->setMaterialApplyToChildren(MaterialManager::get("3DPrinter"));
    scene->add(model);


    // Light object setup
    auto* light = new LightObject(glm::vec3(1.0f, 1.0f, 01.0f)); 
	light->localTransform.translate(glm::vec3(4.5f, 5.0f, 4.5f));
	light->localTransform.scale(glm::vec3(0.1f)); 
    scene->addUnlit(light);


	/*auto testCylinder = Primitives::createDirectedCylinder(16, 0.5f, 1.0f);
    testCylinder->reserveInstances(1);
    testCylinder->appendDynamicInstance(glm::vec3(2.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    scene->addInstanced(testCylinder);

    auto testCube = Primitives::createDirectionalCube();
	testCube->reserveInstances(1);
	testCube->appendDynamicInstance(glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	scene->addInstanced(testCube);*/

	funny(false);
	

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

		// Upload light to shaders
        light->uploadToShader(*defaultShader);
        light->uploadToShader(*filamentShader);
		light->uploadToShader(*unlitShader);

		// Update camera
        auto [width, height] = windowManager.getWindowSize();
        camera.applyToShader(*defaultShader, FOVdeg, nearPlane, farPlane, width, height);
        camera.applyToShader(*filamentShader, FOVdeg, nearPlane, farPlane, width, height);
		camera.applyToShader(*unlitShader, FOVdeg, nearPlane, farPlane, width, height);

        // Render scene
		scene->Draw(*defaultShader, *filamentShader, *unlitShader);

        windowManager.processInput();
		windowManager.updateDisplay();
    }

    // Cleanup
	Camera::getInstance().reset();
    TextureLoader::clear();
    ShaderLoader::clear();
	MaterialManager::clear();
	scene->Delete();

	opengl_running = false;
}








void funny(bool yes)
{
	if (!yes) return;
    MaterialManager::create("brick", glm::vec3(0.8f, 0.2f, 0.2f), 32.0f);
    auto brickMaterial = MaterialManager::get("brick");
    brickMaterial->setTexture(TextureLoader::get("brick"));
    brickMaterial->textureMixRatio = 0.7f;

    MaterialManager::create("e", glm::vec3(0.2f, 0.8f, 0.2f), 100.0f);
    auto eMaterial = MaterialManager::get("e");
    eMaterial->ambientStrength = 0.3f;
    eMaterial->specularStrength = 0.8f;
    eMaterial->setTexture(TextureLoader::get("e"));
    eMaterial->textureMixRatio = 0.5f;

    std::vector<glm::vec3> cubePositions = {
        glm::vec3(-10.0f, 5.0f,  0.0f),
        glm::vec3(10.0f, 5.0f,  0.0f),
        glm::vec3(0.0f,  5.0f, -10.0f),
        glm::vec3(0.0f,  5.0f,  10.0f),
        glm::vec3(0.0f, 15.0f, 0.0f),
        glm::vec3(0.0f, -4.95f, 0.0f),
    };

    for (const auto& pos : cubePositions)
    {
        auto testCubeSceneObject = Primitives::createUnitCube();
        testCubeSceneObject->setMaterial(brickMaterial);
        testCubeSceneObject->localTransform.translate(pos);
        testCubeSceneObject->localTransform.scale(glm::vec3(10.0f));
        scene->add(testCubeSceneObject);
    }

    std::random_device rd;
    std::default_random_engine rng(rd());
    std::uniform_real_distribution<float> distPos(-4.0f, 4.0f);

    for (int i = 0; i < 10; ++i)
    {
        float randX = distPos(rng);
        float randZ = distPos(rng);

        glm::vec3 position(randX, 0.0f, randZ);

        glm::vec3 toCenter = glm::normalize(-position);

        float angleRad = atan2(toCenter.x, toCenter.z);

        auto pyramid = Primitives::createPyramid();
        pyramid->setMaterial(eMaterial);

        pyramid->localTransform.translate(position);
        pyramid->localTransform.rotate(angleRad, glm::vec3(0, 1, 0));
        pyramid->localTransform.scale(glm::vec3(2.0f));

        scene->add(pyramid);
    }

    Camera::getInstance().setAll(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
    Camera::getInstance().toggleMode();
}