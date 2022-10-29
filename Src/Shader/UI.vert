#version 400 core

layout(location = 0) in vec2 inVertexPos;
layout(location = 1) in vec2 inTexCoords;

out vec2 textureCoordinates;


void main()
{
	gl_Position = vec4(inVertexPos.x, inVertexPos.y, 0.0, 1.0);
	textureCoordinates = inTexCoords;
}