#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ply_parser.h"
#include "render.h"

#include <iostream>
using namespace std;

typedef enum _mobject_ {
	MESH_OBJECT_1,
	MESH_OBJECT_2,
	MESH_OBJECT_3,
} mesh_object_type;


void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);

class window_mgmt {

	private:
		GLFWwindow *mainWindow;
		uint8_t parse_top;
		render render_obj;
		float deltaTime, lastFrame;
		char *camera;
	public:
		window_mgmt();
		int window_mainloop_init();
		void window_handle_event(int key, int code, int action, int mode);
		void window_mainloop_run();
};

window_mgmt window_mgmr;

window_mgmt::window_mgmt() {
	camera = "Camera_1";
}

int window_mgmt::window_mainloop_init() {
	const GLint WIDTH = 800, HEIGHT = 600;

	// Initializing GLFW
	if(!glfwInit()) {
		cout <<"GLFW initialization failed.";
		glfwTerminate();
		return 0;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "AssignMent", NULL, NULL);
	if(!mainWindow) {
		cout << "GLFW window creation failed.";
		glfwTerminate();
		return 0;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK) {
		cout << "GLEW initialization failed.";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 0;
	}
	render_obj.initRender(bufferWidth, bufferHeight);
}

void window_mgmt::window_mainloop_run() {
	while(!glfwWindowShouldClose(mainWindow))
  	{
		float currentFrame = glfwGetTime();
        	deltaTime = currentFrame - lastFrame;
        	lastFrame = currentFrame;
		//std::cout << "Delta " << deltaTime << "\n";
		if (deltaTime < 0.03f)
			continue;

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_obj.renderScene(camera);
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		glfwSetKeyCallback(mainWindow, handleKeys);
	}
}

void window_mgmt::window_handle_event(int key, int code, int action, int mode) {
	if (key == 'C' && action == GLFW_PRESS) {
		camera = "Camera_1";
	} else if (key == 'V' && action == GLFW_PRESS) {
		camera = "Camera_2";
	}
}

void handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	   glfwSetWindowShouldClose(window, GL_TRUE);
  	else
		window_mgmr.window_handle_event(key, code, action, mode);
}

int main(int argc, char *argv[])
{
	window_mgmr.window_mainloop_init();
	window_mgmr.window_mainloop_run();
	return 0;
}
