#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <unordered_map>
#include <future>
#include <deque>

#include "ChunkPiece.h"
#include "Blocks/WaterBlock.h"
#include "../Renderer/RenderContextBuilder.h"
#include "Gen/IVec2Hasher.h"
#include "Gen/Noise.h"
#include "../Util/HelperFunctions.h"


const unsigned int g_chunkWidthZ = 32;
const unsigned int g_chunkWidthX = 32;


const unsigned int g_chunkSquared = 1024;
const unsigned int g_chunkCube = 32768;



class Chunk
{
public:

    Chunk(int x, int z, int worldSeed, int(*heightFunction)(int x, int z, int seed));
    ~Chunk();

    void renderChunk(glm::mat4& mvp, float totalTime);

    void reloadMesh();
    void resetRenderContext(bool deleteVao);
    void sendRenderContextSolidToGPU();
    void sendRenderContextWaterToGPU();
    std::vector<glm::ivec2> deleteBlock(glm::ivec3 globalBlockPos, size_t index, std::vector<Chunk*>* adjacentChunks);

    glm::ivec2 getWorldPosXZ() const;

    void fillSpaceY(int x, int z, int height);

    ChunkPiece* getBlocksFromChunk(glm::ivec2 pos);


    bool operator==(const Chunk& other) const
    {
        return other.m_chunkPos == m_chunkPos;
    }


    RenderContext m_renderContextSolid;
    RenderContext m_renderContextWater;

    
    // DEBUG functions
    void setHighlighting(bool highlight);


private:

    void fillSpaceWith(int xStart, int zStart, int xEnd, int zEnd, int height, BLOCK_ID typeID);
    bool posInChunk(int x, int z) const;
    bool posOnChunkEdge(int x, int z) const;

    std::vector<glm::ivec2> loadSurroundingBlocks(glm::vec3 globalCenterPos, std::vector<Chunk*>* adjacentChunks);
    glm::ivec2 updateNearbyBlock(ChunkPiece* cPiece, glm::vec3 pos, unsigned char facesToDisplay);
    void loadSolidMeshToRenderContext();
    void loadWaterMeshToRenderContext();

    // visible blocks are stored in these containers
    std::unordered_map<glm::ivec2, ChunkPiece, IVec2Hasher> m_solidBlocks;
    std::deque<std::unique_ptr<WaterBlock>> m_waterBlocks;

    glm::ivec2 m_chunkPos;
    int m_worldSeed;


    // DEBUG
    bool m_isHighlighted;

};




#endif // !CHUNK_HPP