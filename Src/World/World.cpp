#include "World.h"


//---------------
// Functions used for multithreading

Chunk* loadChunkAsync(int x, int z, int* seed, int(*heightFunction)(int x, int z, int seed))
{
	Chunk* c = new Chunk(x, z, seed, heightFunction);

	return c;
}
//-----------------------------


int defaultHeightFunction(int x, int z, int seed)
{
	FastNoiseLite n1, n2;
	n1.SetSeed(seed);
	n2.SetSeed(seed + 1);

	float n2Multi = 60.4f;
	float mountainWidth = 0.2f;		// lower values -> wider mountains
	float mountainHeight = 10.f;	// higher values -> higher mountain tops

	n1.SetFrequency(0.005f);
	n1.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	n1.SetFractalLacunarity(mountainWidth);
	n1.SetFractalWeightedStrength(mountainHeight);


	float temp1 = ((n1.GetNoise((float)x, (float)z, n2.GetNoise((float)x, (float)z) * n2Multi)) - 2.f);
	float temp2 = 0;

	temp2 = temp1 * 20;
	
	return (int)(temp2);
}



World::World(int seed, Camera* cam)
{
	m_noise.SetSeed(seed);
	m_cam = cam;
	m_seed = seed;

	m_heightFunction = defaultHeightFunction;

	updateChunksAroundCam();
}

World::World(int seed, Camera* cam, int(*heightFunction)(int x, int z, int seed))
{
	m_noise.SetSeed(seed);
	m_cam = cam;
	m_seed = seed;

	m_heightFunction = heightFunction;

	updateChunksAroundCam();
}

World::~World()
{
	for (auto& c : m_chunks)
		delete c.second;

	m_chunks.clear();
}

void World::update(glm::vec3 camPos)
{
	updateChunksAroundCam();
	
	updateFutures();

	updateChunkdrawingOrder(camPos);
}

void World::updateChunksAroundCam()
{
	if (m_chunkFutures.size() < m_maxChunksLoadingCount)
	{
		glm::ivec2 currChunk = translateGlobalPosToChunkIndex(m_cam->getCamPos(), (int)(g_chunkWidthX), (int)(g_chunkWidthZ));

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
					m_chunkFutures.push_back(std::pair<glm::ivec2, std::future<Chunk*>>(currChunk, std::async(std::launch::async, loadChunkAsync, currChunk.x, currChunk.y, &m_seed, m_heightFunction)));
					timesChunkAdded++;
				}
			}
		}

		// did the player move into another chunk?
		if (m_oldChunkPos.x != currChunk.x && m_oldChunkPos.y != currChunk.y)
		{
			deleteFurthestChunks();
			m_oldChunkPos.x = currChunk.x;
			m_oldChunkPos.y = currChunk.y;
		}
	}
}



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

void World::draw(glm::mat4& mvp, float totalTime)
{
	for (size_t i = 0; i < m_sortedChunks.size(); i++)
	{
		m_sortedChunks[i].second->renderChunk(mvp, totalTime);
	}
}

void World::destroyBlock()
{
	for (float i = 0; i < m_maxBlockReach; i += 0.99f)
	{
		glm::vec3 pos = m_cam->castRay(i);
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
				glm::ivec3 blockPos = chunkPiece->blocks[j]->getWorldPos();
				if (pos.y > blockPos.y && pos.y < blockPos.y + 1.f)
				{
					// gather adjacent chunks
					std::vector<Chunk*> adjacentChunks;
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x - 32, chunkPos.y)]);
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x + 32, chunkPos.y)]);
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x, chunkPos.y - 32)]);
					adjacentChunks.push_back(m_chunks[glm::ivec2(chunkPos.x, chunkPos.y + 32)]);

					std::vector<glm::ivec2> chunksToUpdate = m_chunks[chunkPos]->deleteBlock(blockPos, j, &adjacentChunks);
					chunksToUpdate.push_back(chunkPos);

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

void World::deleteFurthestChunks()
{
	for (auto& c : m_chunks)
	{
		float xDiff = std::abs(m_cam->getCamPos().x - c.first.x);
		float zDiff = std::abs(m_cam->getCamPos().z - c.first.y);

		if (xDiff >= m_distanceToDelete || zDiff >= m_distanceToDelete)
		{
#ifdef _DEBUG
			std::cout << "Deleted Chunk at: " << c.first.x << ", " << c.first.y << "\n";
#endif // _DEBUG

			delete c.second;
			m_chunks.erase(c.first);
		}
	}	
}




