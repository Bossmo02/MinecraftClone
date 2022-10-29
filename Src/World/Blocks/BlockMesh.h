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


#endif // !BLOCK_MESH_HPP
