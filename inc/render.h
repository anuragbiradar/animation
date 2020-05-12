#ifndef _RENDER_
#define _RENDER_

#include "ply_parser.h"
#include "scene_graph.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

typedef enum _objectDirection {
	MOVE_INVALID = 0xff,
	MOVE_LEFT = 0x00,
	MOVE_RIGHT = 0x01,
	MOVE_UP = 0x03,
	MOVE_DOWN = 0x04
} objectDirection;

typedef enum _rotationAxis {
	ROTATE_INVALID = 0xff,
	ROTATE_X = 0x00,
	ROTATE_Y = 0x01,
	ROTATE_Z = 0x02,
} rotationAxis;

typedef struct _objectAttributes {
	float diffuseStrength;
	float specularStrength;
	rotationAxis axis;
	objectDirection translate;
	int lightPositionIndex;
} objectAttributes;

typedef enum _objectOperation {
	OBJECT_ROTATE = 0x01,
	OBJECT_SCALE = 0x02,
	OBJECT_POSITION = 0x04,
} objectOperation;

typedef struct _objectStatusUpdate {
	std::string objName;
	bool isUpdated;
	objectOperation operation;
	glm::vec3 position;
	float scale;
	float radian;
	rotationAxis axis;
} objectStatusUpdate;

class render {
	private:
		uint32_t SCR_WIDTH;
		uint32_t SCR_HEIGHT;
		//GLuint sphereProgramId;
		//Texture texture;
		vector<sceneNode*> graphList;
		Camera *camera;
		Camera *camera1;
		vector<Material*> materialList;
	public:
		render();
		~render();
		void initRender(int width, int height, ply_parser *parser);
		void drawSphere(int sphereIndex, objectAttributes attr);
		void drawSpheres(rotationAxis axis, objectDirection translate, objectStatusUpdate update);
};
#endif
