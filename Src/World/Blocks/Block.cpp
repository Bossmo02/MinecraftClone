#include "Block.h"

Block::Block(int x, int y, int z, BLOCK_ID id)
{
	m_worldPos = glm::ivec3(x, y, z);
	m_blockID = id;

	// blocks are invisible by default
	// you need to set up their faces individually
	m_visibleFaces = 0;
}

Block::Block(int x, int y, int z, BLOCK_ID id, unsigned char dir)
{
	m_worldPos = glm::ivec3(x, y, z);
	m_blockID = id;

	// blocks are invisible by default
	// you need to set up their faces individually
	m_visibleFaces = dir;
}


void Block::setFaceVisible(unsigned char dir)
{
	// dir should be a type of FACES_TO_DISPLAY
	m_visibleFaces |= dir;
}

void Block::setFaceHidden(unsigned char dir)
{
	// dir should be a type of FACES_TO_DISPLAY
	m_visibleFaces &= ~dir;
}

int Block::getNumOfFacesVisible() const
{
	int amount = 0;

	if (m_visibleFaces & FACES_TO_DISPLAY::BACK)
		amount++;
	if (m_visibleFaces & FACES_TO_DISPLAY::FRONT)
		amount++;
	if (m_visibleFaces & FACES_TO_DISPLAY::LEFT)
		amount++;
	if (m_visibleFaces & FACES_TO_DISPLAY::RIGHT)
		amount++;
	if (m_visibleFaces & FACES_TO_DISPLAY::TOP)
		amount++;
	if (m_visibleFaces & FACES_TO_DISPLAY::BOTTOM)
		amount++;

	return amount;
}

const glm::ivec3 Block::getWorldPos() const
{
	return m_worldPos;
}

bool Block::isFaceVisible(unsigned char dir) const
{
	if (m_visibleFaces & dir)
		return true;

	return false;
}

const BLOCK_ID Block::getBlockType()
{
	return m_blockID;
}


std::vector<BlockMeshData> Block::getVisibleMesh()
{
	unsigned int timesFacesAdded = 0;
	std::vector<BlockMeshData> faceMeshes;

	if (m_visibleFaces & FACES_TO_DISPLAY::BACK)
	{
		BlockMeshData mesh = getBlockBackMeshData(timesFacesAdded);

		setVerticesToWorldPos(mesh.vertices);
		
		mesh.texCoords = BlockType::get().getTexCoords(m_blockID, FACES_TO_DISPLAY::BACK);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::FRONT)
	{
		BlockMeshData mesh = getBlockFrontMeshData(timesFacesAdded);

		setVerticesToWorldPos(mesh.vertices);

		mesh.texCoords = BlockType::get().getTexCoords(m_blockID, FACES_TO_DISPLAY::FRONT);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::LEFT)
	{
		BlockMeshData mesh = getBlockLeftMeshData(timesFacesAdded);

		setVerticesToWorldPos(mesh.vertices);

		mesh.texCoords = BlockType::get().getTexCoords(m_blockID, FACES_TO_DISPLAY::LEFT);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::RIGHT)
	{
		BlockMeshData mesh = getBlockRightMeshData(timesFacesAdded);

		setVerticesToWorldPos(mesh.vertices);

		mesh.texCoords = BlockType::get().getTexCoords(m_blockID, FACES_TO_DISPLAY::RIGHT);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::BOTTOM)
	{
		BlockMeshData mesh = getBlockBottomMeshData(timesFacesAdded);

		setVerticesToWorldPos(mesh.vertices);

		mesh.texCoords = BlockType::get().getTexCoords(m_blockID, FACES_TO_DISPLAY::BOTTOM);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::TOP)
	{
		BlockMeshData mesh = getBlockTopMeshData(timesFacesAdded);

		setVerticesToWorldPos(mesh.vertices);

		mesh.texCoords = BlockType::get().getTexCoords(m_blockID, FACES_TO_DISPLAY::TOP);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	
	return faceMeshes;
}

void Block::setVerticesToWorldPos(std::vector<GLfloat>& verts)
{
	for (size_t i = 0; i < verts.size(); i += 3)
	{
		verts[i]	 = verts[i]		+ m_worldPos.x;
		verts[i + 1] = verts[i + 1]	+ m_worldPos.y;
		verts[i + 2] = verts[i + 2] + m_worldPos.z;
	}
}


