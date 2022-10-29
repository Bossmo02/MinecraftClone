#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "../../Renderer/RenderContext.h"
#include "BlockTypes.h"


class Block
{
public:

	Block() = default;
	Block(float x, float y, float z, BLOCK_ID id);
	Block(float x, float y, float z, BLOCK_ID id, unsigned char dir);

	void setFaceVisible(unsigned char dir);
	void setFaceHidden(unsigned char dir);
	int getNumOfFacesVisible() const;
	const glm::vec3 getLocalPos() const;
	bool isFaceVisible(unsigned char dir) const;
	const BLOCK_ID getBlockType();

	std::vector<BlockMeshData> getVisibleMesh(float scale = 1);


protected:

	glm::vec<2, int8_t> m_posInChunk;
	float m_yPos;

private:

	void setVerticesToChunkPosScaled(float scale, std::deque<GLfloat>& verts);
	void setVerticesToChunkPosScaled(float scale, std::deque<int>& verts);
	void setVerticesToChunkPos(std::deque<GLfloat>& verts);

	BLOCK_ID m_blockID;

	unsigned char m_visibleFaces;
};




#endif // !BLOCK_HPP
