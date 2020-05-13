#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "scene_graph.h"

Camera::Camera(glm::vec3 position, std::string name, unsigned int scrHeight, unsigned int scrWidth) {
	std::cout << "Camera init name " << name << "\n";
	_position = position;
	_name = name;
	_isSetup = false;
	_lightPos0 = glm::vec3(10.0f, 18.0f, 0.0f);
	_viewMatrix = glm::lookAt(
			position, // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
	_projectionMatrix = glm::perspective(glm::radians(45.0f), (float) scrWidth/ (float) scrHeight, 0.1f, 100.0f);
}

void Camera::setup(unsigned int shaderProgramId) {
	if (_isSetup)
		return;
	std::cout << "Camera name " << _name << " Setup\n";
	GLuint viewID = glGetUniformLocation(shaderProgramId, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &_viewMatrix[0][0]);
	GLuint projectionID = glGetUniformLocation(shaderProgramId, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &_projectionMatrix[0][0]);
	unsigned int lightPos0Id = glGetUniformLocation(shaderProgramId, "ligPos0");
	glUniform3fv(lightPos0Id, 1, &_lightPos0[0]);
	unsigned int cameraPositionId = glGetUniformLocation(shaderProgramId, "cameraPosition");
	glUniform3fv(cameraPositionId, 1, &_position[0]);
	_isSetup = true;
}

void Camera::setPosition(glm::vec3 location) {
	_position = location;
}

void Camera::setLightPos0(glm::vec3 position) {
	_lightPos0 = position;
}

glm::vec3 Camera::getLightPos0() {
	return _lightPos0;
}

glm::vec3 Camera::getPosition() {
	return _position;
}

void Camera::setProjectionMatix(glm::mat4 matrix) {
	_projectionMatrix = matrix;
}

void Camera::setViewMatrix(glm::mat4 matrix) {
	_viewMatrix = matrix;
}

glm::mat4 Camera::getProjectionMatrix() {
	return _projectionMatrix;
}

glm::mat4 Camera::getViewMatrix() {
	return _viewMatrix;
}
