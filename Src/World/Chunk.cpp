#include "Chunk.h"

#include "../Renderer/BasicRenderer.h"
#include "../Renderer/WaterRenderer.h"
#include "../Util/Random.h"

#include "../Util/Timer.h"


#include "../Util/Timer.h"



Chunk::Chunk(int x, int z, int worldSeed, int(*heightFunction)(int x, int z, int seed))
{
	// for chunks positioned in negative space
	// you should pass in the most negative x and z coordiante
	m_chunkPos.x = x;
	m_chunkPos.y = z;

	int xEnd = x + g_chunkWidthX;
	int zEnd = z + g_chunkWidthZ;

	m_worldSeed = worldSeed;


	//DEBUG

	m_isHighlighted = false;

	//!DEBUG



	// heightMap includes adjacent heights from nearby chunks
	int heightMap[g_chunkWidthX + 2][g_chunkWidthZ + 2];

	for (int i = m_chunkPos.x; i < xEnd; ++i)
	{
		for (int j = m_chunkPos.y; j < zEnd; ++j)
		{
			glm::ivec2 localPos = translateGlobalToLocalCoords(glm::ivec2(i, j), g_chunkWidthX, g_chunkWidthZ);
			heightMap[localPos.x + 1][localPos.y + 1] = heightFunction(i, j, m_worldSeed);

			m_solidBlocks[localPos].height = heightMap[localPos.x + 1][localPos.y + 1];
			m_solidBlocks[localPos].stoneHeight = heightMap[localPos.x + 1][localPos.y + 1] - Random::get().getInt(3, 5);
		}
	}

	//fill the rest of the heightmap with adjacent chunk heights
	for (int i = -1; i <= int(g_chunkWidthX); ++i)
	{
		for (int j = -1; j <= int(g_chunkWidthZ); ++j)
		{
			if (i == -1 || j == -1 || i == g_chunkWidthX || j == g_chunkWidthZ)
			{
				heightMap[i + 1][j + 1] = heightFunction((m_chunkPos.x + i), (m_chunkPos.y + j), m_worldSeed);
			}
		}
	}

	// setup used block positions
	for (auto& e : m_solidBlocks)
	{
		e.second.setup();
	}


	// add all visible blocks to m_*blocks and initialize their faces
	for (int localX = 0; localX < g_chunkWidthX; ++localX)
	{
		for (int localZ = 0; localZ < g_chunkWidthZ; ++localZ)
		{
			glm::ivec2 currPos = { localX, localZ };
			glm::ivec2 globalPos = translateLocalToGlobalCoords(currPos, m_chunkPos);

			// add water blocks if height is at a certain threshhold
			if (heightMap[currPos.x + 1][currPos.y + 1] < g_waterLevel)
			{
				m_waterBlocks.push_back(std::make_unique<WaterBlock>(WaterBlock(currPos.x, g_waterLevel, currPos.y)));
			}


			bool blockAdded = false;
			for (int i = heightMap[currPos.x + 1][currPos.y + 1]; i >= g_minChunkHeight; --i)
			{
				// i is the current y-coordinate
				// every hightmap index must be increased by 1, because the heightmaps dimensions, relative to the pos, range from -1 to 17
				blockAdded = false;

				if (i == heightMap[currPos.x + 1][currPos.y + 1])
				{
					m_solidBlocks[currPos].addBlock(glm::vec3(currPos.x, heightMap[currPos.x + 1][currPos.y + 1], currPos.y), FACES_TO_DISPLAY::TOP);
					blockAdded = true;
				}
				else
				{
					// add other visible blocks in y-column
					if (heightMap[currPos.x + 1][currPos.y + DIRECTION_BACK.z + 1] < i)
					{
						m_solidBlocks[currPos].addBlock(glm::vec3(currPos.x, i, currPos.y), 0);
						blockAdded = true;
					}
					if (heightMap[currPos.x + 1][currPos.y + DIRECTION_FRONT.z + 1] < i)
					{
						m_solidBlocks[currPos].addBlock(glm::vec3(currPos.x, i, currPos.y), 0);
						blockAdded = true;
					}
					if (heightMap[currPos.x + DIRECTION_LEFT.x + 1][currPos.y + 1] < i)
					{
						m_solidBlocks[currPos].addBlock(glm::vec3(currPos.x, i, currPos.y), 0);
						blockAdded = true;
					}
					if (heightMap[currPos.x + DIRECTION_RIGHT.x + 1][currPos.y + 1] < i)
					{
						m_solidBlocks[currPos].addBlock(glm::vec3(currPos.x, i, currPos.y), 0);
						blockAdded = true;
					}
				}

				// when no new block was added -> break loop
				if (!blockAdded)
					break;

				// set up faces of newly added block
				if (heightMap[currPos.x + 1][currPos.y + DIRECTION_BACK.z + 1] < i)
				{
					m_solidBlocks[currPos].blocks.back()->setFaceVisible(FACES_TO_DISPLAY::BACK);
				}
				if (heightMap[currPos.x + 1][currPos.y + DIRECTION_FRONT.z + 1] < i)
				{
					m_solidBlocks[currPos].blocks.back()->setFaceVisible(FACES_TO_DISPLAY::FRONT);
				}
				if (heightMap[currPos.x + DIRECTION_LEFT.x + 1][currPos.y + 1] < i)
				{
					m_solidBlocks[currPos].blocks.back()->setFaceVisible(FACES_TO_DISPLAY::LEFT);
				}
				if (heightMap[currPos.x + DIRECTION_RIGHT.x + 1][currPos.y + 1] < i)
				{
					m_solidBlocks[currPos].blocks.back()->setFaceVisible(FACES_TO_DISPLAY::RIGHT);
				}
			}
		}
	}

	
	loadSolidMeshToRenderContext();
	loadWaterMeshToRenderContext();
	

	// chunk meshes are send to the GPU in World.cpp
}

Chunk::~Chunk()
{
	m_renderContextSolid.deleteContextData(true);

	m_solidBlocks.clear();
	m_waterBlocks.clear();
}

void Chunk::renderChunk(glm::mat4& mvp, float totalTime)
{
	if (m_renderContextSolid.numOfIndices > 0)
	{
		BasicRenderer::get().draw(m_renderContextSolid, mvp, m_chunkPos, true, m_isHighlighted);
	}

	if (m_renderContextWater.numOfIndices > 0)
	{
		WaterRenderer::get().draw(m_renderContextWater, mvp, m_chunkPos, totalTime, true);
	}
}

void Chunk::reloadMesh()
{
	loadSolidMeshToRenderContext();
}

void Chunk::resetRenderContext(bool deleteVao)
{
	m_renderContextSolid.deleteContextData(deleteVao);
}

void Chunk::sendRenderContextSolidToGPU()
{
	sendDataToGPU(&m_renderContextSolid);
}

void Chunk::sendRenderContextWaterToGPU()
{
	sendDataToGPU(&m_renderContextWater);
}

// returns chunk positions of adjacent chunks if they got changed in the process of destrying a block
std::vector<glm::ivec2> Chunk::deleteBlock(glm::ivec3 globalBlockPos, size_t index, std::vector<Chunk*>* adjacentChunks)
{
	try
	{
		// delete pointer to block
		glm::ivec2 localXZ = translateGlobalToLocalCoords(glm::ivec2(globalBlockPos.x, globalBlockPos.z), g_chunkWidthX, g_chunkWidthZ);
		m_solidBlocks[localXZ].blocks.erase(m_solidBlocks[localXZ].blocks.begin() + index);

		std::vector<glm::ivec2> chunksNeedingUpdate = loadSurroundingBlocks(globalBlockPos, adjacentChunks);

		return chunksNeedingUpdate;
	}
	catch (std::exception e)
	{
		std::cout << e.what() << "\n";
		return std::vector<glm::ivec2>();
	}
}

std::vector<glm::ivec2> Chunk::loadSurroundingBlocks(glm::vec3 globalCenterPos, std::vector<Chunk*>* adjacentChunks)
{
	glm::ivec2 localPos = translateGlobalToLocalCoords(glm::ivec2(globalCenterPos.x, globalCenterPos.z), g_chunkWidthX, g_chunkWidthZ);
	glm::ivec2 chunkPos = translateGlobalPosToChunkPos(globalCenterPos, (int)(g_chunkWidthX), (int)(g_chunkWidthZ));

	ChunkPiece* left, * right, * back, * front;
	bool leftInChunk, rightInChunk, backInChunk, frontInChunk;
	std::vector<glm::ivec2> chunksPositionsUpdated;

	// get adjacent pieces
	if (localPos.x == 0)
	{
		left = adjacentChunks->at(0)->getBlocksFromChunk(glm::ivec2(globalCenterPos.x - 1, globalCenterPos.z));
		leftInChunk = false;
	}
	else
	{
		left = &m_solidBlocks[glm::ivec2(localPos.x - 1, localPos.y)];
		leftInChunk = true;
	}

	if (localPos.x == g_chunkWidthX - 1)
	{
		right = adjacentChunks->at(1)->getBlocksFromChunk(glm::ivec2(globalCenterPos.x + 1, globalCenterPos.z));
		rightInChunk = false;
	}
	else
	{
		right = &m_solidBlocks[glm::ivec2(localPos.x + 1, localPos.y)];
		rightInChunk = true;
	}

	if (localPos.y == 0)
	{
		back = adjacentChunks->at(2)->getBlocksFromChunk(glm::ivec2(globalCenterPos.x, globalCenterPos.z - 1));
		backInChunk = false;
	}
	else
	{
		back = &m_solidBlocks[glm::ivec2(localPos.x, localPos.y - 1)];
		backInChunk = true;
	}

	if (localPos.y == g_chunkWidthZ - 1)
	{
		front = adjacentChunks->at(3)->getBlocksFromChunk(glm::ivec2(globalCenterPos.x, globalCenterPos.z + 1));
		frontInChunk = false;
	}
	else
	{
		front = &m_solidBlocks[glm::ivec2(localPos.x, localPos.y + 1)];
		frontInChunk = true;
	}

	// update blocks in pieces
	// bottom
	if (m_solidBlocks[localPos].blocks.empty())
	{
		if (globalCenterPos.y > g_minChunkHeight)
			m_solidBlocks[localPos].addBlock(glm::vec3(globalCenterPos.x, globalCenterPos.y - 1, globalCenterPos.z), FACES_TO_DISPLAY::TOP);
	}
	else
	{
		int index = m_solidBlocks[localPos].searchForBlock((int)globalCenterPos.y - 1);
		if (index == -1)
		{
			if (globalCenterPos.y > g_minChunkHeight)
			{
				m_solidBlocks[localPos].addBlock(glm::vec3(globalCenterPos.x, globalCenterPos.y - 1, globalCenterPos.z), FACES_TO_DISPLAY::TOP);
			}
		}
		else
		{
			m_solidBlocks[localPos].blocks[index].get()->setFaceVisible(FACES_TO_DISPLAY::TOP);
		}
	}
	// top
	if (m_solidBlocks[localPos].blocks.empty())
	{
		m_solidBlocks[localPos].addBlock(glm::vec3(globalCenterPos.x, globalCenterPos.y + 1, globalCenterPos.z), FACES_TO_DISPLAY::BOTTOM);
	}
	else
	{
		int index = m_solidBlocks[localPos].searchForBlock((int)globalCenterPos.y + 1);
		if (index == -1)
		{
			m_solidBlocks[localPos].addBlock(glm::vec3(globalCenterPos.x, globalCenterPos.y + 1, globalCenterPos.z), FACES_TO_DISPLAY::BOTTOM);
		}
		else
		{
			m_solidBlocks[localPos].blocks[index].get()->setFaceVisible(FACES_TO_DISPLAY::BOTTOM);
		}
	}

	// left
	{
		glm::ivec2 update = updateNearbyBlock(left, glm::vec3(globalCenterPos.x - 1, globalCenterPos.y, globalCenterPos.z), FACES_TO_DISPLAY::RIGHT);
		if (!leftInChunk)
			chunksPositionsUpdated.push_back(adjacentChunks->at(0)->getWorldPosXZ());
	}
	// right
	{
		glm::ivec2 update = updateNearbyBlock(right, glm::vec3(globalCenterPos.x + 1, globalCenterPos.y, globalCenterPos.z), FACES_TO_DISPLAY::LEFT);
		if (!rightInChunk)
			chunksPositionsUpdated.push_back(adjacentChunks->at(1)->getWorldPosXZ());
	}
	// back
	{
		glm::ivec2 update = updateNearbyBlock(back, glm::vec3(globalCenterPos.x, globalCenterPos.y, globalCenterPos.z - 1), FACES_TO_DISPLAY::FRONT);
		if (!backInChunk)
			chunksPositionsUpdated.push_back(adjacentChunks->at(2)->getWorldPosXZ());
	}
	// front
	{
		glm::ivec2 update = updateNearbyBlock(front, glm::vec3(globalCenterPos.x, globalCenterPos.y, globalCenterPos.z + 1), FACES_TO_DISPLAY::BACK);
		if (!frontInChunk)
			chunksPositionsUpdated.push_back(adjacentChunks->at(3)->getWorldPosXZ());
	}

	return chunksPositionsUpdated;
}

// Returns the chunk position of the updated block
// Returns (-1, -1) if nothing was changed
glm::ivec2 Chunk::updateNearbyBlock(ChunkPiece* cPiece, glm::vec3 pos, unsigned char facesToDisplay)
{
	int index = cPiece->searchForBlock((int)pos.y);
	if (index == -1)
	{
		if (pos.y < cPiece->height)
		{
			glm::ivec2 localPos = translateGlobalToLocalCoords(glm::ivec2(pos.x, pos.z), g_chunkWidthX, g_chunkWidthZ);
			cPiece->addBlock(glm::vec3(localPos.x, pos.y, localPos.y), facesToDisplay);
			return glm::ivec2(translateGlobalPosToChunkPos(pos, (int)g_chunkWidthX, (int)g_chunkWidthZ));
		}
	}
	else
	{
		cPiece->blocks[index].get()->setFaceVisible(facesToDisplay);
		return glm::ivec2(translateGlobalPosToChunkPos(pos, (int)g_chunkWidthX, (int)g_chunkWidthZ));
	}

	return glm::ivec2(-1, -1);
}


glm::ivec2 Chunk::getWorldPosXZ() const
{
	return { m_chunkPos.x, m_chunkPos.y };
}


void Chunk::fillSpaceY(int x, int z, int height)
{
	int moreDirt = Random::get().getInt(0, 2);

	glm::ivec2 localPos = translateGlobalToLocalCoords(glm::ivec2(x, z), g_chunkWidthX, g_chunkWidthZ);

	for (int i = 0; i <= height; ++i)
	{
		if (i == height)
		{
			m_solidBlocks[localPos].blocks.push_back(std::make_unique<Block>(Block(x, i, z, BLOCK_ID::GRASS)));
		}
		else if (i >= (height - 2 - moreDirt))
		{
			m_solidBlocks[localPos].blocks.push_back(std::make_unique<Block>(Block(x, i, z, BLOCK_ID::DIRT)));
		}
		else
		{
			m_solidBlocks[localPos].blocks.push_back(std::make_unique<Block>(Block(x, i, z, BLOCK_ID::STONE)));
		}
	}
}

ChunkPiece* Chunk::getBlocksFromChunk(glm::ivec2 pos)
{
	if (pos.x >= g_chunkWidthX || pos.x < 0 || pos.y >= g_chunkWidthZ || pos.y < 0)
	{
		pos = translateGlobalToLocalCoords(pos, g_chunkWidthX, g_chunkWidthZ);
	}
	return &m_solidBlocks[pos];
}

void Chunk::setHighlighting(bool highlight)
{
	m_isHighlighted = highlight;
}

void Chunk::fillSpaceWith(int xStart, int zStart, int xEnd, int zEnd, int height, BLOCK_ID typeID)
{
	if (xStart > xEnd)
	{
		int temp = xStart;
		xStart = xEnd;
		xEnd = temp;
	}
	if (zStart > zEnd)
	{
		int temp = zStart;
		zStart = zEnd;
		zEnd = temp;
	}

	for (int x = xStart; x < xEnd; ++x)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			m_solidBlocks[glm::ivec2(x, z)].blocks.push_back(std::make_unique<Block>(Block(x, height, z, typeID)));
		}
	}
}

bool Chunk::posInChunk(int x, int z) const
{
	int xMax = m_chunkPos.x + g_chunkWidthX;
	int zMax = m_chunkPos.y + g_chunkWidthZ;

	if (x < m_chunkPos.x || x >= xMax ||
		z < m_chunkPos.y || z >= zMax)
		return false;

	return true;
}

bool Chunk::posOnChunkEdge(int x, int z) const
{
	if ((x / g_chunkWidthX) == 0 || (x % g_chunkWidthX) == 15 ||
		(z / g_chunkWidthZ) == 0 || (z % g_chunkWidthZ) == 15)
		return true;

	return false;
}

void Chunk::loadSolidMeshToRenderContext()
{
	std::deque<BlockMeshData> allMeshData;
	std::deque<int> numOfFacesPerBlock;

	for (auto& kV : m_solidBlocks)
	{
		for (auto& b : kV.second.blocks)
		{
			std::vector<BlockMeshData> blockMeshData = b->getVisibleMesh();

			for (auto& bFace : blockMeshData)
			{
				allMeshData.push_back(bFace);
			}

			// not visible blocks shouldn't be given to the RenderContextBuilder
			// it doesn't expect a zero. 
			// !!!---- Passing a zero will result in wrongly displayed blocks ----!!!
			if (b->getNumOfFacesVisible() != 0)
				numOfFacesPerBlock.push_back(b->getNumOfFacesVisible());
		}
	}

	if (allMeshData.size() > 0 && numOfFacesPerBlock.size() > 0)
	{
		parseBlockDataToRenderContext(&m_renderContextSolid, allMeshData, numOfFacesPerBlock);
	}

}

void Chunk::loadWaterMeshToRenderContext()
{
	std::deque<BlockMeshData> allMeshData;
	std::deque<int> numOfFacesPerBlock;

	for (auto& wB : m_waterBlocks)
	{
		std::vector<BlockMeshData> blockMeshData = wB->getVisibleMesh();

		for (auto& bFace : blockMeshData)
		{
			allMeshData.push_back(bFace);
		}

		// not visible blocks shouldn't be given to the RenderContextBuilder
		// it doesn't expect a zero. 
		// !!!---- Passing a zero will result in wrongly displayed blocks ----!!!
		if (wB->getNumOfFacesVisible() != 0)
			numOfFacesPerBlock.push_back(wB->getNumOfFacesVisible());
	}

	if (allMeshData.size() > 0 && numOfFacesPerBlock.size() > 0)
	{
		parseBlockDataToRenderContext(&m_renderContextWater, allMeshData, numOfFacesPerBlock);
	}
}
