#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <fstream>
#include <sstream>
#include <math.h>
#include <cstdlib>

#include "render.h"
using namespace std;

static Camera *cameraObj;
static bool isCrash = false;

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
	for (int i = 0; i < materialList.size(); i++)
		delete materialList[i];
	delete camera;
	delete camera1;
	delete env;
}

void render::initRender(int width, int height) {

	//Camera
	camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f), "Camera_1", height, width);
	camera1 = new Camera(glm::vec3(1.0f, 1.0f, -5.0f), "Camera_2", height, width);
	env = new renderEnv(glm::vec3(-20.0f, 10.25f, 0.0f), glm::vec3(-20.0f, 10.25f, 0.0f));
	//Create Parent node
	Material *grassMaterial = new Material();
	grassMaterial->loadTexture("data/grass.png");
	unsigned int shaderId = grassMaterial->loadShaderFile("data/animation.vs", "data/animation.fs");
	materialList.push_back(grassMaterial);

	Material *metalMaterial = new Material(shaderId);
	metalMaterial->loadTexture("data/metal.png");
	materialList.push_back(metalMaterial);

	sceneNode *landNode = new sceneNode("Land");
	float planeVertices[] = {
		// positions            // normals         // texcoords
		5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f,  0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
	};

	landNode->loadMeshObj(planeVertices, sizeof(planeVertices)/sizeof(float));
	landNode->setMaterial(metalMaterial);


	sceneNode *grassNode = new sceneNode("Grass");
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f
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
	Material *sphereMaterial = new Material(shaderId);
	sphereMaterial->loadTexture("data/air.jpg");
	materialList.push_back(sphereMaterial);

	Material *basketMaterial = new Material(shaderId);
	basketMaterial->loadTexture("data/basket.jpg");
	materialList.push_back(basketMaterial);

	sceneNode *sphereNode = new sceneNode("Sphere");
	sphereNode->setMaterial(sphereMaterial);
	sphereNode->loadMeshObj("data/sphere.ply");
	sphereNode->setScale(glm::vec3(0.5f));
	sphereNode->setPosition(glm::vec3(-1.0f, 2.0f, 0.0f));

	sceneNode *basketNode = new sceneNode("Basket");
	basketNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	basketNode->setMaterial(basketMaterial);
	basketNode->setScale(glm::vec3(0.5f));
	basketNode->setPosition(glm::vec3(-0.50f, -2.50f, 0.0f));

	sphereNode->addChild(basketNode);

	graphList.push_back(sphereNode);

	Material *antMaterial = new Material(shaderId);
	antMaterial->loadTexture("data/ant.jpg");
	materialList.push_back(antMaterial);

	// Ant Group
	sceneNode *antGroupNode1 = new sceneNode("AntG1");
	antGroupNode1->loadMeshObj("data/ant.ply");
	antGroupNode1->setMaterial(antMaterial);
	antGroupNode1->setScale(glm::vec3(0.005f));
	antGroupNode1->setPosition(glm::vec3(3.0f, 2.25f, 0.0f));


	sceneNode *antNode1 = new sceneNode("Ant1");
	antNode1->loadMeshObj("data/ant.ply");
	antNode1->setMaterial(antMaterial);
	antNode1->setPosition(glm::vec3(15.90f, 2.25f, -3.0f));

	sceneNode *antNode2 = new sceneNode("Ant2");
	antNode2->loadMeshObj("data/ant.ply");
	antNode2->setMaterial(antMaterial);
	antNode2->setPosition(glm::vec3(25.50f, 2.25f, 0.0f));

	antGroupNode1->addChild(antNode1);
	antGroupNode1->addChild(antNode2);

	sceneNode *antGroupNode2 = new sceneNode("AntG2");
	antGroupNode2->loadMeshObj("data/ant.ply");
	antGroupNode2->setMaterial(antMaterial);
	antGroupNode2->setScale(glm::vec3(0.005f));
	antGroupNode2->setPosition(glm::vec3(-300.0f, 2.25f, 500.0f));

	sceneNode *antNodeC1 = new sceneNode("Ant_1");
	antNodeC1->loadMeshObj("data/ant.ply");
	antNodeC1->setMaterial(antMaterial);
	antNodeC1->setPosition(glm::vec3(15.90f, 2.25f, -3.0f));


	sceneNode *antNodeC2 = new sceneNode("Ant_2");
	antNodeC2->loadMeshObj("data/ant.ply");
	antNodeC2->setMaterial(antMaterial);
	antNodeC2->setPosition(glm::vec3(25.90f, 2.25f, -3.0f));

	antGroupNode2->addChild(antNodeC1);
	antGroupNode2->addChild(antNodeC2);

	graphList.push_back(antGroupNode1);
	graphList.push_back(antGroupNode2);

	//Material *moonMaterial = new Material(shaderId);
	Material *moonMaterial = new Material();
	moonMaterial->loadShaderFile("data/moon.vs", "data/moon.fs");
	moonMaterial->loadTexture("data/moon.jpg");
	materialList.push_back(moonMaterial);

	sceneNode *moonNode = new sceneNode("Moon");
	moonNode->loadMeshObj("data/sphere.ply");
	moonNode->setScale(glm::vec3(0.1f));
	moonNode->setPosition(glm::vec3(-25.0f, 18.25f, 0.0f));
	//moonNode->setPosition(glm::vec3(-10.0f, 10.25f, 0.0f));
	moonNode->setMaterial(moonMaterial);

	sceneNode *planeNode = new sceneNode("Airplane");
	planeNode->loadMeshObj("data/airplane.ply");
	planeNode->setPosition(glm::vec3(100.0f, 40.25f, 0.0f));
	planeNode->setMaterial(metalMaterial);
	planeNode->setScale(glm::vec3(0.009f));
	planeNode->setRotation(-90.0f, glm::vec3(1.0f, 1.0f , 1.0f));

	graphList.push_back(moonNode);
	graphList.push_back(planeNode);
	camera->setup(shaderId);
	camera->setLightPos0(glm::vec3(-25.0f, 10.25f, -5.0f));
	//camera->setLightPos0(glm::vec3(-10.0f, 10.25f, 0.0f));
	camera1->setup(shaderId);
	camera1->setLightPos0(glm::vec3(-25.0f, 10.25f, -5.0f));
	cameraObj = camera;
	const char *envValue = getenv("CRASH");
	if (envValue != NULL && strcmp(envValue, "1") == 0) {
		isCrash = true;
	}
	return;
}

void render::renderScene(const char *cameraName) {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (strcmp(cameraName, "Camera_1") == 0)
		cameraObj = camera;
	else
		cameraObj = camera1;
	static int count = 0;
	for (int i = 0; i < graphList.size(); i++)
	{
		if (graphList[i]->getName().compare("Moon") == 0) {
			glm::vec3 scale = graphList[i]->getScale() + glm::vec3(0.00005);
			graphList[i]->setScale(scale);
			graphList[i]->setPosition(glm::vec3(0.09f, -0.01f, 0.0f));
			cameraObj->setLightPos0(glm::vec3(0.09f, 0.00f, 0.0f));
		} else if (graphList[i]->getName().compare("Sphere") == 0) {
			glm::vec3 scale = graphList[i]->getScale() + glm::vec3(0.0005);
			if (isCrash) {
				if (count > 200) {
					//std::cout << "Will make it disappear\n";
					glm::vec3 po = graphList[i]->getPosition();
					std::cout << "Sphere x " << po.x << " y " << po.y << " z " << po.z << endl;
					graphList[i]->setVisible(false);
					// free fall
					graphList[i]->setPosition(glm::vec3(0.0f, -0.05f, 0.0f));
				} else {
					graphList[i]->setScale(scale);
					graphList[i]->setPosition(glm::vec3(-0.005f, -0.0005f, 0.0f));
					graphList[i]->setPosition(glm::vec3(0.000f, 0.0f, -0.0005f));
				}
			} else {
				graphList[i]->setScale(scale);
				graphList[i]->setPosition(glm::vec3(-0.005f, -0.0005f, 0.0f));
				graphList[i]->setPosition(glm::vec3(0.000f, 0.0f, -0.0005f));
			}
		} else if (graphList[i]->getName().compare("AntG1") == 0) {
			graphList[i]->setPosition(glm::vec3(0.0f, 0.0f, 0.5f));
		} else if (graphList[i]->getName().compare("AntG2") == 0) {
			graphList[i]->setPosition(glm::vec3(0.0f, 0.0f, -0.5f));
		} else if (graphList[i]->getName().compare("Airplane") == 0) {
			if (isCrash) {
				if (count < 70) {
					graphList[i]->setPosition(glm::vec3(0.0f, 0.0f, 1.0f));
					graphList[i]->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
				} else {
					graphList[i]->setPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
					glm::vec3 po = graphList[i]->getPosition();
					std::cout << "Air x " << po.x << " y " << po.y << " z " << po.z << endl;
				}
			} else {
				graphList[i]->setPosition(glm::vec3(0.0f, 0.0f, 1.0f));
				graphList[i]->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
			}
			//graphList[i]->setRotation(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			//std::cout << "Count " << count << "\n";
		}
		graphList[i]->displayScene(cameraObj->getProjectionMatrix(), cameraObj->getViewMatrix(), cameraObj->getLightPos0());
	}
	count++;
}
