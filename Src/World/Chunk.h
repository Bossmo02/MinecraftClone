#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <unordered_map>

#include "Blocks/Block.h"
#include "../Renderer/RenderContextBuilder.h"
#include "Gen/IVec2Hasher.h"
#include "Gen/Noise.h"


const unsigned int g_chunkWidthZ = 32;
const unsigned int g_chunkWidthX = 32;
const unsigned int g_baseChunkHeight = 5;

const unsigned int g_chunkSquared = 1024;
const unsigned int g_chunkCube = 32768;



class Chunk
{
public:

	Chunk(int x, int z, int* worldSeed, int(*heightFunction)(int x, int z, int seed));
	~Chunk();

    void init();

	void reloadMesh();
    void sendRenderContextToGPU();

	void renderChunk(glm::mat4& mvp);

	glm::ivec2 getWorldPosXZ() const;

    void fillSpaceY(int x, int z, int height);


    bool operator==(const Chunk& other) const
    {
        return other.m_chunkPos == m_chunkPos;
    }

    
	RenderContext m_renderContext;

private:

	void fillSpaceWith(int xStart, int zStart, int xEnd, int zEnd, int height, BLOCK_ID typeID);
    bool posInChunk(int x, int z) const;
    bool posOnChunkEdge(int x, int z) const;
    glm::ivec2 translateGlobalToLocalCoords(glm::ivec2 globalCoords);
    glm::ivec2 translateLocalToGlobalCoords(glm::ivec2 localCoords);

    const std::unordered_map<glm::ivec2, std::vector<Block*>, IVec2Hasher>& getBlocksFromChunk() const;

	void loadMeshToRenderContext();

    // blocks are added to m_blocks when they are visible
	std::unordered_map<glm::ivec2, std::vector<Block*>, IVec2Hasher> m_solidBlocks;
    std::unordered_map<glm::ivec2, std::vector<Block*>, IVec2Hasher> m_transparentBlocks;
	glm::ivec2 m_chunkPos;
    int* m_heightFunc;
    int* m_worldSeed;


    GLenum glCheckError(const char* file, int line)
    {
        GLenum errorCode;
        while ((errorCode = glGetError()) != GL_NO_ERROR)
        {
            std::string error;
            switch (errorCode)
            {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << error << " | " << file << " (" << line << ")" << std::endl;
        }
        return errorCode;
    }

};




#endif // !CHUNK_HPP
