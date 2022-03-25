#ifndef WORLD_HPP
#define WOLRD_HPP

#include "Chunk.h"
#include "../Util/OpenGL/Camera.h"
#include "Gen/IVec2Hasher.h"
#include "../Util/HelperFunctions.h"

#include <stack>


class World
{
public:
	World(int seed, Camera* cam);
	World(int seed, Camera* cam, int(*heightFunction)(int x, int z, int seed));
	~World();

	void update(glm::vec3 camPos);

	void updateChunksAroundCam();
	void updateChunkdrawingOrder(glm::vec3 camPos);
	void updateFutures();
	void render(glm::mat4& mvp, float totalTime);


	void destroyBlock();
	void deleteFurthestChunks();

	// DEBUG
	void resetHighlighting();
	void reloadAllChunks();


private:

	glm::ivec2 m_oldChunkPos;
	std::unordered_map<glm::ivec2, Chunk*, IVec2Hasher> m_chunks;
	std::deque<std::pair<glm::ivec2, std::future<Chunk*>>> m_chunkFutures;

	// sorted chunks are used for drawing
	// chunks furthest away will be drawn first, closest last, in order to make work transparency
	// the first element of a pair represents the distance from the camera to the chunks origin (used for sorting)
	std::deque<std::pair<float, Chunk*>> m_sortedChunks;

	int(*m_heightFunction)(int x, int z, int seed);
	FastNoiseLite m_noise;
	Camera* m_cam;

#ifdef _DEBUG
	int m_maxLoadChunkDistance = 3;
#else
	int m_maxLoadChunkDistance = 15;
#endif // !_DEBUG

	int m_maxChunksLoadingCount = 30;
	int m_maxChunkAddPerIter = 3;
	int m_distanceToDelete = m_maxLoadChunkDistance + 2;

	// heigher values cause lags. Sending data to the GPU is slow
	int m_maxChunkToGPULoads = 3000000;


	int m_seed;


	float m_maxBlockReach = 8.f;
};


#endif // !WORLD_HPP