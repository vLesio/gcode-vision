#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <thread>

#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"



/////// Triangle rendering - 3 triangles
// Vertex data for the triangles
GLfloat vertices[] =
{
	// Positions														// Colors
	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,						1.0f, 0.0f, 0.0f, // Lower left corner
	0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,							1.0f, 0.0f, 0.0f, // Lower right corner
	0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,						0.0f, 1.0f, 0.0f, // Upper corner	
	-0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,						0.0f, 0.0f, 1.0f, // Inner left	
	0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,						0.0f, 0.0f, 1.0f, // Inner right	
	0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f,							1.0f, 0.0f, 1.0f  // Inner down
};

// Indices order for the triangles
GLuint indices[] = {
	0, 3, 5, // Triangle 1
	3, 2 ,4, // Triangle 2
	5, 4, 1  // Triangle 3
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
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
	glViewport(0, 0, 800, 600);

	// Set the simulation_running flag to true
	simulation_running = true;

	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0); // Vertex positions
	VAO1.LinkAttribute(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Vertex colors
    VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Screen background color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    
    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Check if the simulation should stop
        if (!simulation_running) {
            break;
        }


		// Scene rendering
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		// Activating shader program in OpenGL
		shaderProgram.Activate();
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		// Swap back and front buffers
		//    back buffer - is where we draw things
		//    front buffer - is what we see
        glfwSwapBuffers(window);
		glfwPollEvents(); // Needed to update the window
    }


	/////// Clean up
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

    // Clean up and close the window
    glfwDestroyWindow(window);
    glfwTerminate();
}
