#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP

#include <vector>

#include "../Util/OpenGL/VAO.h"
#include "../Util/OpenGL/VBO.h"
#include "../Util/OpenGL/EBO.h"
#include "../Util/OpenGL/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct RenderContext
{
	VAO vertexArray;
	VBO vertexData;
	VBO textureData;
	VBO lightLevelData;
	EBO indicesData;
	Shader shader;

	unsigned int numOfIndices = 0;

	std::vector<GLfloat> vec_vertexData;
	std::vector<GLfloat> vec_texCoordData;
	std::vector<GLuint> vec_indicesData;
	std::vector<GLfloat> vec_lighlevelData;

	void deleteContextData(bool deleteVAO)
	{
		if(deleteVAO)
			vertexArray.deleteVAO();

		vertexData.deleteVBO();
		textureData.deleteVBO();
		lightLevelData.deleteVBO();
		indicesData.deleteEBO();
		shader.deleteShader();

		vec_vertexData.clear();
		std::vector<GLfloat>().swap(vec_vertexData);
		vec_texCoordData.clear();
		std::vector<GLfloat>().swap(vec_texCoordData);
		vec_indicesData.clear();
		std::vector<GLuint>().swap(vec_indicesData);
		vec_lighlevelData.clear();
		std::vector<GLfloat>().swap(vec_lighlevelData);
	}
};



#endif // !RENDER_CONTEXT_HPP
