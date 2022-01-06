#version 400 core

layout(location = 0) in uint inVertexData;

out vec2 textureCoordinates;
out float lightLevel;

uniform mat4 u_mvp;
uniform float u_time;
uniform vec2 u_chunkPos;

#define PI 3.1415926535897932384626433832795


const float TEXTUREATLAS_WIDTH = 1152.0;
const float TEXTUREATLAS_HEIGHT = 1280.0;
const float SINGLE_IMG_WIDTH = 128.0;
const float MAX_LIGHT_VAL = 31.0;


void main()
{
	// 0x1F == 0b11111. Retrieves only the first 5 bits and translates them to an int
	int xPos = int(inVertexData & 0x1F);
	// bits need to be pushed back into position ('>> 5' because it was inserted with '<< 5')
	int yPos = int(inVertexData & 0x3E0) >> 5;
	int zPos = int(inVertexData & 0x7C00) >> 10;

	float newXPos = float(xPos) + u_chunkPos.x;
	float newZPos = float(zPos) + u_chunkPos.y;

	gl_Position = u_mvp * vec4(newXPos, float(yPos), newZPos, 1.0);

	int texIndexX = int(inVertexData & 0x78000) >> 15;
	int texIndexY = int(inVertexData & 0x780000) >> 19;

	vec2 texCoords = vec2( (float(texIndexX) * SINGLE_IMG_WIDTH) / TEXTUREATLAS_WIDTH, (float(texIndexY) * SINGLE_IMG_WIDTH) / TEXTUREATLAS_HEIGHT);
	textureCoordinates = texCoords;

	int lightVal = int(inVertexData & 0xF800000) >> 23;
	lightLevel = float(lightVal) / MAX_LIGHT_VAL;
}