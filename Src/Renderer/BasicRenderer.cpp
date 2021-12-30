#include "BasicRenderer.h"


BasicRenderer& BasicRenderer::get()
{
	static BasicRenderer r;
	return r;
}

BasicRenderer::BasicRenderer()
{
	m_defaultShader.createShader("src/shader/Default.vert", "src/shader/Default.frag");
}

void BasicRenderer::draw(RenderContext& renderContext, glm::mat4& mvp, bool useDefaultShader)
{
	glEnable(GL_CULL_FACE);

	renderContext.vertexArray.bindVAO();

	if (useDefaultShader)
		m_defaultShader.useShader();
	else
		renderContext.shader.useShader();

#ifdef _DEBUG
	glCheckError(__FILE__, __LINE__);
#endif // _DEBUG


	if (useDefaultShader)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_defaultShader.ID, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1i(glGetUniformLocation(m_defaultShader.ID, "u_texture"), 0);
	}
	else
	{ 
		glUniformMatrix4fv(glGetUniformLocation(renderContext.shader.ID, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1i(glGetUniformLocation(renderContext.shader.ID, "u_texture"), 0);
	}

#ifdef _DEBUG
	glCheckError(__FILE__, __LINE__);
#endif // _DEBUG

	glDrawElements(GL_TRIANGLES, renderContext.numOfIndices, GL_UNSIGNED_INT, (void*)0);

#ifdef _DEBUG
	glCheckError(__FILE__, __LINE__);
#endif // _DEBUG

	if (useDefaultShader)
		m_defaultShader.unbindShader();
	else
		renderContext.shader.unbindShader();
	
	renderContext.vertexArray.unbindVAO();

	glDisable(GL_CULL_FACE);
}

