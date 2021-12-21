#include "World.h"


//---------------
// Functions used for multithreading
static std::mutex g_m;

Chunk* loadChunkAsync(int x, int z, int* seed, int(*heightFunction)(int x, int z, int seed))
{
	Chunk* c = new Chunk(x, z, seed, heightFunction);

	return c;
}
//-----------------------------


int defaultHeightFunction(int x, int z, int seed)
{
	FastNoiseLite n;
	n.SetSeed(seed);
	return (n.GetNoise((float)x, (float)z) * 4) + g_baseChunkHeight;
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


		if (m_oldChunkPos.x != currChunk.x && m_oldChunkPos.y != currChunk.y)
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

			m_oldChunkPos.x = currChunk.x;
			m_oldChunkPos.y = currChunk.y;
		}


	}
	
	updateFutures();

#ifdef _DEBUG
	//std::cout << "m_chunks size: " << m_chunks.size() * sizeof(glm::ivec2) + m_chunks.size() * sizeof(Chunk) << " bytes\n";
#endif // _DEBUG

}

void World::updateFutures()
{
	int count = 0;

	for (size_t i = 0; i < m_chunkFutures.size(); ++i)
	{
		if (m_chunkFutures[i].second.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
		{
			Chunk* c = m_chunkFutures[i].second.get();
			c->sendRenderContextToGPU();
			m_chunks[c->getWorldPosXZ()] = c;
			m_chunkFutures.erase(m_chunkFutures.begin() + i);
			count++;
		}

		if (count >= m_maxChunkToGPULoads)
			break;
	}
	
}



void World::draw(glm::mat4& mvp)
{
	for (auto& c : m_chunks)
		c.second->renderChunk(mvp);
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
					std::vector<glm::ivec2> chunksToUpdate = m_chunks[chunkPos]->deleteBlock(blockPos, j, &m_chunks);
					chunksToUpdate.push_back(chunkPos);

					for (auto& c : chunksToUpdate)
					{
						m_chunks[c]->resetRenderContext(false);
						m_chunks[c]->reloadMesh();
						m_chunks[c]->sendRenderContextToGPU();
					}
					return;
				}
			}
		}
	}

}




