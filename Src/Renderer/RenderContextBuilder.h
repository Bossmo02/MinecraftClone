#include <vector>
#include "RenderContext.h"
#include "../Util/Timer.h"
#include "../World/Blocks/BlockMesh.h"


#ifdef _DEBUG
    #define CHECK_GL_ERROR(file, line) glCheckError(file, line)
#else
    #define CHECK_GL_ERROR(file, line) 
#endif // !_DEBUG



GLenum inline glCheckError(const char* file, int line)
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


void inline parseBlockDataToRenderContext(RenderContext* con, std::deque<BlockMeshData> blockData, std::deque<int> numOfFacesForBlocks)
{
    int indicesOffset = 0;
    int timesFaceAdded = 0;
    int timesWholeBlockAdded = 0;

    //
    // !!!--- zeros in the 'numOfFacesForBlocks' container will result in wrongly displayed blocks ---!!!
    //
    for (size_t i = 0; i < blockData.size(); ++i)
    {

        // if the right amount ao faces have been pushed into the vectors
        // we want to make sure that the next indices will be increased correctly
        if (numOfFacesForBlocks[timesWholeBlockAdded] == timesFaceAdded)
        {
            indicesOffset += numOfFacesForBlocks[timesWholeBlockAdded] * 4;
            timesWholeBlockAdded++;

            timesFaceAdded = 0;
        }


        for (size_t j = 0; j < blockData[i].vertices.size(); ++j)
            con->vec_vertexData.push_back(blockData[i].vertices[j]);

        for (size_t j = 0; j < blockData[i].texCoords.size(); ++j)
            con->vec_texCoordData.push_back(blockData[i].texCoords[j]);


        for (size_t j = 0; j < blockData[i].indices.size(); ++j)
            con->vec_indicesData.push_back(blockData[i].indices[j] + indicesOffset);


        for (size_t j = 0; j < 4; ++j)
            con->vec_lighlevelData.push_back(blockData[i].lightLevel);

        timesFaceAdded++;
    }

    con->numOfIndices = con->vec_indicesData.size();
}



void inline sendDataToGPU(RenderContext* con)
{
    CHECK_GL_ERROR(__FILE__, __LINE__);

    con->vertexArray.bindVAO();

    CHECK_GL_ERROR(__FILE__, __LINE__);

    con->vertexData.sendData(con->vec_vertexData.data(), sizeof(GLfloat) * con->vec_vertexData.size(), GL_STATIC_DRAW);
    con->textureData.sendData(con->vec_texCoordData.data(), sizeof(GLfloat) * con->vec_texCoordData.size(), GL_STATIC_DRAW);
    con->lightLevelData.sendData(con->vec_lighlevelData.data(), sizeof(GLfloat) * con->vec_lighlevelData.size(), GL_STATIC_DRAW);

    CHECK_GL_ERROR(__FILE__, __LINE__);

    con->vertexArray.linkVBO(con->vertexData, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    con->vertexArray.linkVBO(con->textureData, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    con->vertexArray.linkVBO(con->lightLevelData, 2, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (void*)0);

    CHECK_GL_ERROR(__FILE__, __LINE__);

    con->indicesData.sendData(con->vec_indicesData.data(), sizeof(GLuint) * con->vec_indicesData.size(), GL_STATIC_DRAW);

    CHECK_GL_ERROR(__FILE__, __LINE__);

    con->vertexArray.unbindVAO();

    CHECK_GL_ERROR(__FILE__, __LINE__);

    con->deleteContextData(false);
}
