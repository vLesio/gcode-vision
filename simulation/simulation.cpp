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
#include "Printer.h"

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
    Camera::init(glm::vec3(1.095f, 0.4f, 0.95f), 0.0f, 0.0f, 5.0f, 1.0f, 5.0f);
    Camera& camera = Camera::getInstance();

    // Scene init
    Scene localScene;
    scene = &localScene;

    // Register scene in simulation manager
    SimulationManager& sim = SimulationManager::get();
    sim.setScene(scene);

    // Add ground plane
    SceneObject* ground = Primitives::createPlane(100.0f);
	ground->localTransform.translate(glm::vec3(0.0f, -2.0f, 0.0f));
	ground->setMaterial(MaterialManager::get("ground"));
    //scene->add(ground);

	// Add 3D printer model
    SceneObject* model = ModelLoader::loadModel("printer.gltf");
	std::shared_ptr<Printer> printer = std::make_unique<Printer>(model);
    printer->setPrinterPosition(glm::vec3(-0.55f, -0.97f, 2.59f))
           ->setPrinterScale(glm::vec3(sim.getContext().simulationScale))
           ->setPrinterRotation(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
           ->setPrinterMaterial(MaterialManager::get("3DPrinter"))
		   ->addToScene(scene);
    sim.setPrinter(printer);

    // Light object setup
    auto* light = new LightObject(glm::vec3(1.0f, 1.0f, 01.0f)); 
	light->localTransform.translate(glm::vec3(14.5f, 50.0f, 14.5f));
	light->localTransform.scale(glm::vec3(0.1f)); 
    scene->addUnlit(light);


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



//  for (float i = 0; i < 3; i += 0.05)
 //  {
       //for (float j = 0; j < 3; j += 0.05)
       //{
       //	SceneObject* cube = Primitives::createUnitCube();
       //	cube->localTransform.translate(glm::vec3(i, 0.0f, j));
 //          int i_int = i;
 //          int j_int = j;
 //          if (i - i_int > 0 || j - j_int > 0) {
 //              cube->localTransform.scale(glm::vec3(0.005f));
 //          }
 //          else {
 //              cube->localTransform.scale(glm::vec3(0.05f));
 //          }
 //          if (i == 0 && j == 0)
 //          {
       //		MaterialManager::create("zerozero", glm::vec3(0.1f, 0.0f, 0.0f), 100.0f); // red
       //		auto zerozeroMaterial = MaterialManager::get("zerozero");
       //		cube->setMaterial(zerozeroMaterial);
 //          }
       //	else if (i == 0 || j == 0)
       //	{
       //		MaterialManager::create("edge", glm::vec3(0.8f, 0.2f, 0.2f), 32.0f); // edge color - red
       //		auto edgeMaterial = MaterialManager::get("edge");
       //		cube->setMaterial(edgeMaterial);
       //	}
       //	else if (i < 0 && j < 0)
       //	{
       //		MaterialManager::create("negative", glm::vec3(0.2f, 0.2f, 0.8f), 16.0f); // blue
       //		auto negativeMaterial = MaterialManager::get("negative");
       //		cube->setMaterial(negativeMaterial);
       //	}
       //	else if (i > 0 && j > 0)
       //	{
       //		MaterialManager::create("positive", glm::vec3(0.8f, 0.8f, 0.2f), 64.0f); // yellow
       //		auto positiveMaterial = MaterialManager::get("positive");
       //		cube->setMaterial(positiveMaterial);
       //	}
       //	else if (i < 0 && j > 0)
       //	{
       //		MaterialManager::create("negpos", glm::vec3(0.8f, 0.2f, 0.8f), 48.0f); // magenta
       //		auto negposMaterial = MaterialManager::get("negpos");
       //		cube->setMaterial(negposMaterial);
       //	}
       //	else if (i > 0 && j < 0)
       //	{
       //		MaterialManager::create("posneg", glm::vec3(0.2f, 0.8f, 0.8f), 24.0f); // cyan
       //		auto posnegMaterial = MaterialManager::get("posneg");
       //		cube->setMaterial(posnegMaterial);
       //	}
 //          else
 //          {
       //		cube->setMaterial(MaterialManager::get("default")); // Default material
 //          }

       //	scene->add(cube);
       //}
 //  }
