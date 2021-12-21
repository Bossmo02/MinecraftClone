#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.h"
#include "../Util/OpenGL/Camera.h"
#include "Gen/IVec2Hasher.h"
#include "../Util/HelperFunctions.h"

#include <stack>
#include <future>


class World
{
public:
	World(int seed, Camera* cam);
	World(int seed, Camera* cam, int(*heightFunction)(int x, int z, int seed));
	~World();
	
	void updateChunksAroundCam();
	void updateFutures();
	void draw(glm::mat4& mvp);


	void destroyBlock();


private:

	glm::ivec2 m_oldChunkPos;
	std::unordered_map<glm::ivec2, Chunk*, IVec2Hasher> m_chunks;
	std::vector<std::pair<glm::ivec2, std::future<Chunk*>>> m_chunkFutures;


	int(*m_heightFunction)(int x, int z, int seed);
	FastNoiseLite m_noise;
	Camera* m_cam;
	int m_maxLoadChunkDistance = 2;
	int m_maxChunksLoadingCount = 1000;
	int m_maxChunkAddPerIter = 2000;
	int m_distanceToDelete = 30 * g_chunkWidthX;

	// heigher values cause lags. Sending data to the GPU is slow
	int m_maxChunkToGPULoads = 3;
	
	
	int m_seed;


	float m_maxBlockReach = 8.f;
};


#endif // !WORLD_HPP
