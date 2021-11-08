#include "BlockTypes.h"

BlockType& BlockType::get()
{
	static BlockType b;
	return b;
}

std::vector<GLfloat> BlockType::getTexCoords(BLOCK_ID blockID, const unsigned char& face)
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
	case BLOCK_ID::TREE_TRUNK:
		if(face & FACES_TO_DISPLAY::TOP || face & FACES_TO_DISPLAY::BOTTOM)
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

std::vector<GLfloat> BlockType::getTextureCoordinates(glm::vec2 tilePos)
{
	std::vector<GLfloat> coords(8);

	int x = tilePos.x;
	int y = tilePos.y;

	float sizeSingle = TEXTUREATLAS_WIDTH / TEXTUREATLAS_TOTALX;

	coords[0] = (x		 * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[1] = (y		 * sizeSingle) / TEXTUREATLAS_HEIGHT;

	coords[2] = ((x + 1) * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[3] = (y		 * sizeSingle) / TEXTUREATLAS_HEIGHT;

	coords[4] = (x		 * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[5] = ((y + 1) * sizeSingle) / TEXTUREATLAS_HEIGHT;

	coords[6] = ((x + 1) * sizeSingle) / TEXTUREATLAS_WIDTH;
	coords[7] = ((y + 1) * sizeSingle) / TEXTUREATLAS_HEIGHT;
	

	return coords;
}



