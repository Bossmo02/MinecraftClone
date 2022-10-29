#include "World.h"




//---------------
// Functions used for multithreading

Chunk* loadChunkAsync(int x, int z, int seed, int(*heightFunction)(int x, int z, int seed))
{
	Chunk* c = new Chunk(x, z, seed, heightFunction);

	return c;
}
//-----------------------------


//-------------------------------------
// standart heightfunction

float lowest = 1;

int defaultHeightFunction(int x, int z, int seed)
{
	FastNoiseLite n1, n2;
	n1.SetSeed(seed);
	n2.SetSeed(seed + 1);

	float n2Multi = 1.f;
	float mountainWidth = 0.5f;		// idk what these values are doing
	float mountainHeight = 8.f;		// but it looks nice

	n1.SetFrequency(0.005f);
	n1.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	n1.SetFractalLacunarity(mountainWidth);
	n1.SetFractalWeightedStrength(mountainHeight);
	n1.SetFractalGain(0.5f);


	n2.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	n2.SetFractalLacunarity(0.8f);
	n2.SetFractalWeightedStrength(10.f);
	n2.SetFractalGain(4.f);


	float temp1 = ((n1.GetNoise((float)x, (float)z, n2.GetNoise((float)x, (float)z) * n2Multi)) + 0.5f) / 1.5f;
	float temp2 = 0;

	if (temp1 < lowest)
		lowest = temp1;

	temp2 = temp1 * 20;

	return static_cast<int>(temp2);
}
//-----------------------------------------------------------------------


World::World(int seed)
	: m_texture(GL_TEXTURE_2D, GL_TEXTURE0, "Res/spritesheet_tiles.png")
{
	m_noise.SetSeed(seed);
	m_seed = seed;

	m_heightFunction = defaultHeightFunction;

	m_oldChunkPos = glm::ivec2(0, 0);
}

World::World(int seed, int(*heightFunction)(int x, int z, int seed))
	: m_texture(GL_TEXTURE_2D, GL_TEXTURE0, "Res/spritesheet_tiles.png")
{
	m_noise.SetSeed(seed);
	m_seed = seed;

	m_heightFunction = heightFunction;

	m_oldChunkPos = glm::ivec2(0, 0);
}

World::~World()
{
	for (auto& c : m_chunks)
		delete c.second;

	m_chunks.clear();
	m_texture.deleteTexture();
}

void World::update(glm::vec3 camPos)
{
	updateChunksAroundCam(camPos);

	updateFutures();

	updateChunkdrawingOrder(camPos);
}

void World::updateChunksAroundCam(glm::vec3 camPos)
{
	if (m_chunkFutures.size() < m_maxChunksLoadingCount)
	{
		glm::ivec2 currChunk = translateGlobalPosToChunkIndex(camPos, (int)(g_chunkWidthX), (int)(g_chunkWidthZ));

		int timesChunkAdded = 0;

		for (int i = currChunk.x - m_maxLoadChunkDistance; i <= currChunk.x + m_maxLoadChunkDistance; ++i)
		{
			for (int j = currChunk.y - m_maxLoadChunkDistance; j <= currChunk.y + m_maxLoadChunkDistance; ++j)
			{
				bool loading = false;
				glm::ivec2 currChunk = { i * int(g_chunkWidthX), j * int(g_chunkWidthZ) };

				// check if chunk is loading
				for (auto& f : m_chunkFutures)
				{
					if (f.first == currChunk)
					{
						loading = true;
						break;
					}
				}

				// check if chunk is loaded
				if (!m_chunks.contains(currChunk) && !loading && timesChunkAdded < m_maxChunkAddPerIter)
				{
					m_chunkFutures.push_back(std::pair<glm::ivec2, std::future<Chunk*>>(currChunk, std::async(std::launch::async, loadChunkAsync, currChunk.x, currChunk.y, m_seed, m_heightFunction)));
					timesChunkAdded++;
				}
			}
		}

		// did the player move into another chunk?
		if (m_oldChunkPos.x != currChunk.x || m_oldChunkPos.y != currChunk.y)
		{
			deleteFurthestChunks(camPos);

			m_oldChunkPos.x = currChunk.x;
			m_oldChunkPos.y = currChunk.y;
		}
	}
}


// ------------------------------ Rendering stuff ---------------------------

bool compareChunkDistances(const std::pair<float, Chunk*>& c1, const std::pair<float, Chunk*>& c2)
{
	return c1.first > c2.first;
}

float dist(const glm::vec3& p1, const glm::vec3 p2)
{
	return std::sqrt(std::pow(p2.x - p1.x, 2) +
		std::pow(p2.y - p1.y, 2) +
		std::pow(p2.z - p1.z, 2));
}

void World::updateChunkdrawingOrder(glm::vec3 camPos)
{
	// refresh pointers
	m_sortedChunks.clear();

	for (auto& c : m_chunks)
	{
		// add and calculate distance for each loaded chunk
		glm::ivec2 chunkPos = c.second->getWorldPosXZ();
		m_sortedChunks.push_back(std::pair<float, Chunk*>(dist(camPos, glm::vec3(static_cast<float>(chunkPos.x) + static_cast<float>(g_chunkWidthX) / 2.f, 0, static_cast<float>(chunkPos.y) + static_cast<float>(g_chunkWidthZ) / 2.f)), c.second));
	}

	std::sort(m_sortedChunks.begin(), m_sortedChunks.end(), compareChunkDistances);
}


void World::render(glm::mat4& mvp, float totalTime)
{
	for (size_t i = 0; i < m_sortedChunks.size(); i++)
	{
		m_sortedChunks[i].second->renderChunk(mvp, totalTime);
	}
}
// !------------------------------------------------------------------------------------------

void World::updateFutures()
{
	int count = 0;

	for (size_t i = 0; i < m_chunkFutures.size(); ++i)
	{
		if (m_chunkFutures[i].second.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
		{
			Chunk* c = m_chunkFutures[i].second.get();
			c->sendRenderContextSolidToGPU();
			c->sendRenderContextWaterToGPU();
			m_chunks[c->getWorldPosXZ()] = c;
			m_chunkFutures.erase(m_chunkFutures.begin() + i);
			count++;
		}

		if (count >= m_maxChunkToGPULoads)
			break;
	}
}


void World::destroyBlock(Camera* cam)
{
	// needs tweaking 
	// incrementing i with higher values may cause problems with finding the right block
	for (float i = 0; i < m_maxBlockReach; i += 0.09f)
	{
		glm::vec3 pos = cam->castRay(i);
		glm::ivec2 chunkPos = translateGlobalPosToChunkPos(pos, (int)(g_chunkWidthX), (int)(g_chunkWidthZ));

		if (m_chunks.contains(chunkPos))
		{
			// needs offset in the negative to work
			if (pos.x < 0)
				pos.x--;
			if (pos.z < 0)
				pos.z--;

			ChunkPiece* chunkPiece = m_chunks[chunkPos]->getBlocksFromChunk(glm::ivec2(pos.x, pos.z));

			for (size_t j = 0; j < chunkPiece->blocks.size(); ++j)
			{
				glm::ivec3 blockPos = chunkPiece->blocks[j]->getLocalPos();
				if (pos.y > blockPos.y && pos.y < blockPos.y + 1.f)
				{
					// get adjacent chunks
					std::vector<Chunk*> adjacentChunks;
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x - 32, chunkPos.y)]);
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x + 32, chunkPos.y)]);
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x, chunkPos.y - 32)]);
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x, chunkPos.y + 32)]);

					std::vector<glm::ivec2> chunksToUpdate = m_chunks[chunkPos]->deleteBlock(blockPos, j, &adjacentChunks);
					chunksToUpdate.push_back(chunkPos);

					std::cout << "Deleted Block at:\tX: " << blockPos.x << "\tY: " << blockPos.y << "\tZ: " << blockPos.z << "\n";


					for (auto& c : chunksToUpdate)
					{
						m_chunks[c]->resetRenderContext(false);
						m_chunks[c]->reloadMesh();
						m_chunks[c]->sendRenderContextSolidToGPU();
					}
					return;
				}
			}
		}
	}

}

void World::deleteFurthestChunks(glm::vec3 camPos)
{
	std::deque<glm::ivec2> positionsToDelete;

	for (auto& c : m_chunks)
	{
		glm::ivec2 camChunkPos = translateGlobalPosToChunkIndex(glm::vec3(camPos.x, 0, camPos.z), g_chunkWidthX, g_chunkWidthZ);
		glm::ivec2 chunkIndex = translateGlobalPosToChunkIndex(glm::vec3(c.first.x, 0, c.first.y), g_chunkWidthX, g_chunkWidthZ);

		int xDiff = std::abs(camChunkPos.x - chunkIndex.x);
		int zDiff = std::abs(camChunkPos.y - chunkIndex.y);

		if (xDiff >= m_distanceToDelete || zDiff >= m_distanceToDelete)
		{
#ifdef _DEBUG
			std::cout << "Deleted Chunk at: " << c.first.x << ", " << c.first.y << "\n";
#endif // _DEBUG

			positionsToDelete.push_back(c.first);
		}
	}

	for (size_t i = 0; i < positionsToDelete.size(); ++i)
	{
		delete m_chunks[positionsToDelete[i]];
		m_chunks.erase(positionsToDelete[i]);
	}
}

void World::resetHighlighting()
{
	for (auto& c : m_chunks)
	{
		c.second->setHighlighting(false);
	}
}

void World::reloadAllChunks()
{
	for (auto& c : m_chunks)
	{
		delete c.second;
	}
	m_chunks.clear();
}

std::array<glm::vec3, 5> World::getHeightsNearPos(glm::vec3 pos)
{
	glm::ivec3 posInt = glm::ivec3(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z));
	
	std::array<glm::vec3, 5> heights =
	{
		glm::vec3(posInt.x	  , m_heightFunction(posInt.x    ,  posInt.z - 1, m_seed), posInt.z - 1),

		glm::vec3(posInt.x - 1, m_heightFunction(posInt.x - 1,  posInt.z    , m_seed), posInt.z ),
		glm::vec3(posInt.x	  , m_heightFunction(posInt.x    ,  posInt.z    , m_seed), posInt.z ),
		glm::vec3(posInt.x + 1, m_heightFunction(posInt.x + 1,  posInt.z    , m_seed), posInt.z ),
		
		glm::vec3(posInt.x	  , m_heightFunction(posInt.x    ,  posInt.z + 1, m_seed), posInt.z + 1)
	};

	return heights;
}

std::array<glm::vec3, 10> World::getBlockPositionsAroundPlayerPos(glm::vec3 pos)
{
	// needs offset in the negative to work
	if (pos.x < 0)
		pos.x--;
	if (pos.z < 0)
		pos.z--;

	glm::ivec3 roundedPos = glm::ivec3((int)pos.x, (int)pos.y, (int)pos.z);
	glm::ivec2 chunkPos = translateGlobalPosToChunkPos(glm::vec3(roundedPos.x, roundedPos.y, roundedPos.z), g_chunkWidthX, g_chunkWidthZ);
	glm::ivec2 localCoords = translateGlobalToLocalCoords(glm::ivec2(roundedPos.x, roundedPos.z), g_chunkWidthX, g_chunkWidthZ);

	// 0: center
	// 1: x-1
	// 2: z-1
	// 3: x+1
	// 4: z+1
	std::array<ChunkPiece*, 5> chunkPieces;

	// all adjacent chunks loaded?
	if (m_chunks.size() < 5 || (m_chunks.find(chunkPos) == m_chunks.end() ||
								m_chunks.find(glm::ivec2(chunkPos.x - g_chunkWidthX, chunkPos.y)) == m_chunks.end() ||
								m_chunks.find(glm::ivec2(chunkPos.x, chunkPos.y - g_chunkWidthZ)) == m_chunks.end() ||
								m_chunks.find(glm::ivec2(chunkPos.x + g_chunkWidthX, chunkPos.y)) == m_chunks.end() ||
								m_chunks.find(glm::ivec2(chunkPos.x, chunkPos.y + g_chunkWidthZ)) == m_chunks.end() ))
	{
		std::array<glm::vec3, 10> nothing;
		for (size_t i = 0; i < nothing.size(); i++)
		{
			nothing[i] = BLOCK_NOTHING;
		}
		return nothing;
	}

	chunkPieces[0] = m_chunks[chunkPos]->getBlocksFromChunk(localCoords);

	// x-1
	if (localCoords.x - 1 < 0)
	{
		chunkPieces[1] = m_chunks[glm::ivec2(chunkPos.x - g_chunkWidthX, chunkPos.y)]->getBlocksFromChunk(glm::ivec2(g_chunkWidthX - 1, localCoords.y));
	}
	else
	{
		chunkPieces[1] = m_chunks[chunkPos]->getBlocksFromChunk(glm::ivec2(localCoords.x - 1, localCoords.y));
	}

	// z-1
	if (localCoords.y - 1 < 0)
	{
		chunkPieces[2] = m_chunks[glm::ivec2(chunkPos.x, chunkPos.y - g_chunkWidthZ)]->getBlocksFromChunk(glm::ivec2(localCoords.x, g_chunkWidthZ - 1));
	}
	else
	{
		chunkPieces[2] = m_chunks[chunkPos]->getBlocksFromChunk(glm::ivec2(localCoords.x, localCoords.y - 1));
	}

	// x+1
	if (localCoords.x + 1 >= g_chunkWidthX)
	{
		chunkPieces[3] = m_chunks[glm::ivec2(chunkPos.x + g_chunkWidthX, chunkPos.y)]->getBlocksFromChunk(glm::ivec2(0, localCoords.y));
	}
	else
	{
		chunkPieces[3] = m_chunks[chunkPos]->getBlocksFromChunk(glm::ivec2(localCoords.x + 1, localCoords.y));
	}

	// z+1
	if (localCoords.y + 1 >= g_chunkWidthZ)
	{
		chunkPieces[4] = m_chunks[glm::ivec2(chunkPos.x, chunkPos.y + g_chunkWidthZ)]->getBlocksFromChunk(glm::ivec2(localCoords.x, 0));
	}
	else
	{
		chunkPieces[4] = m_chunks[chunkPos]->getBlocksFromChunk(glm::ivec2(localCoords.x, localCoords.y + 1));
	}


	std::array<glm::vec3, 10> nearbyBlocks;

	// bottom
	int index = chunkPieces[0]->searchForBlock(pos.y - 1.75);
	nearbyBlocks[0] = index != -1 ? glm::vec3(roundedPos.x, pos.y - 1.75, roundedPos.z) : BLOCK_NOTHING;

	// top
	index = chunkPieces[0]->searchForBlock(roundedPos.y + 1);
	nearbyBlocks[1] = index != -1 ? glm::vec3(roundedPos.x, roundedPos.y + 1, roundedPos.z) : BLOCK_NOTHING;

	// x-1
	index = chunkPieces[1]->searchForBlock(roundedPos.y);
	nearbyBlocks[2] = index != -1 ? glm::vec3(roundedPos.x - 1, roundedPos.y	, roundedPos.z) : BLOCK_NOTHING;
	index = chunkPieces[1]->searchForBlock(roundedPos.y - 1);
	nearbyBlocks[3] = index != -1 ? glm::vec3(roundedPos.x - 1, roundedPos.y - 1, roundedPos.z) : BLOCK_NOTHING;

	// z-1
	index = chunkPieces[2]->searchForBlock(roundedPos.y);
	nearbyBlocks[4] = index != -1 ? glm::vec3(roundedPos.x, roundedPos.y, roundedPos.z - 1) : BLOCK_NOTHING;
	index = chunkPieces[2]->searchForBlock(roundedPos.y - 1);
	nearbyBlocks[5] = index != -1 ? glm::vec3(roundedPos.x, roundedPos.y - 1, roundedPos.z - 1) : BLOCK_NOTHING;
	
	// x+1
	index = chunkPieces[3]->searchForBlock(roundedPos.y);
	nearbyBlocks[6] = index != -1 ? glm::vec3(roundedPos.x + 1, roundedPos.y, roundedPos.z) : BLOCK_NOTHING;
	index = chunkPieces[3]->searchForBlock(roundedPos.y - 1);
	nearbyBlocks[7] = index != -1 ? glm::vec3(roundedPos.x + 1, roundedPos.y - 1, roundedPos.z) : BLOCK_NOTHING;

	// z+1
	index = chunkPieces[4]->searchForBlock(roundedPos.y);
	nearbyBlocks[8] = index != -1 ? glm::vec3(roundedPos.x, roundedPos.y, roundedPos.z + 1) : BLOCK_NOTHING;
	index = chunkPieces[4]->searchForBlock(roundedPos.y - 1);
	nearbyBlocks[9] = index != -1 ? glm::vec3(roundedPos.x, roundedPos.y - 1, roundedPos.z + 1) : BLOCK_NOTHING;

	return nearbyBlocks;
}

bool World::isLoadingChunks()
{
	return m_chunkFutures.size() > 0;
}

float World::getHeightestPointAt(glm::vec3 pos)
{
	ChunkPiece* p = m_chunks[translateGlobalPosToChunkPos(pos, g_chunkWidthX, g_chunkWidthZ)]->getBlocksFromChunk(translateGlobalToLocalCoords(glm::ivec2(pos.x, pos.z), g_chunkWidthX, g_chunkWidthZ));
	return p->height;
}




