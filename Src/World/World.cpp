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


	m_chunkFutures.push_back(std::pair<glm::ivec2, std::future<Chunk*>>(glm::ivec2(0, 0), std::async(std::launch::async, loadChunkAsync, 0, 0, &m_seed, m_heightFunction)));


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
		int x = (int)m_cam->getCamPos().x;
		int z = (int)m_cam->getCamPos().z;

		int currChunkX = (x / (int)g_chunkWidthX);
		int currChunkZ = (z / (int)g_chunkWidthZ);

		int timesChunkAdded = 0;

		for (int i = currChunkX - m_loadChunkDistance; i <= currChunkX + m_loadChunkDistance; ++i)
		{
			for (int j = currChunkZ - m_loadChunkDistance; j <= currChunkZ + m_loadChunkDistance; ++j)
			{
				bool loading = false;
				glm::ivec2 currPos = { i * int(g_chunkWidthX), j * int(g_chunkWidthZ) };

				// check if chunk is loading
				for (auto& f : m_chunkFutures)
				{
					if (f.first == currPos)
					{
						loading = true;
						break;
					}
				}

				// check if chunk is loaded
				if (!m_chunks.contains(currPos) && !loading && timesChunkAdded < m_maxChunkAddPerIter)
				{
					m_chunkFutures.push_back(std::pair<glm::ivec2, std::future<Chunk*>>(currPos, std::async(std::launch::async, loadChunkAsync, currPos.x, currPos.y, &m_seed, m_heightFunction)));
					timesChunkAdded++;
				}
			}
		}


		if (m_oldChunkPos.x != currChunkX && m_oldChunkPos.y != currChunkZ)
		{
			/*for (size_t i = 0; i < m_chunks.size(); ++i)
			{
				float xDiff = std::abs(m_cam->getCamPos().x - m_chunks[i]->getWorldPosXZ().x);
				float zDiff = std::abs(m_cam->getCamPos().z - m_chunks[i]->getWorldPosXZ().y);

				if (xDiff >= g_chunkWidthX * (m_updateChunkDistance + 1) || zDiff >= g_chunkWidthZ * (m_updateChunkDistance + 1))
				{
					m_chunks.erase(m_chunks.begin() + i);
				}
			}*/

			m_oldChunkPos.x = currChunkX;
			m_oldChunkPos.y = currChunkZ;
		}


	}
	
	updateFutures();

#ifdef _DEBUG
	std::cout << "m_chunks size: " << m_chunks.size() * sizeof(glm::ivec2) + m_chunks.size() * sizeof(Chunk) << " bytes\n";
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


