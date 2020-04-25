#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec2 aTextureCoord;

out vec3 fragVertex;
out vec2 textureCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 MVP;

void main() {
	fragVertex = vertexPosition;
	textureCoord = aTextureCoord;
	gl_Position = MVP * vec4(vertexPosition, 1.0f);
}
