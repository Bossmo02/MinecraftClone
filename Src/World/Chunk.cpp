#include "Chunk.h"

#include "../Renderer/BasicRenderer.h"
#include "../Util/Random.h"



#include "../Util/Timer.h"



Chunk::Chunk(int x, int z, int* worldSeed, int(*heightFunction)(int x, int z, int seed))
{
	// for chunks positioned in negative space
	// you should pass in the most negative x and z coordiante
	m_chunkPos.x = x;
	m_chunkPos.y = z;

	int xEnd = x + g_chunkWidthX;
	int zEnd = z + g_chunkWidthZ;

	m_worldSeed = worldSeed;

	// heightMap includes adjacent heights from nearby chunks
	int heightMap[g_chunkWidthX + 2][g_chunkWidthZ + 2];

	for (int i = m_chunkPos.x; i < xEnd; ++i)
	{
		for (int j = m_chunkPos.y; j < zEnd; ++j)
		{			
			glm::ivec2 localPos = translateGlobalToLocalCoords(glm::ivec2(i, j));
			heightMap[localPos.x+1][localPos.y+1] = heightFunction((float)i, (float)j, *m_worldSeed) * 4 + g_baseChunkHeight;
		}
	}

	//fill the rest of the heightmap with adjacent chunk heights
	for (int i = -1; i <= int(g_chunkWidthX); ++i)
	{
		for (int j = -1; j <= int(g_chunkWidthZ); ++j)
		{
			if (i == -1 || j == -1 || i == g_chunkWidthX || j == g_chunkWidthZ)
			{
				heightMap[i+1][j+1] = heightFunction((float)(m_chunkPos.x + i), (float)(m_chunkPos.y + j), *m_worldSeed) * 4 + g_baseChunkHeight;
			}
		}
	}
	



	// add all visible blocks to m_*blocks and initialize their faces
	for (int localX = 0; localX < g_chunkWidthX; ++localX)
	{
		for (int localZ = 0; localZ < g_chunkWidthZ; ++localZ)
		{
			glm::ivec2 currPos = { localX, localZ };
			glm::ivec2 globalPos = translateLocalToGlobalCoords(currPos);

			// add water blocks if height is at a certain threshhold
			if (heightMap[currPos.x][currPos.y] <= g_baseChunkHeight)
			{
				
			}
			

			bool blockAdded = false;
			for (int i = heightMap[currPos.x+1][currPos.y+1]; i >= 0; --i)
			{
				// i is the current y-coordinate
				// every hightmap index must be increased by 1, because the heightmaps dimensions, relative to the pos, range from -1 to 17
				blockAdded = false;

				if (i == heightMap[currPos.x+1][currPos.y+1])
				{
					m_solidBlocks[currPos].push_back(new Block(globalPos.x, heightMap[currPos.x+1][currPos.y+1], globalPos.y, BLOCK_ID::GRASS));
					m_solidBlocks[currPos].back()->setFaceVisible(FACES_TO_DISPLAY::TOP);
					blockAdded = true;
				}
				else
				{
					// add other visible blocks in y-column
					if (heightMap[currPos.x + 1][currPos.y + DIRECTION_BACK.z + 1] < i)
					{
						m_solidBlocks[currPos].push_back(new Block(globalPos.x, i, globalPos.y, BLOCK_ID::DIRT));
						blockAdded = true;
					}
					if (heightMap[currPos.x + 1][currPos.y + DIRECTION_FRONT.z + 1] < i)
					{
						m_solidBlocks[currPos].push_back(new Block(globalPos.x, i, globalPos.y, BLOCK_ID::DIRT));
						blockAdded = true;
					}
					if (heightMap[currPos.x + DIRECTION_LEFT.x + 1][currPos.y + 1] < i)
					{
						m_solidBlocks[currPos].push_back(new Block(globalPos.x, i, globalPos.y, BLOCK_ID::DIRT));
						blockAdded = true;
					}
					if (heightMap[currPos.x + DIRECTION_RIGHT.x + 1][currPos.y + 1] < i)
					{
						m_solidBlocks[currPos].push_back(new Block(globalPos.x, i, globalPos.y, BLOCK_ID::DIRT));
						blockAdded = true;
					}
				}

				// when no new block was added -> break loop
				if (!blockAdded)
					break;

				// set up faces of newly added block
				if (heightMap[currPos.x + 1][currPos.y + DIRECTION_BACK.z + 1] < i)
				{
					m_solidBlocks[currPos].back()->setFaceVisible(FACES_TO_DISPLAY::BACK);
				}
				if (heightMap[currPos.x + 1][currPos.y + DIRECTION_FRONT.z + 1] < i)
				{
					m_solidBlocks[currPos].back()->setFaceVisible(FACES_TO_DISPLAY::FRONT);
				}
				if (heightMap[currPos.x + DIRECTION_LEFT.x + 1][currPos.y + 1] < i)
				{
					m_solidBlocks[currPos].back()->setFaceVisible(FACES_TO_DISPLAY::LEFT);
				}
				if (heightMap[currPos.x + DIRECTION_RIGHT.x + 1][currPos.y + 1] < i)
				{
					m_solidBlocks[currPos].back()->setFaceVisible(FACES_TO_DISPLAY::RIGHT);
				}
			}


			//if (!m_solidBlocks[currPos].empty())
			//{
			//	int yPos = m_solidBlocks[currPos].back()->getWorldPos().y;

			//	m_solidBlocks[currPos].back()->setFaceVisible(FACES_TO_DISPLAY::TOP);

			//	for (int i = yPos; i >= 0; --i)
			//	{
			//		// i is the current y-coordinate
			//		// every hightmap index must be increased by 1, because the heightmaps dimensions, relative to the pos, range from -1 to 17

			//		if (m_solidBlocks[currPos][i]->getBlockType() != BLOCK_ID::WATER)
			//		{

			//			if (heightMap[currPos.x + 1][currPos.y + DIRECTION_BACK.z + 1] < i)
			//			{
			//				m_solidBlocks[currPos][i]->setFaceVisible(FACES_TO_DISPLAY::BACK);
			//			}
			//			if (heightMap[currPos.x + 1][currPos.y + DIRECTION_FRONT.z + 1] < i)
			//			{
			//				m_solidBlocks[currPos][i]->setFaceVisible(FACES_TO_DISPLAY::FRONT);
			//			}
			//			if (heightMap[currPos.x + DIRECTION_LEFT.x + 1][currPos.y + 1] < i)
			//			{
			//				m_solidBlocks[currPos][i]->setFaceVisible(FACES_TO_DISPLAY::LEFT);
			//			}
			//			if (heightMap[currPos.x + DIRECTION_RIGHT.x + 1][currPos.y + 1] < i)
			//			{
			//				m_solidBlocks[currPos][i]->setFaceVisible(FACES_TO_DISPLAY::RIGHT);
			//			}

			//			// check for block meshes under water
			//			if (i < yPos)
			//			{
			//				if (m_solidBlocks[currPos][i + 1]->getBlockType() == BLOCK_ID::WATER && m_solidBlocks[currPos][i]->getBlockType() != BLOCK_ID::WATER)
			//				{
			//					m_solidBlocks[currPos][i]->setFaceVisible(FACES_TO_DISPLAY::TOP);
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}

	loadMeshToRenderContext();
	// chunk meshes are send to the GPU in World.cpp
}

Chunk::~Chunk()
{
	m_renderContext.deleteContextData(true);

	for (auto& kV : m_solidBlocks)
	{
		for (auto b : kV.second)
		{
			delete b;
		}
	}

	m_solidBlocks.clear();
}


void Chunk::reloadMesh()
{
	loadMeshToRenderContext();
}

void Chunk::sendRenderContextToGPU()
{
	RenderContextBuilder::get().sendDataToGPU(&m_renderContext);
}

void Chunk::renderChunk(glm::mat4& mvp)
{
	if (m_renderContext.numOfIndices > 0)
	{
		BasicRenderer::get().draw(m_renderContext, mvp, true);
	}
}

glm::ivec2 Chunk::getWorldPosXZ() const
{
	return { m_chunkPos.x, m_chunkPos.y };
}


void Chunk::fillSpaceY(int x, int z, int height)
{
	int moreDirt = Random::get().getInt(0, 2);

	glm::ivec2 localPos = translateGlobalToLocalCoords(glm::ivec2(x, z));

	m_solidBlocks[localPos].reserve(height);

	for (int i = 0; i <= height; ++i)
	{
		if (i == height)
		{
			m_solidBlocks[localPos].push_back(new Block(x, i, z, BLOCK_ID::GRASS));
		}
		else if (i >= (height - 2 - moreDirt))
		{
			m_solidBlocks[localPos].push_back(new Block(x, i, z, BLOCK_ID::DIRT));
		}
		else
		{
			m_solidBlocks[localPos].push_back(new Block(x, i, z, BLOCK_ID::STONE));
		}
	}
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
			m_solidBlocks[glm::ivec2(x, z)].push_back(new Block(x, height, z, typeID));
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

glm::ivec2 Chunk::translateGlobalToLocalCoords(glm::ivec2 globalCoords)
{
	return { globalCoords.x % g_chunkWidthX, globalCoords.y % g_chunkWidthZ };
}

glm::ivec2 Chunk::translateLocalToGlobalCoords(glm::ivec2 localCoords)
{
	return { m_chunkPos.x + localCoords.x, m_chunkPos.y + localCoords.y };
}


void Chunk::loadMeshToRenderContext()
{
	std::vector<BlockMeshData> allMeshData;
	std::vector<int> numOfFacesPerBlock;

	for (auto& kV : m_solidBlocks)
	{
		for (auto b : kV.second)
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

	if(allMeshData.size() > 0 && numOfFacesPerBlock.size() > 0)
		RenderContextBuilder::get().parseBlockDataToRenderContext(&m_renderContext, allMeshData, numOfFacesPerBlock);
}
