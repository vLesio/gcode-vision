#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shaderClass.h"
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

void run_opengl() {
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

    simulation_running = true;

    Shader shaderProgram("default.vert", "default.frag");
    Texture texture("br.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    texture.texUnit(shaderProgram, "tex0", 0);
    GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
    shaderProgram.Activate();
    glUniform1i(tex0Uni, 0);

   Mesh* sharedMesh = new Mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));

	// Root 
	SceneObject* root = new SceneObject(sharedMesh);
	root->setTexture(&texture);
	root->localTransform.translate(glm::vec3(0.0f, 1.5f, 0.0f));

	//// Child1
	//SceneObject* child1 = new SceneObject(sharedMesh);
	//root->addChild(child1);

	//// Child2 
	//SceneObject* child2 = new SceneObject(sharedMesh);
	//child1->addChild(child2);

	// Add only root to the scene
	Scene scene;
	scene.add(root);


    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    Camera::init(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.4f, 0.0f), 0.0f, 0.0f, 5.0f, 1.0f, 5.0f);
    Camera& camera = Camera::getInstance();

    bool pushed = false;
    bool popped = false;

    while (!glfwWindowShouldClose(window)) {
        if (!simulation_running)
            break;

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        camera.keyboardInputs(window);
        camera.applyToShader(shaderProgram, "camMatrix", 45.0f, 0.1f, 100.0f);

        float time = glfwGetTime();

        float t = glfwGetTime();

        if (t > 2.0f && !pushed) {
            root->localTransform.push();
            root->localTransform.translate(glm::vec3(2.0f, 0.0f, 0.0f));
            pushed = true;
        }

        if (t > 7.0f && !popped) {
            root->localTransform.pop();
            popped = true;
        }

        //// Root
        //root->localTransform.resetTop();
        //root->localTransform.translate(glm::vec3(sin(time), 0.0f, 0.0f));

        //// Child1
        //child1->localTransform.resetTop();
        //child1->localTransform.translate(glm::vec3(0.0f, -1.5f, 0.0f));
        //child1->localTransform.rotate(time, glm::vec3(0, 1, 0));

        //// Child2
        //child2->localTransform.resetTop();
        //child2->localTransform.translate(glm::vec3(0.0f, -1.5f, 0.0f));
        //child2->localTransform.rotate(time * 2.0f, glm::vec3(0, 1, 0));

        scene.Draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    texture.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
}

