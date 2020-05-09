#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <math.h>

#include "render.h"
using namespace std;


glm::vec3 cameraPosition1(1.0f, 1.0f, 3.0f); 

glm::vec3 positions[] = {
	glm::vec3(10.0f, 10.0f, -40.3f),
	glm::vec3(10.0f, 10.0f, -40.3f),
};

glm::vec3 spherePosition[] = {
	// Middle Column Bottom First
	glm::vec3(0.0f, -2.0f, 0.0f),
	glm::vec3(0.0f,  0.0f, 0.0f),
	glm::vec3(0.0f,  2.0f, 0.0f),
	// Last Column Bottom First
	glm::vec3(-2.0f, -2.0f, 0.0f),
	glm::vec3(-2.0f,  0.0f, 0.0f),
	glm::vec3(-2.0f,  2.0f, 0.0f),
	// First Column Bottom First
	glm::vec3(2.0f, -2.0f, 0.0f),
	glm::vec3(2.0f,  0.0f, 0.0f),
	glm::vec3(2.0f,  2.0f, 0.0f)
};

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

render::render() {
}

render::~render() {
	for (int i = 0; i < graphList.size(); i++)
		delete graphList[i];
	delete camera;
}

void render::initRender(int width, int height, ply_parser *parser) {

	//Camera
	camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f), "Camera_1", height, width);
	//Create Parent node
	Material *grassMaterial = new Material();
	grassMaterial->loadTexture("data/grass.png");
	unsigned int shaderId = grassMaterial->loadShaderFile("data/3.1.blending.vs", "data/3.1.blending.fs");

	Material *metalMaterial = new Material(shaderId);
	metalMaterial->loadTexture("data/metal.png");

	sceneNode *landNode = new sceneNode("Land");
	float planeVertices[] = {
		// positions          // texture Coords 
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	landNode->loadMeshObj(planeVertices, sizeof(planeVertices)/sizeof(float));
	landNode->setMaterial(metalMaterial);


	sceneNode *grassNode = new sceneNode("Grass");
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	grassNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	grassNode->setMaterial(grassMaterial);

	landNode->addChild(grassNode);

	// Parent node list
	graphList.push_back(landNode);

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glEnable(GL_DEPTH_TEST);

	// Create Parash
	//sceneNode *sphereNode = new sceneNode("Sphere", glm::vec3(0.005f, 0.005f, 0.005f), glm::vec3(0.0f, 1.0f, 0.0f));
	Material *sphereMaterial = new Material(shaderId);
	sphereMaterial->loadTexture("data/earth.png");

	Material *basketMaterial = new Material(shaderId);
	basketMaterial->loadTexture("data/container2.png");

	sceneNode *sphereNode = new sceneNode("Sphere");
	sphereNode->setMaterial(sphereMaterial);
	//sphereNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	//sphereNode->loadMeshObj("data/sphere.ply");
	sphereNode->loadMeshObjVert("data/sphere.vert");
	//sphereNode->setScale(glm::vec3(0.005f, 0.002f, 0.005f));
	sphereNode->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
	//sphereNode->setPosition(glm::vec3(-50.10f, 80.0f, 0.0f));
	sphereNode->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	//sceneNode *basketNode = new sceneNode("Basket", glm::vec3(0.2f, 0.2f, 2.2f), glm::vec3(0.0f, -1.5f, 3.0f));
	sceneNode *basketNode = new sceneNode("Basket");
	basketNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	//basketNode->loadMeshObj("data/sphere.ply");
	basketNode->setMaterial(basketMaterial);
	//basketNode->setScale(glm::vec3(500.02f, 500.02f, 500.02f));
	basketNode->setScale(glm::vec3(0.5f));
	basketNode->setPosition(glm::vec3(0.00f, -2.20f, 0.0f));

	sphereNode->addChild(basketNode);
	//basketNode->addChild(sphereNode);

	graphList.push_back(sphereNode);
	//graphList.push_back(basketNode);

#if 0
	// setup
	for (int i = 0; i < graphList.size(); i++) {
		graphList[i]->setup();
	}
#endif
	camera->setup(shaderId);
	return;
}

void render::drawSpheres(rotationAxis axis, objectDirection translate) {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (int i = 0; i < graphList.size(); i++)
	{
		if (graphList[i]->getName().compare("Sphere") == 0 || graphList[i]->getName().compare("Basket") == 0) {
			//std::cout << "NAME " << graphList[i]->getName() << "\n";
			if (translate == MOVE_RIGHT)
				graphList[i]->setPosition(glm::vec3(0.2f, 0.0f, 0.0f));
			if (translate == MOVE_LEFT)
				graphList[i]->setPosition(glm::vec3(-0.2f, 0.0f, 0.0f));
			if (translate == MOVE_UP)
				graphList[i]->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
			if (translate == MOVE_DOWN)
				graphList[i]->setPosition(glm::vec3(0.0f, -0.5f, 0.0f));
		}
		graphList[i]->displayScene(camera->getProjectionMatrix(), camera->getViewMatrix());
	}
}
