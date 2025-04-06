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

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // Vertex positions
	VAO1.LinkAttribute(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Vertex colors
	VAO1.LinkAttribute(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
    VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

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

		texture.Bind();

		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

		// Swap back and front buffers
		//    back buffer - is where we draw things
		//    front buffer - is what we see
        glfwSwapBuffers(window);
		glfwPollEvents(); // Needed to update the window, without it the window will freeze as it will not respond to any events
    }


	/////// Clean up
	texture.Delete();
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

    // Clean up and close the window
    glfwDestroyWindow(window);
    glfwTerminate();
}
