#pragma once
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_float3.hpp>


glm::ivec2 inline translateGlobalToLocalCoords(glm::ivec2 globalCoords, unsigned int chunkWidthX, unsigned int chunkWidthZ)
{
	return { globalCoords.x % chunkWidthX, globalCoords.y % chunkWidthZ };
}

glm::ivec2 inline translateLocalToGlobalCoords(glm::ivec2 localCoords, glm::ivec2 chunkPos)
{
	return { chunkPos.x + localCoords.x, chunkPos.y + localCoords.y };
}

glm::ivec2 inline translateGlobalPosToChunkPos(glm::vec3 pos, int chunkWidthX, int chunkWidthZ)
{
	glm::ivec2 cPos = glm::ivec2(pos.x / chunkWidthX, pos.z / chunkWidthZ);

	if (pos.x < 0)
		cPos.x--;
	if (pos.z < 0)
		cPos.y--;

	cPos.x *= chunkWidthX;
	cPos.y *= chunkWidthZ;

	return cPos;
}

glm::ivec2 inline translateGlobalPosToChunkIndex(glm::vec3 pos, int chunkWidthX, int chunkWidthZ)
{
	glm::ivec2 cPos = glm::ivec2(pos.x / chunkWidthX, pos.z / chunkWidthZ);

	if (pos.x < 0)
		cPos.x--;
	if (pos.z < 0)
		cPos.y--;

	return cPos;
}