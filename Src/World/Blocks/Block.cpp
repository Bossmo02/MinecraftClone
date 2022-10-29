#include "Block.h"

Block::Block(float x, float y, float z, BLOCK_ID id)
{
	m_posInChunk = glm::vec2(x, z);
	m_yPos = y;
	m_blockID = id;

	// blocks are invisible by default
	// you need to set up their faces individually
	m_visibleFaces = 0;
}

Block::Block(float x, float y, float z, BLOCK_ID id, unsigned char dir)
{
	m_posInChunk = glm::vec2(x, z);
	m_yPos = y;
	m_blockID = id;

	if (id == BLOCK_ID::WATER && y == 1)
	{
		std::cout << "Ooopsie....\n";
	}

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

const glm::vec3 Block::getLocalPos() const
{
	return glm::vec3(m_posInChunk.x, m_yPos, m_posInChunk.y);
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


std::vector<BlockMeshData> Block::getVisibleMesh(float scale)
{
	unsigned int timesFacesAdded = 0;
	std::vector<BlockMeshData> faceMeshes;

	int numOfFaces = getNumOfFacesVisible();
	faceMeshes.reserve(numOfFaces);

	if (m_visibleFaces & FACES_TO_DISPLAY::BACK)
	{
		BlockMeshData mesh = getBlockBackMeshData(timesFacesAdded);

		setVerticesToChunkPosScaled(scale, mesh.vertices);
		
		mesh.texCoords = getTexCoords(m_blockID, FACES_TO_DISPLAY::BACK);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::FRONT)
	{
		BlockMeshData mesh = getBlockFrontMeshData(timesFacesAdded);

		setVerticesToChunkPosScaled(scale, mesh.vertices);

		mesh.texCoords = getTexCoords(m_blockID, FACES_TO_DISPLAY::FRONT);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::LEFT)
	{
		BlockMeshData mesh = getBlockLeftMeshData(timesFacesAdded);

		setVerticesToChunkPosScaled(scale, mesh.vertices);

		mesh.texCoords = getTexCoords(m_blockID, FACES_TO_DISPLAY::LEFT);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::RIGHT)
	{
		BlockMeshData mesh = getBlockRightMeshData(timesFacesAdded);

		setVerticesToChunkPosScaled(scale, mesh.vertices);

		mesh.texCoords = getTexCoords(m_blockID, FACES_TO_DISPLAY::RIGHT);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::BOTTOM)
	{
		BlockMeshData mesh = getBlockBottomMeshData(timesFacesAdded);

		setVerticesToChunkPosScaled(scale, mesh.vertices);

		mesh.texCoords = getTexCoords(m_blockID, FACES_TO_DISPLAY::BOTTOM);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	if (m_visibleFaces & FACES_TO_DISPLAY::TOP)
	{
		BlockMeshData mesh = getBlockTopMeshData(timesFacesAdded);

		setVerticesToChunkPosScaled(scale, mesh.vertices);

		mesh.texCoords = getTexCoords(m_blockID, FACES_TO_DISPLAY::TOP);

		faceMeshes.push_back(mesh);
		timesFacesAdded++;
	}
	
	return faceMeshes;
}


void Block::setVerticesToChunkPosScaled(float scale, std::deque<GLfloat>& verts)
{
	for (size_t i = 0; i < verts.size(); i += 3)
	{
		verts[i] = (verts[i] * scale) + m_posInChunk.x;
		verts[i + 1] = (verts[i + 1] * scale) + m_yPos;
		verts[i + 2] = (verts[i + 2] * scale) + m_posInChunk.y;
	}
}

void Block::setVerticesToChunkPosScaled(float scale, std::deque<int>& verts)
{
	for (size_t i = 0; i < verts.size(); i += 3)
	{
		verts[i] = (verts[i] * scale) + m_posInChunk.x;
		verts[i + 1] = (verts[i + 1] * scale) + m_yPos;
		verts[i + 2] = (verts[i + 2] * scale) + m_posInChunk.y;
	}
}

void Block::setVerticesToChunkPos(std::deque<GLfloat>& verts)
{
	for (size_t i = 0; i < verts.size(); i += 3)
	{
		verts[i]	 = verts[i]		+ m_posInChunk.x;
		verts[i + 1] = verts[i + 1]	+ m_yPos;
		verts[i + 2] = verts[i + 2] + m_posInChunk.y;
	}
}


