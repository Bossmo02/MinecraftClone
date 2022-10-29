#version 400 core

layout(location = 0) in vec3 inVertexPos;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in float inLightLevel;

out vec2 textureCoordinates;
out float lightLevel;
out float debugHighlight;


uniform mat4 u_mvp;
uniform vec2 u_chunkPos;
uniform float u_debugHighlight;


void main()
{
	float newXPos = inVertexPos.x + u_chunkPos.x;
	float newZPos = inVertexPos.z + u_chunkPos.y;


	gl_Position = u_mvp * vec4(newXPos, inVertexPos.y, newZPos, 1.0);
	textureCoordinates = inTexCoords;
	lightLevel = inLightLevel;

	debugHighlight = u_debugHighlight;
}