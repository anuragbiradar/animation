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
	moveObject = 0.0f;
	sphereProgramId = 0;
	sphereVertexArrayObject = 0;
	sphereVertexBufferObject = 0;
}

render::~render() {
	glDeleteBuffers(1, &sphereVertexArrayObject);
	glDeleteBuffers(1, &sphereVertexBufferObject);
	glDeleteProgram(sphereProgramId);
	for (int i = 0; i < graphList.size(); i++)
		delete graphList[i];
}

void render::initRender(int width, int height, ply_parser *parser) {

	//Camera
	Camera *camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f), "Camera_1", height, width);
	//Create Parent node
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
	landNode->material.loadTexture("data/metal.png");
	sceneGraph *landGraph = new sceneGraph(landNode, "LandGraph");
	landGraph->init(camera);

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
	grassNode->material.loadTexture("data/grass.png");
	landGraph->addChild(grassNode);

	sphereProgramId = LoadShaders("data/3.1.blending.vs", "data/3.1.blending.fs");
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glEnable(GL_DEPTH_TEST);
	landGraph->setShaderProgramId(sphereProgramId);
	//graphList.push_back(landGraph);

	// Create Parash
	//sceneNode *sphereNode = new sceneNode("Sphere", glm::vec3(0.005f, 0.005f, 0.005f), glm::vec3(0.0f, 1.0f, 0.0f));
	sceneNode *sphereNode = new sceneNode("Sphere");
	sphereNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	//sphereNode->loadMeshObj("data/sphere.ply");
	sphereNode->material.loadTexture("data/earth.png");
	sphereNode->setScale(glm::vec3(1.005f, 1.002f, 1.005f));
	//sphereNode->setScale(glm::vec3(0.5f, 0.2f, 0.5f));
	sphereNode->setTransformation(glm::vec3(-0.6f, 1.0f, 0.0f));

	sceneGraph *parachuteGraph = new sceneGraph(sphereNode, "Parachute");
	parachuteGraph->init(camera);
	
	//sceneNode *basketNode = new sceneNode("Basket", glm::vec3(0.2f, 0.2f, 2.2f), glm::vec3(0.0f, -1.5f, 3.0f));
	sceneNode *basketNode = new sceneNode("Basket");
	parachuteGraph->addChild(basketNode);
	basketNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	//basketNode->loadMeshObj("data/sphere.ply");
	basketNode->material.loadTexture("data/container2.png");
	basketNode->setScale(glm::vec3(1.02f, 1.02f, 1.02f));
	basketNode->setTransformation(glm::vec3(-0.3f, -0.7f, 0.0f));

	parachuteGraph->setShaderProgramId(sphereProgramId);
	
	graphList.push_back(parachuteGraph);

	// setup
	for (int i = 0; i < graphList.size(); i++) {
		graphList[i]->setup();
	}

	return;
}

void render::drawSpheres(rotationAxis axis, objectDirection translate) {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (int i = 0; i < graphList.size(); i++)
	{
		if (graphList[i]->getName().compare("Parachute") == 0) {
			//std::cout << "NAME " << graphList[i]->getName() << "\n";
			if (translate == MOVE_RIGHT)
				graphList[i]->update(glm::vec3(0.2f, 0.0f, 0.0f));
			if (translate == MOVE_LEFT)
				graphList[i]->update(glm::vec3(-0.2f, 0.0f, 0.0f));
		}
		graphList[i]->displayScene();
	}
}

GLuint render::LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
