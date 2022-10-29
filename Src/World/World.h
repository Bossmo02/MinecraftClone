#ifndef WORLD_HPP
#define WOLRD_HPP

#include "Chunk.h"
#include "../Util/OpenGL/Texture.h"
#include "../Util/OpenGL/Camera.h"
#include "Gen/IVec2Hasher.h"
#include "../Util/HelperFunctions.h"

#include <stack>
#include <array>

class World
{
public:
	World(int seed);
	World(int seed, int(*heightFunction)(int x, int z, int seed));
	~World();

	void update(glm::vec3 camPos);
		
	void render(glm::mat4& mvp, float totalTime);

	void destroyBlock(Camera* cam);

	// DEBUG -----------
	void resetHighlighting();
	void reloadAllChunks();
	// -----------------

	std::array<glm::vec3, 5> getHeightsNearPos(glm::vec3 pos);
	std::array<glm::vec3, 10> getBlockPositionsAroundPlayerPos(glm::vec3 pos);

	bool isLoadingChunks();
	float getHeightestPointAt(glm::vec3 pos);

private:

	void updateChunksAroundCam(glm::vec3 camPos);
	void updateChunkdrawingOrder(glm::vec3 camPos);
	void deleteFurthestChunks(glm::vec3 camPos);
	void updateFutures();

	glm::ivec2 m_oldChunkPos;
	std::unordered_map<glm::ivec2, Chunk*, IVec2Hasher> m_chunks;
	std::deque<std::pair<glm::ivec2, std::future<Chunk*>>> m_chunkFutures;

	// sorted chunks are used for drawing
	// chunks furthest away will be drawn first, closest last, in order to make work transparency
	// the first element of a pair represents the distance from the camera to the chunks origin (used for sorting)
	std::deque<std::pair<float, Chunk*>> m_sortedChunks;

	int(*m_heightFunction)(int x, int z, int seed);
	FastNoiseLite m_noise;
	Texture m_texture;

#ifdef _DEBUG
	int m_maxLoadChunkDistance = 2;
#else
	int m_maxLoadChunkDistance = 6;
#endif // !_DEBUG

	int m_maxChunksLoadingCount = 30;
	int m_maxChunkAddPerIter = 30;
	int m_distanceToDelete = m_maxLoadChunkDistance + 2;

	// heigher values cause lags. Sending data to the GPU is slow
	int m_maxChunkToGPULoads = 3000000;


	int m_seed;


	float m_maxBlockReach = 8.f;
};


#endif // !WORLD_HPP