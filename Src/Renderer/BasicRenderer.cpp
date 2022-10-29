#include "BasicRenderer.h"


#ifdef _DEBUG
#define CHECK_GL_ERROR(file, line) glCheckError(file, line) 
#else
#define CHECK_GL_ERROR(file, line) 
#endif // !_DEBUG




BasicRenderer& BasicRenderer::get(bool singleVBOMode)
{
	static BasicRenderer r(singleVBOMode);
	return r;
}

BasicRenderer::BasicRenderer(bool singleVBOMode)
{
	if (singleVBOMode)
	{
		m_defaultShader.createShader("src/shader/DefaultSingleVBO.vert", "src/shader/Default.frag");
	}
	else
	{
		m_defaultShader.createShader("src/shader/Default.vert", "src/shader/Default.frag");
	}
}

void BasicRenderer::draw(RenderContext& renderContext, glm::mat4& mvp, glm::ivec2 chunkPos, bool useDefaultShader, bool highlightChunk)
{
	glEnable(GL_CULL_FACE);

	renderContext.vertexArray.bindVAO();

	if (useDefaultShader)
		m_defaultShader.useShader();
	else
		renderContext.shader.useShader();

	CHECK_GL_ERROR(__FILE__, __LINE__);


	if (useDefaultShader)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_defaultShader.ID, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform2f(glGetUniformLocation(m_defaultShader.ID, "u_chunkPos"), (float)chunkPos.x, (float)chunkPos.y);
		glUniform1i(glGetUniformLocation(m_defaultShader.ID, "u_texture"), 0);

		if (highlightChunk)
			glUniform1f(glGetUniformLocation(m_defaultShader.ID, "u_debugHighlight"), 1.f);
		else
			glUniform1f(glGetUniformLocation(m_defaultShader.ID, "u_debugHighlight"), 0.f);
		
	}
	else
	{ 
		glUniformMatrix4fv(glGetUniformLocation(renderContext.shader.ID, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1i(glGetUniformLocation(renderContext.shader.ID, "u_texture"), 0);
	}

	CHECK_GL_ERROR(__FILE__, __LINE__);

	glDrawElements(GL_TRIANGLES, renderContext.numOfIndices, GL_UNSIGNED_INT, (void*)0);

	CHECK_GL_ERROR(__FILE__, __LINE__);

	if (useDefaultShader)
		m_defaultShader.unbindShader();
	else
		renderContext.shader.unbindShader();
	
	renderContext.vertexArray.unbindVAO();

	glDisable(GL_CULL_FACE);
}

