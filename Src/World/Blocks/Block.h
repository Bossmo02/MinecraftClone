#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "../../Renderer/RenderContext.h"
#include "BlockTypes.h"


class Block
{
public:

	Block(int x, int y, int z, BLOCK_ID id);
	Block(int x, int y, int z, BLOCK_ID id, unsigned char dir);

	void setFaceVisible(unsigned char dir);
	void setFaceHidden(unsigned char dir);
	int getNumOfFacesVisible() const;
	const glm::ivec3 getWorldPos() const;
	bool isFaceVisible(unsigned char dir) const;
	const BLOCK_ID getBlockType();

	std::vector<BlockMeshData> getVisibleMesh();

private:

	void setVerticesToWorldPos(std::vector<GLfloat>& verts);

	glm::ivec3 m_worldPos;
	BLOCK_ID m_blockID;

	unsigned char m_visibleFaces;
};




#endif // !BLOCK_HPP
