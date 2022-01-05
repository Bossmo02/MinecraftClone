#ifndef BLOCK_TYPES_HPP
#define BLOCK_TYPES_HPP

#include <glm/vec2.hpp>

#include <vector>

#include "BlockMesh.h"


#define TEXTUREATLAS_WIDTH 1152
#define TEXTUREATLAS_HEIGHT 1280
#define TEXTUREATLAS_TOTALX	9
#define TEXTUREATLAS_TOTALY 10


	// all coords begin at the bottom left corner
#define DEBUG_TILE glm::vec2(7, 2)

#define DIRT_TILE glm::vec2(7, 4)
#define GRASS_TILE glm::vec2(6, 8)
#define GRASS_SIDE_TILE glm::vec2(7, 5)
#define STONE_TILE glm::vec2(3, 5)
#define WATER_TILE glm::vec2(0, 3)
#define SAND_TILE glm::vec2(3, 3)
#define TREE_TRUNK_TILE glm::vec2(0, 0)
#define TREE_TRUNK_SIDE_TILE glm::vec2(1, 9)
#define TREE_LEAVES_TILE glm::vec2(4, 1)


enum class BLOCK_ID
{
	GRASS = 0,
	DIRT,
	STONE,
	SAND,
	WATER,
	TREE_TRUNK,
	TREE_LEAVES
};

enum FACES_TO_DISPLAY
{
	TOP = 0x01,
	BOTTOM = 0x02,
	FRONT = 0x04,
	BACK = 0x08,
	RIGHT = 0x10,
	LEFT = 0x20,
	ALL = 0x3F
};



std::vector<GLfloat> inline getTextureCoordinates(glm::vec2 tilePos)
{
	std::vector<GLfloat> coords(8);

	int x = tilePos.x;
	int y = tilePos.y;

	float sizeSingle = TEXTUREATLAS_WIDTH / TEXTUREATLAS_TOTALX;

	coords[0] = (x * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[1] = (y * sizeSingle) / TEXTUREATLAS_HEIGHT;

	coords[2] = ((x + 1) * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[3] = (y * sizeSingle) / TEXTUREATLAS_HEIGHT;

	coords[4] = (x * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[5] = ((y + 1) * sizeSingle) / TEXTUREATLAS_HEIGHT;

	coords[6] = ((x + 1) * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[7] = ((y + 1) * sizeSingle) / TEXTUREATLAS_HEIGHT;


	return coords;
}


std::vector<GLfloat> inline getTexCoords(BLOCK_ID blockID, const unsigned char& face)
{
	switch (blockID)
	{
	case BLOCK_ID::GRASS:
		if (face & FACES_TO_DISPLAY::TOP)
			return getTextureCoordinates(GRASS_TILE);
		else if (face & FACES_TO_DISPLAY::BOTTOM)
			return getTextureCoordinates(DIRT_TILE);
		else
			return getTextureCoordinates(GRASS_SIDE_TILE);
		break;
	case BLOCK_ID::DIRT:
		return getTextureCoordinates(DIRT_TILE);
		break;
	case BLOCK_ID::STONE:
		return getTextureCoordinates(STONE_TILE);
		break;
	case BLOCK_ID::SAND:
		return getTextureCoordinates(SAND_TILE);
		break;
	case BLOCK_ID::WATER:
		return getTextureCoordinates(WATER_TILE);
		break;
	case BLOCK_ID::TREE_TRUNK:
		if (face & FACES_TO_DISPLAY::TOP || face & FACES_TO_DISPLAY::BOTTOM)
			return getTextureCoordinates(TREE_TRUNK_TILE);
		else
			return getTextureCoordinates(TREE_TRUNK_SIDE_TILE);
		break;
	case BLOCK_ID::TREE_LEAVES:
		return getTextureCoordinates(TREE_LEAVES_TILE);
		break;
	default:
		return getTextureCoordinates(DEBUG_TILE);
		break;
	}
}


#endif // !BLOCK_TYPES_HPP
