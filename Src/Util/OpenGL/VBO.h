#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>

class VBO
{
public:
	GLuint ID = 0;

	VBO(GLfloat* data, GLsizeiptr size, GLenum usage) 
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	VBO()
	{
	}

	void sendData(GLfloat* data, GLsizeiptr size, GLenum usage)
	{
		if(ID == 0)
			glGenBuffers(1, &ID);

		bindVBO();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	void sendData(GLuint* data, GLsizeiptr size, GLenum usage)
	{
		if (ID == 0)
			glGenBuffers(1, &ID);

		bindVBO();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	void bindVBO()
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}
	void unbindVBO()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void deleteVBO()
	{
		if (ID != 0)
			glDeleteBuffers(1, &ID);
		
		ID = 0;
	}

};

#endif // !VBO_HPP


