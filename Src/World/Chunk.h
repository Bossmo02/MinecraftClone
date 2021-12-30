#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <unordered_map>

#include "Blocks/Block.h"
#include "../Renderer/RenderContextBuilder.h"
#include "Gen/IVec2Hasher.h"
#include "Gen/Noise.h"
#include "../Util/HelperFunctions.h"


const unsigned int g_chunkWidthZ = 32;
const unsigned int g_chunkWidthX = 32;
const unsigned int g_baseChunkHeight = 5;

const unsigned int g_chunkSquared = 1024;
const unsigned int g_chunkCube = 32768;


struct ChunkPiece
{
    std::vector<std::unique_ptr<Block>> blocks;
    std::vector<short> reservedBlockPositions;
    short height, stoneHeight;

    void setup()
    {
        for (int i = height; i >= 0; --i)
        {
            reservedBlockPositions.push_back((short)i);
        }
    }

    int searchForBlock(unsigned int y) const
    {
        // returns the index of the block at given height
        for (int i = 0; i < blocks.size(); ++i)
        {
            if (blocks[i].get()->getWorldPos().y == y)
                return i;
        }

        return -1;
    }

    void addBlock(glm::vec3 pos, unsigned char visibleFaces)
    {
        if (blockReserved(pos.y))
        {
            if(pos.y == height)
                blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::GRASS, visibleFaces)));
            else if (pos.y <= stoneHeight && pos.y >= 0)
                blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::STONE, visibleFaces)));
            else
                blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::DIRT, visibleFaces)));
        }
    }

    bool blockReserved(unsigned int y)
    {
        for (size_t i = 0; i < reservedBlockPositions.size(); ++i)
        {
            if (reservedBlockPositions[i] == (short)y)
            {
                reservedBlockPositions.erase(reservedBlockPositions.begin() + i);
                return true;
            }
        }
        return false;
    }
};


class Chunk
{
public:

	Chunk(int x, int z, int* worldSeed, int(*heightFunction)(int x, int z, int seed));
	~Chunk();

    void init();

	void reloadMesh();
    void resetRenderContext(bool deleteVao);
    void sendRenderContextToGPU();
    std::vector<glm::ivec2> deleteBlock(glm::ivec3 globalBlockPos, size_t index, std::vector<Chunk*>* adjacentChunks);

	void renderChunk(glm::mat4& mvp);

	glm::ivec2 getWorldPosXZ() const;

    void fillSpaceY(int x, int z, int height);

    ChunkPiece* getBlocksFromChunk(glm::ivec2 pos);


    bool operator==(const Chunk& other) const
    {
        return other.m_chunkPos == m_chunkPos;
    }

    
	RenderContext m_renderContext;

private:

	void fillSpaceWith(int xStart, int zStart, int xEnd, int zEnd, int height, BLOCK_ID typeID);
    bool posInChunk(int x, int z) const;
    bool posOnChunkEdge(int x, int z) const;

    std::vector<glm::ivec2> loadSurroundingBlocks(glm::vec3 globalCenterPos, std::vector<Chunk*>* adjacentChunks);
    glm::ivec2 updateNearbyBlock(ChunkPiece* cPiece, glm::vec3 pos, unsigned char facesToDisplay);
	void loadMeshToRenderContext();

    // blocks are added to m_blocks when they are visible
	std::unordered_map<glm::ivec2, ChunkPiece, IVec2Hasher> m_solidBlocks;
    std::unordered_map<glm::ivec2, ChunkPiece, IVec2Hasher> m_transparentBlocks;
	glm::ivec2 m_chunkPos;
    int* m_heightFunc;
    int* m_worldSeed;

};




#endif // !CHUNK_HPP
