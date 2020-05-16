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

#include "scene_graph.h"

sceneRender::sceneRender() {
	_material = NULL;
	_vertexArrayObject = 0;
	_vertexBufferObject = 0;
}

sceneRender::~sceneRender() {
	glDeleteBuffers(1, &_vertexArrayObject);
	glDeleteBuffers(1, &_vertexBufferObject);
}

void sceneRender::loadMeshObj(float *vertices, int size) {
	for (int i = 0; i < size; i++) {
		_vertices.push_back(vertices[i]);
	}
	glGenVertexArrays(1, &_vertexArrayObject);
	glGenBuffers(1, &_vertexBufferObject);
	glBindVertexArray(_vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void sceneRender::loadMeshObjVert(const char *vertFilePath) {
	_plyData.load_vert(vertFilePath);
	vector<float> vertices = _plyData.get_vertex();
	// Data
	for (int i = 0; i < vertices.size(); i = i + 3) {
		_vertices.push_back(vertices[i]);
		_vertices.push_back(vertices[i + 1]);
		_vertices.push_back(vertices[i + 2]);
		glm::vec3 vert = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
		float theta = atan2(vert.z, vert.x);
		float u = (theta + PI) / (2 * PI);
		float phi = acos(vert.y / glm::length(vert));
		float v = phi / PI;
		_vertices.push_back(u);
		_vertices.push_back(v);
	}
	glGenVertexArrays(1, &_vertexArrayObject);
	glGenBuffers(1, &_vertexBufferObject);
	glBindVertexArray(_vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void sceneRender::loadMeshObj(const char *plyFilePath) {
	_plyData.load_ply(plyFilePath);
	vector<point> points = _plyData.get_element_face_points();
	std::cout << "loadPly size " << points.size() << "\n"; 
	for (int i = 0; i < points.size(); i++) {
		// Vertices
		_vertices.push_back(points[i].x);
		_vertices.push_back(points[i].y);
		_vertices.push_back(points[i].z);
		// Normals
		GLfloat x, y, z;
		points[i].get_avg_vertex_normal(&x, &y, &z);
		_vertices.push_back(x);
		_vertices.push_back(y);
		_vertices.push_back(z);
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
		_vertices.push_back(u);
		_vertices.push_back(v);
	}

	// Sphere Data
	glGenVertexArrays(1, &_vertexArrayObject);
	glGenBuffers(1, &_vertexBufferObject);
	glBindVertexArray(_vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), &_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void sceneRender::setMaterial(Material *material) {
	_material = material;
}

Material* sceneRender::getMaterial() {
	return _material;
}

void sceneRender::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 worldMatrix, glm::vec3 ligPos0) {
	if (!_material) {
		std::cout << "Material is not for this object\n";
		return;
	}
	unsigned int shaderProgramId = _material->getShaderId();
	glUseProgram(shaderProgramId);
	GLuint viewID = glGetUniformLocation(shaderProgramId, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLuint projectionID = glGetUniformLocation(shaderProgramId, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	unsigned modelID = glGetUniformLocation(shaderProgramId, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &worldMatrix[0][0]);
	unsigned ligPos0ID = glGetUniformLocation(shaderProgramId, "ligPos0");
	glUniform3fv(ligPos0ID, 1, &ligPos0[0]);
	unsigned int textureID  = glGetUniformLocation(shaderProgramId, "myTextureSampler");
	glUniform1i(textureID, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _material->getTextureID());
	glBindVertexArray(_vertexArrayObject);
	//std::cout << "Size "<<_vertices.size() << std::endl;
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
}

sceneNode::sceneNode(std::string objName) {
	_name = objName;
	_worldMatrix = glm::mat4(1);
	_localMatrix = glm::mat4(1);
	_scale = glm::vec3(1.0f);
	_position = glm::vec3(0.0f);
	_rotateAngle = 0.0f;
	_parent = NULL;
}

sceneNode::~sceneNode() {
	vector<sceneNode*>::iterator it;
	for (it = _childList.begin(); it != _childList.end(); it++) {
		delete((*it));
	}
}

glm::vec3 sceneNode::getScale() {
	return _scale;
}

void sceneNode::setPosition(glm::vec3 translation) {
	_position += translation;
	_localMatrix = glm::mat4(1);
	//std::cout << "SetPosition " << _name << " vvalue " << glm::to_string(_position) << std::endl;
	_localMatrix = glm::scale(_localMatrix, _scale);
	if (_rotateAngle != 0) {
		//std::cout << "Set Rotation\n";
		_localMatrix = glm::rotate(_localMatrix, _rotateAngle, _axis);
	}
	_localMatrix = glm::translate(_localMatrix, _position);
}

void sceneNode::setScale(glm::vec3 scale) {
	_scale = scale;
	_localMatrix = glm::mat4(1);
	_localMatrix = glm::scale(_localMatrix, scale);
	_localMatrix = glm::translate(_localMatrix, _position);
}

void sceneNode::setRotation(float radian, glm::vec3 axis) {
	// Check for init
	_rotateAngle = radian;
	_axis = axis;
	_localMatrix = glm::rotate(_localMatrix, radian, axis);
}

glm::mat4 sceneNode::getWorldMatrix() {
	return _worldMatrix;
}

glm::mat4 sceneNode::getLocalMatrix() {
	return _localMatrix;
}

void sceneNode::addChild(sceneNode *node) {
	_childList.push_back(node);
	node->_parent = this;
}

void sceneNode::displayScene(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::vec3 ligPos0) {
	//std::cout << "Draw object " << name << "\n";
	if (_parent) {
		_worldMatrix = _parent->getWorldMatrix() * _localMatrix;
	} else {
		_worldMatrix = _localMatrix;
	}
	Material *_mat = getMaterial();

	//std::cout << "Draw " << _name << std::endl;
	draw(projectionMatrix, viewMatrix, _worldMatrix, ligPos0);
	
	vector<sceneNode*>::iterator it;
	for (it = _childList.begin(); it != _childList.end(); it++) {
		(*it)->displayScene(projectionMatrix, viewMatrix, ligPos0);
	}
}

std::string sceneNode::getName() {
	return _name;
}
