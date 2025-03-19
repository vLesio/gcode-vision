#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <thread>

#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"



/////// Square
// Vertex data for the triangles
GLfloat vertices[] =
{
	// Positions			// Colors
	-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f, // Bottom left corner		index: 0
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f, // Top left corner		index: 1
	 0.5f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f, // Top right corner		index: 2
	 0.5f, -0.5f, 0.0f,		1.0f, 1.0f, 0.0f // Bottom right corner		index: 3
};

// Indices order for the triangles (we make a square out of two triangles)
GLuint indices[] = {
	0, 1, 3, // First triangle
	1, 2, 3  // Second triangle
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Window", nullptr, nullptr);
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
	glViewport(0, 0, 800, 800);

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

	// Get the uniform ID for the scale variable located in the vertex shader file (default.vert)
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

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

		// Update the uniform variable in the vertex shader, which is the scale variable.
		// Must be done after activating the shader program but before rendering the object
		glUniform1f(uniID, 0.5f);

		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap back and front buffers
		//    back buffer - is where we draw things
		//    front buffer - is what we see
        glfwSwapBuffers(window);
		glfwPollEvents(); // Needed to update the window, without it the window will freeze as it will not respond to any events
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
