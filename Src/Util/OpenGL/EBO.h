#ifndef EBO_HPP
#define EBO_HPP

#include <glad/glad.h>

class EBO
{
public:
	GLuint ID = 0;

	EBO(GLuint* data, GLsizeiptr size, GLenum usage)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}

	EBO()
	{
		
	}

	void sendData(GLuint* data, GLsizeiptr size, GLenum usage)
	{
		if(ID == 0)
			glGenBuffers(1, &ID);

		bindEBO();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}

	void bindEBO()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	}
	void unbindEBO()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void deleteEBO()
	{
		glDeleteBuffers(1, &ID);
		ID = 0;
	}

};

#endif // !EBO_HPP