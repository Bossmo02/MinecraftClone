#version 400 core

layout(location = 0) in vec3 inVertexPos;
layout(location = 1) in vec2 inTexCoords;

out vec2 textureCoordinates;
out float lightLevel;

uniform mat4 u_mvp;
uniform float u_time;
uniform vec2 u_chunkPos;

#define PI 3.1415926535897932384626433832795



void main()
{
	float newXPos = inVertexPos.x + u_chunkPos.x;
	float newZPos = inVertexPos.z + u_chunkPos.y;

	float newYPos = inVertexPos.y + 0.15 * sin(u_time);

	gl_Position = u_mvp * vec4(newXPos, newYPos, newZPos, 1.0);
	textureCoordinates = inTexCoords;

	lightLevel = 1.0;
}