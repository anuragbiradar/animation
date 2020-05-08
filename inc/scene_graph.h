#ifndef _SCENE_GRAPH_
#define _SCENE_GRAPH_
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ply_parser.h"

#define PI 3.14159265

class Texture {
	private:
		unsigned int textureID;
		char texturePath[50]; 
	public:
		Texture() {
			texturePath[0] = '0';
			textureID = 0;
		}
		unsigned int loadTexture(const char *texturePath);
		inline unsigned int getTextureID() {
			return textureID;
		}
};

class Material : public Texture {
	private:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	public:
		Material() {
			ambient = glm::vec3(0.1, 0.1, 0.1),
			diffuse = glm::vec3(0.8, 0.8, 0.8),
			specular = glm::vec3(1.0, 1.0, 1.0),
			shininess = 75.0f;
		}	
};

class Camera {
private:
	glm::vec3 position;
	std::string name;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	bool isSetup;
public:
	Camera(glm::vec3 location, std::string name, unsigned int scrHeight, unsigned int scrWidth);

	void setup(unsigned int id);
	void setPosition(glm::vec3 location) {
		position = location;
	}
	glm::vec3 getPosition() {
		return position;
	}
	void setProjectionMatix(glm::mat4 matrix) {
		projectionMatrix = matrix;
	}
	void setViewMatrix(glm::mat4 matrix) {
		viewMatrix = matrix;
	}
	glm::mat4 getProjectionMatrix() {
		return projectionMatrix;
	}
	glm::mat4 getViewMatrix() {
		return viewMatrix;
	}
};

class sceneNode {
private:
	ply_parser plyData;
	std::string name;
	glm::mat4 localMatrix;
	glm::mat4 worldMatrix;
	glm::mat4 modelMatrix;
	glm::vec3 scale;
	glm::vec3 translate;
	sceneNode *parent;
	unsigned vertexArrayObject, vertexBufferObject;
	vector<float> vertices;
public:
	Material material;
	sceneNode(std::string objName) {
		name = objName;
		modelMatrix = glm::mat4(1);
		localMatrix = glm::mat4(1);
		worldMatrix = glm::mat4(1);
		scale = glm::vec3(1.0f);
		translate = glm::vec3(0.0f);
		parent = NULL;
	}
	sceneNode(std::string objName, glm::vec3 scale, glm::vec3 translate);
	~sceneNode();
	inline void setParent(sceneNode *node) {
		parent = node;
	}
	inline std::string getName() {
		return name;
	}
	void init();
	void setup();
	void loadMeshObj(const char *plyFilePath);
	void loadMeshObj(float *vertexs, int size);
	void setMaterial(Material value);
	void setTransformation(glm::vec3 translation);
	void setTransformation(glm::mat4 matrix);
	void setScale(glm::vec3 scale);
	void setRotation(float radians, glm::vec3 axis);
	void draw(unsigned int shaderProgramId);
	glm::mat4 getModelMatrix() {
		return modelMatrix;
	}
	glm::mat4 getLocalMatrix() {
		return localMatrix;
	}
	glm::mat4 getWorldMatrix() {
		return worldMatrix;
	}
};

class sceneGraph {
private:
	// make sure parent always at 0 index
	vector<sceneNode*> childList;
	std::string name;
	Camera *camera;
	unsigned int shaderProgramId;
public:
	sceneGraph(sceneNode *parent, std::string name) {
		childList.push_back(parent);
		parent->setParent(NULL);
		this->name = name;
	}
	~sceneGraph();
	void setShaderProgramId(unsigned int id) {
		shaderProgramId = id;
	}
	// Init project matrix and view matrix
	void init(Camera *camera);

	void addChild(sceneNode *node) {
		childList.push_back(node);
		node->setParent(childList[0]);
	}
	inline std::string getName() {
		return name;
	}
	void setup();
	void displayScene();
	void update(glm::vec3 translate);
};

#endif
