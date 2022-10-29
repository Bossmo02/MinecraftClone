#ifndef WATER_RENDERER_HPP
#define WATER_RENDERER_HPP


#include <glad/glad.h>

#include "RenderContext.h"


class WaterRenderer
{
public:

    static WaterRenderer& get(bool singleVBOMode = false);

    void draw(RenderContext& renderContext, glm::mat4& mvp, glm::ivec2 chunkPos, float totalTime, bool useDefaultShader = false);


private:
    WaterRenderer(bool singleVBOMode);
    WaterRenderer(const WaterRenderer& other) = delete;
    WaterRenderer operator= (WaterRenderer& other) = delete;

    Shader m_defaultShader;

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




#endif // !WATER_RENDERER_HPP
