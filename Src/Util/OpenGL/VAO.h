#ifndef VAO_HPP
#define VAO_HPP

#include <glad/glad.h>
#include "VBO.h"


#include <iostream>


class VAO
{
public:
	GLuint ID = 0;

	VAO()
	{
	}

	void linkVBO(VBO& vbo, GLuint layout, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* offset)
	{
		vbo.bindVBO();
		glVertexAttribPointer(layout, size, type, normalized, stride, offset);
		glEnableVertexAttribArray(layout);
		vbo.unbindVBO();
	}

	void disableVertexAttribArray(GLint layout)
	{
		glDisableVertexAttribArray(layout);
	}

	void bindVAO()
	{
		if(ID == 0)
			glGenVertexArrays(1, &ID);

		glBindVertexArray(ID);
	}

	void unbindVAO()
	{
		glBindVertexArray(0);
	}

	void deleteVAO()
	{
		if (ID != 0)
		{
			glDeleteVertexArrays(1, &ID);
			ID = 0;
		}
	}

};

#endif // !VAO_HPP

