#version 400 core

layout(location = 0) in vec3 inVertexPos;
layout(location = 1) in vec2 inTexCoords;

out vec2 textureCoordinates;


uniform mat4 u_mvp;
uniform float u_time;


#define PI 3.1415926535897932384626433832795


void main()
{
	gl_Position = u_mvp * vec4(inVertexPos, 1.0);
	textureCoordinates = inTexCoords;
}