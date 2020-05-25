#ifndef _RENDER_
#define _RENDER_

#include "ply_parser.h"
#include "scene_graph.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class render {
	private:
		uint32_t SCR_WIDTH;
		uint32_t SCR_HEIGHT;
		vector<sceneNode*> graphList;
		Camera *camera;
		Camera *camera1;
		renderEnv *env;
		vector<Material*> materialList;
	public:
		render();
		~render();
		void initRender(int width, int height);
		void renderScene(const char *cameraName);
};
#endif
