#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <sstream>
#include <math.h>

#include "scene_graph.h"

unsigned int Texture::loadTexture(const char *texturePath) {
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrComponents, 0);
	if (data) {
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << texturePath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

Camera::Camera(glm::vec3 location, std::string name, unsigned int scrHeight, unsigned int scrWidth) {
	std::cout << "Camera init name " << name << "\n";
	position = location;
	this->name = name;
	isSetup = false;
	viewMatrix = glm::lookAt(
			position, // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float) scrWidth/ (float) scrHeight, 0.1f, 100.0f);
}

void Camera::setup(unsigned int shaderProgramId) {
	if (isSetup)
		return;
	std::cout << "Camera name " << name << " Setup\n";
	GLuint viewID = glGetUniformLocation(shaderProgramId, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLuint projectionID = glGetUniformLocation(shaderProgramId, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	isSetup = true;
}


sceneNode::sceneNode(std::string objName, glm::vec3 scale, glm::vec3 translate) {
	name = objName;
	modelMatrix = glm::mat4(1);
	this->scale = scale;
	this->translate = translate;
	modelMatrix = glm::translate(modelMatrix, translate);
	//modelMatrix = glm::scale(modelMatrix, scale);
}

sceneNode::~sceneNode() {
	glDeleteBuffers(1, &vertexArrayObject);
	glDeleteBuffers(1, &vertexBufferObject);
}

void sceneNode::init() {

}

void sceneNode::loadMeshObj(const char *plyFilePath) {
	plyData.load_ply(plyFilePath);
	vector<point> points = plyData.get_element_face_points();
	std::cout << "loadPly size " << points.size() << "\n"; 
	for (int i = 0; i < points.size(); i++) {
		// Vertices
		vertices.push_back(points[i].x);
		vertices.push_back(points[i].y);
		vertices.push_back(points[i].z);
		// Normals
		GLfloat x, y, z;
		points[i].get_avg_vertex_normal(&x, &y, &z);
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
		// Calculate UV
#if 0
		float u = asin(x)/PI + 0.5;
		float v = asin(y)/PI + 0.5;
		cout << "u v " << u << " " << v << endl;
		vertex.push_back(u);
		vertex.push_back(v);
#endif
		// Texture Coordinate
		glm::vec3 vert = glm::vec3(points[i].x, points[i].y, points[i].z);
		float theta = atan2(vert.z, vert.x);
		float u = (theta + PI) / (2 * PI);
		float phi = acos(vert.y / glm::length(vert));
		float v = phi / PI;
		vertices.push_back(u);
		vertices.push_back(v);
	}

	// Sphere Data
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(1, &vertexBufferObject);
	glBindVertexArray(vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void sceneNode::loadMeshObj(float *vertex, int size) {
	for (int i = 0; i < size; i++) {
		vertices.push_back(vertex[i]);
	}
	std::cout << "Name " << name << " size "<< vertices.size() << "\n";
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(1, &vertexBufferObject);
	glBindVertexArray(vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void sceneNode::setMaterial(Material value) {
	material = value;
}

void sceneNode::setTransformation(glm::vec3 translation) {
#if 1
	std::cout << "Name " << name << " set translate\n";
	//std::cout << "Before Content " << glm::to_string(modelMatrix) << "\n";
	std::cout << "Before Translate " << glm::to_string(translate) << "\n";
	translate += translation;
	if (parent) {
		std::cout << "Has parent\n";
		modelMatrix = glm::translate(parent->getModelMatrix(), translate);
	std::cout << "Name " << name << " set translate\n";
	//std::cout << "After Content " << glm::to_string(modelMatrix) << "\n";
	std::cout << "After Translate " << glm::to_string(translate) <<"\n";
		return;
	}
	modelMatrix = glm::translate(modelMatrix, translate);
	std::cout << "Name " << name << " set translate\n";
	//std::cout << "After Content " << glm::to_string(modelMatrix) << "\n";
	std::cout << "After Translate " << glm::to_string(translate) <<"\n";
#endif
}

void sceneNode::setTransformation(glm::mat4 matrix) {
	modelMatrix = matrix;
}

void sceneNode::setScale(glm::vec3 scale) {
	// Check for init
	//modelMatrix = glm::scale(modelMatrix, scale);
	this->scale = scale;
	//std::cout << "Name " << name << " set scale\n";
	//std::cout << "Content " << glm::to_string(modelMatrix) << "\n";
}

void sceneNode::setRotation(float radian, glm::vec3 axis) {
	// Check for init
	modelMatrix = glm::rotate(modelMatrix, radian, axis);
}

void sceneNode::draw(unsigned int shaderProgramId) {
	//std::cout << "Draw object " << name << "\n";
	//Check parent data	
	//glUseProgram(shaderProgramId);
	//std::cout << "X " << this->scale.x << "\n";
	glm::mat4 matrix = glm::scale(modelMatrix, scale);
	//if(name.compare("Sphere") == 0)
	//	return;
	GLuint modelID = glGetUniformLocation(shaderProgramId, "model");
	//std::cout << "Set modelMatix\n";
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &matrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material.getTextureID());
	glBindVertexArray(vertexArrayObject);
	//std::cout << "Size " << vertices.size() << "\n";
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

sceneGraph::~sceneGraph() {
	vector<sceneNode*>::iterator it;
	for (it = childList.begin(); it != childList.end(); it++) {
		delete((*it));
	}
}

void sceneGraph::init(Camera *camera) {
	this->camera = camera;
}

void sceneGraph::setup() {
	glUseProgram(shaderProgramId);
	camera->setup(shaderProgramId);
}

void sceneGraph::update(glm::vec3 translate) {
	//GLuint viewID = glGetUniformLocation(shaderProgramId, "view");
	//glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);

	vector<sceneNode*>::iterator it;
	for (it = childList.begin(); it != childList.end(); it++) {
		std::cout << "ChildList name " << (*it)->getName() << "\n";
		(*it)->setTransformation(translate);
	}

}

void sceneGraph::displayScene() {
	vector<sceneNode*>::iterator it;
	//std::cout << "Set ViewMatix\n";
	//std::cout << "Set ProjectionMatix\n";
	GLuint textureID  = glGetUniformLocation(shaderProgramId, "myTextureSampler");
	glUniform1i(textureID, 0);

	for (it = childList.begin(); it != childList.end(); it++) {
		(*it)->draw(shaderProgramId);
	}
}
