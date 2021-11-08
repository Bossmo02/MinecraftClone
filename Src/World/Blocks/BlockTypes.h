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
#define DEBUG_TILE glm::vec2(0, 0)

#define DIRT_TILE glm::vec2(7, 4)
#define GRASS_TILE glm::vec2(6, 8)
#define GRASS_SIDE_TILE glm::vec2(7, 5)
#define STONE_TILE glm::vec2(3, 5)
#define WATER_TILE glm::vec2(0, 3)
#define TREE_TRUNK_TILE glm::vec2(0, 0)
#define TREE_TRUNK_SIDE_TILE glm::vec2(1, 9)
#define TREE_LEAVES_TILE glm::vec2(4, 1)


enum class BLOCK_ID
{
	GRASS = 0,
	DIRT,
	STONE,
	WATER,
	TREE_TRUNK,
	TREE_LEAVES
};

enum FACES_TO_DISPLAY
{
	TOP		= 0x01,
	BOTTOM	= 0x02,
	FRONT	= 0x04,
	BACK	= 0x08,
	RIGHT	= 0x10,
	LEFT	= 0x20,
	ALL		= 0x3F
};



class BlockType
{
public:

	static BlockType& get();

	std::vector<GLfloat> getTexCoords(BLOCK_ID blockID, const unsigned char& face);


protected:

	glm::vec2 m_topTexCoordAtlas = DEBUG_TILE;
	glm::vec2 m_sidesTexCoordAtlas = DEBUG_TILE;
	glm::vec2 m_bottomTexCoordAtlas = DEBUG_TILE;


	std::vector<GLfloat> getTextureCoordinates(glm::vec2 tilePos);

private:
	
	BlockType() = default;
	BlockType(const BlockType& other) = delete;
	BlockType operator = (const BlockType& other) = delete;

};



#endif // !BLOCK_TYPES_HPP
