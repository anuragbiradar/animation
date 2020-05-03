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
	graphList.push_back(landGraph);

	// Create Parash
	sceneNode *sphereNode = new sceneNode("Sphere");
	sphereNode->loadMeshObj("data/sphere.ply");
	sphereNode->material.loadTexture("data/earth.png");
	sphereNode->setTransformation(glm::vec3(2.0f, 2.0f, 0.0f));
	sphereNode->setScale(glm::vec3(0.005f, 0.005f, 0.005f));

	sceneGraph *parachuteGraph = new sceneGraph(sphereNode, "Parachute");
	parachuteGraph->init(camera);
	
	sceneNode *basketNode = new sceneNode("Basket");
	basketNode->loadMeshObj(transparentVertices, sizeof(transparentVertices)/sizeof(float));
	basketNode->material.loadTexture("data/container2.png");
	basketNode->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
	basketNode->setTransformation(glm::vec3(4.6f, 3.3f, 0.0f));

	parachuteGraph->addChild(basketNode);
	parachuteGraph->setShaderProgramId(sphereProgramId);
	
	graphList.push_back(parachuteGraph);


	// setup
	for (int i = 0; i < graphList.size(); i++) {
		graphList[i]->setup();
	}

	return;
}

void render::setView1() {
	projectionMatrix1 = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH/ (float) SCR_HEIGHT, 0.1f, 100.0f);

	//projectionMatrix = glm::ortho(-100.0f,100.0f,-100.0f,100.0f,0.0f,50.0f); // In world coordinates
	glCheckError();

	viewMatrix1 = glm::lookAt(
			cameraPosition1, // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
	GLuint viewID = glGetUniformLocation(sphereProgramId, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix1[0][0]);

	GLuint projectionID = glGetUniformLocation(sphereProgramId, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix1[0][0]);

	glm::vec3 ObjectColor(1.0f, 0.0f, 0.0f);
	GLuint ObjectColorID = glGetUniformLocation(sphereProgramId, "objectColor");
	glUniform3fv(ObjectColorID, 1, &ObjectColor[0]);

	glm::vec3 LightColor(1.0f, 1.0f, 1.0f);
	GLuint lightColorID = glGetUniformLocation(sphereProgramId, "lightColor");
	glUniform3fv(lightColorID, 1, glm::value_ptr(LightColor));

}

#if 0
unsigned int render::loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int render::loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif

void render::drawFloor() {

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	GLuint modelID = glGetUniformLocation(sphereProgramId, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelMatrix[0][0]);
	glm::mat4 mvp = projectionMatrix1 * viewMatrix1 * modelMatrix;
	GLuint mvpID = glGetUniformLocation(sphereProgramId, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(planeVertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render::drawGrass() {

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	GLuint modelID = glGetUniformLocation(sphereProgramId, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelMatrix[0][0]);
	glm::mat4 mvp = projectionMatrix1 * viewMatrix1 * modelMatrix;
	GLuint mvpID = glGetUniformLocation(sphereProgramId, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(grassVertexArrayObject);
	//glBindTexture(GL_TEXTURE_2D, floorTexture);
	glBindTexture(GL_TEXTURE_2D, transparentTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render::drawSphere(int sphereIndex, objectAttributes attr) {

	if (attr.translate != MOVE_INVALID) {
		if (attr.translate == MOVE_LEFT) {
			spherePosition[sphereIndex].x += 0.1f;
		} else if (attr.translate == MOVE_RIGHT) {
			spherePosition[sphereIndex].x -= 0.1f;
		} else if (attr.translate == MOVE_UP) {
			spherePosition[sphereIndex].y += 0.1f;
		} else {
			spherePosition[sphereIndex].y -= 0.1f;
		}
	}

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), spherePosition[sphereIndex]);
	glm::vec3 scale = glm::vec3(0.005f, 0.005f, 0.005f);
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	static float rot = 0.0f;
	if (attr.axis != ROTATE_INVALID)
	{
		if (attr.axis == ROTATE_X)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
		if (attr.axis == ROTATE_Y)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		if (attr.axis == ROTATE_Z)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
		rot = rot + 3;
		prev_axis = attr.axis;
	} else {
		if (prev_axis == ROTATE_X)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
		if (prev_axis == ROTATE_Y)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		if (prev_axis == ROTATE_Z)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (rot > 360)
		rot = 0.0f;
	glCheckError();

	GLuint lightPosID0 = glGetUniformLocation(sphereProgramId, "lightPos");
	glUniform3fv(lightPosID0, 1, &positions[attr.lightPositionIndex][0]);

	GLuint lightPosID1 = glGetUniformLocation(sphereProgramId, "lightPos1");
	glUniform3fv(lightPosID1, 1, &positions[attr.lightPositionIndex+1][0]);

	GLuint diffusedStrengthID = glGetUniformLocation(sphereProgramId, "diffuseStrength");
	glUniform1f(diffusedStrengthID, attr.diffuseStrength);

	GLuint specularStrengthID = glGetUniformLocation(sphereProgramId, "specularStrength");
	glUniform1f(specularStrengthID, attr.specularStrength);

	GLuint modelID = glGetUniformLocation(sphereProgramId, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelMatrix[0][0]);

	GLuint cameraPositionID = glGetUniformLocation(sphereProgramId, "cameraPosition");
	glUniform3fv(cameraPositionID, 1, &cameraPosition1[0]);

	glm::mat4 mvp = projectionMatrix1 * viewMatrix1 * modelMatrix;
	GLuint mvpID = glGetUniformLocation(sphereProgramId, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

	glBindVertexArray(sphereVertexArrayObject);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMapSphere);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size());
	//spherePosition[sphereIndex].y += 0.5f;
	glm::vec3 pos = spherePosition[sphereIndex];
	pos.y -= 0.5f;
	pos.z += 0.5f;
	glm::mat4 basketModel = glm::translate(glm::mat4(1.0f), pos);
	basketModel = glm::scale(basketModel, glm::vec3(0.5f, 0.5f, 0.5f));
	modelID = glGetUniformLocation(sphereProgramId, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &basketModel[0][0]);
	glBindVertexArray(grassVertexArrayObject);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render::drawSpheres(rotationAxis axis, objectDirection translate) {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if 0
	glCheckError();
	glUseProgram(sphereProgramId);
	GLuint textureID  = glGetUniformLocation(sphereProgramId, "myTextureSampler");
	glUniform1i(textureID, 0);
	glCheckError();
	setView1();
	objectAttributes attr = {
		.diffuseStrength = 0.0,
		.specularStrength = 0.0,
		.axis = axis,
		.translate = translate,
		.lightPositionIndex = 0,
	};

	// (Middle Column) Column wise First is bottom
	attr.diffuseStrength = 0.0; attr.specularStrength = 0.5;
	drawSphere(1, attr);
	drawFloor();
	drawGrass();
#endif
		
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
