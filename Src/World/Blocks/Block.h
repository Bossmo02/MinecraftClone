#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "../../Renderer/RenderContext.h"
#include "BlockTypes.h"


class Block
{
public:

	Block(float x, float y, float z, BLOCK_ID id);
	Block(float x, float y, float z, BLOCK_ID id, unsigned char dir);

	void setFaceVisible(unsigned char dir);
	void setFaceHidden(unsigned char dir);
	int getNumOfFacesVisible() const;
	const glm::ivec3 getWorldPos() const;
	bool isFaceVisible(unsigned char dir) const;
	const BLOCK_ID getBlockType();

	std::vector<BlockMeshData> getVisibleMesh(float scale = 1);


protected:

	glm::vec3 m_worldPos;

private:

	void setVerticesToWorldPosScaled(float scale, std::vector<GLfloat>& verts);
	void setVerticesToWorldPos(std::vector<GLfloat>& verts);

	
	BLOCK_ID m_blockID;

	unsigned char m_visibleFaces;
};




#endif // !BLOCK_HPP
