#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "mesh.h"
#include "Scene.h"
#include "sceneObject.h"
#include "texture.h"

//Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


/////// Square
// Vertex data for the triangles
GLfloat vertices[] =
{
	// Positions			// Colors			// Texture coordinates
	-0.5f, 0.0f, 0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 
	-0.5f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
	 0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 0.0f,	5.0f, 0.0f,  
	 0.0f, 0.8f, 0.0f,		1.0f, 1.0f, 0.0f,	2.5f, 5.0f  
};

// Indices order for the triangles 
GLuint indices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};


// Global variable for controlling the simulation
extern std::atomic<bool> simulation_running;

// Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// OpenGL rendering function
void run_opengl() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    // Set OpenGL context
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize GLAD
    gladLoadGL();

	// Initialize viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Set the simulation_running flag to true
	simulation_running = true;

	Shader shaderProgram("default.vert", "default.frag");


		
	Mesh* sharedMesh = new Mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));

	// Root pyramid 
	SceneObject* root = new SceneObject(sharedMesh);

	// Child 1 
	SceneObject* child1 = new SceneObject(sharedMesh);
	root->addChild(child1);

	// Child 2 
	SceneObject* child2 = new SceneObject(sharedMesh);
	child1->addChild(child2);


	Scene scene;
	scene.add(root);

	// Texture
	Texture texture("br.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	texture.texUnit(shaderProgram, "tex0", 0);

	GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
	shaderProgram.Activate();
	glUniform1i(tex0Uni, 0);

	// Screen background color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);



	// Enables depth testing buffer
	glEnable(GL_DEPTH_TEST);

	Camera::init(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.4f, 0.0f), 0.0f, 0.0f, 5.0f, 1.0f, 5.0f);
	Camera& camera = Camera::getInstance();

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Check if the simulation should stop
        if (!simulation_running) {
            break;
        }


		// Scene rendering
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activating shader program in OpenGL
		shaderProgram.Activate();



		camera.keyboardInputs(window);
		camera.applyToShader(shaderProgram, "camMatrix", 45.0f, 0.1f, 100.0f);

		// Root 
		root->localTransform.top() = glm::mat4(1.0f);
		root->localTransform.translate(glm::vec3(sin(glfwGetTime()), 0.0f, 0.0f));

		// Child1 
		child1->localTransform.top() = glm::mat4(1.0f);
		child1->localTransform.translate(glm::vec3(0.0f, -1.5f, 0.0f)); 
		child1->localTransform.rotate(glfwGetTime(), glm::vec3(0, 1, 0));

		// Child2 
		child2->localTransform.top() = glm::mat4(1.0f);
		child2->localTransform.translate(glm::vec3(0.0f, -1.5f, 0.0f)); 
		child2->localTransform.rotate(glfwGetTime() * 2.0f, glm::vec3(0, 1, 0));

		scene.Draw(shaderProgram);


		// Swap back and front buffers
		//    back buffer - is where we draw things
		//    front buffer - is what we see
        glfwSwapBuffers(window);
		glfwPollEvents(); // Needed to update the window, without it the window will freeze as it will not respond to any events
    }


	/////// Clean up
	texture.Delete();
	//VAO1.Delete();
	//VBO1.Delete();
	//EBO1.Delete();
	shaderProgram.Delete();

    // Clean up and close the window
    glfwDestroyWindow(window);
    glfwTerminate();
}
