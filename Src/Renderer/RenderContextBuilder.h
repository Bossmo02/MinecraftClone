#ifndef RENDER_CONTEXT_BUILDER_HPP
#define RENDER_CONTEXT_BUILDER_HPP

#include "RenderContext.h"
#include <vector>

#include "../World/Blocks/BlockMesh.h"



class RenderContextBuilder
{
public:
	static RenderContextBuilder& get();

	void parseBlockDataToRenderContext(RenderContext* con, std::vector<BlockMeshData> blockData, std::vector<int> numOfFacesForBlocks);
    void sendDataToGPU(RenderContext* con);

private:

	RenderContextBuilder() = default;
	RenderContextBuilder(RenderContextBuilder& other) = delete;
	RenderContextBuilder operator= (RenderContextBuilder& other) = delete;

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

#endif // !RENDER_CONTEXT_BUILDER_HPP

