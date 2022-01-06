#ifndef BLOCK_MESH_HPP
#define BLOCK_MESH_HPP

#include <glad/glad.h>
#include <glm/vec3.hpp>

#include <deque>
#include <vector>


#define DIRECTION_BACK	glm::ivec3( 0,  0, -1)
#define DIRECTION_FRONT glm::ivec3( 0,  0,  1)
#define DIRECTION_LEFT	glm::ivec3(-1,  0,  0)
#define DIRECTION_RIGHT glm::ivec3( 1,  0,  0)
#define DIRECTION_DOWN	glm::ivec3( 0, -1,  0)
#define DIRECTION_UP	glm::ivec3( 0,  1,  0)

// represents a single face
struct BlockMeshData
{
	std::deque<GLfloat> vertices;
	std::deque<GLfloat> texCoords;
	std::deque<GLuint> indices;
	GLfloat lightLevel;
};

BlockMeshData getBlockBackMeshData(int indicesIndex);
BlockMeshData getBlockFrontMeshData(int indicesIndex);
BlockMeshData getBlockLeftMeshData(int indicesIndex);
BlockMeshData getBlockRightMeshData(int indicesIndex);
BlockMeshData getBlockBottomMeshData(int indicesIndex);
BlockMeshData getBlockTopMeshData(int indicesIndex);


std::deque<BlockMeshData> getAllBlockData(int indicesIndex);


struct BlockMeshDataForSingleVBO
{
	// values from 0 to 31 are expected
	std::deque<int> vertices;
	// x-values from 0 to 8 and y-values from 0 to 8 are expected
	std::deque<int> texCoords;
	std::deque<GLuint> indices;

	// lightLevels from 0 to 31 are valid
	int lightLevel;
};

BlockMeshDataForSingleVBO getBlockBackMeshDataForSingleVBO(int indicesIndex);
BlockMeshDataForSingleVBO getBlockFrontMeshDataForSingleVBO(int indicesIndex);
BlockMeshDataForSingleVBO getBlockLeftMeshDataForSingleVBO(int indicesIndex);
BlockMeshDataForSingleVBO getBlockRightMeshDataForSingleVBO(int indicesIndex);
BlockMeshDataForSingleVBO getBlockBottomMeshDataForSingleVBO(int indicesIndex);
BlockMeshDataForSingleVBO getBlockTopMeshDataForSingleVBO(int indicesIndex);

#endif // !BLOCK_MESH_HPP
