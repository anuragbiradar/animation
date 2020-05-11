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
		unsigned int shaderProgramId;
	public:
		Material() {
			ambient = glm::vec3(0.1, 0.1, 0.1),
			diffuse = glm::vec3(0.8, 0.8, 0.8),
			specular = glm::vec3(1.0, 1.0, 1.0),
			shininess = 75.0f;
			shaderProgramId = 0;
		}
		Material(unsigned int shaderId) {
			shaderProgramId = shaderId;
		}
		~Material();
		unsigned int loadShaderFile(std::string vs, std::string fs);
		unsigned int getShaderId();
		void setShaderId(unsigned int id);
};

class Camera {
private:
	glm::vec3 _position;
	std::string _name;
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;
	bool _isSetup;
public:
	Camera(glm::vec3 location, std::string name, unsigned int scrHeight, unsigned int scrWidth);

	void setup(unsigned int id);
	void setPosition(glm::vec3 location);
	glm::vec3 getPosition();
	void setProjectionMatix(glm::mat4 matrix);
	void setViewMatrix(glm::mat4 matrix);
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
};

class sceneRender {
private:
	ply_parser _plyData;
	unsigned int _vertexArrayObject, _vertexBufferObject;
	vector<float> _vertices;
	// Add ref count for material
	Material *_material;
public:
	sceneRender();
	virtual ~sceneRender();
	void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 worldMatrix);
	void loadMeshObj(const char *plyFilePath);
	void loadMeshObj(float *vertices, int size);
	void loadMeshObjVert(const char *vertFilePath);
	void setMaterial(Material *mat);
	Material *getMaterial();
};

class sceneNode : public sceneRender {
private:
	std::string _name;
	glm::mat4 _localMatrix, _worldMatrix;
	glm::vec3 _scale, _position, _axis;
	float _rotateAngle;
	sceneNode *_parent;
	vector<sceneNode*> _childList;
public:
	sceneNode(std::string objName);
	~sceneNode();
	void addChild(sceneNode *node);
	void setPosition(glm::vec3 position);
	void setScale(glm::vec3 scale);
	void setRotation(float radians, glm::vec3 axis);
	glm::vec3 getScale();
	glm::mat4 getLocalMatrix();
	glm::mat4 getWorldMatrix();
	void displayScene(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
	std::string getName();
};
#endif
