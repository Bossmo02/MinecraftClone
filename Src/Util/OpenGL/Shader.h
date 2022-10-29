#ifndef SHADER_HPP
#define SHADER_HPP


#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>



class Shader
{
public:

	GLuint ID = 0;

	Shader(const char* vertShaderSrc, const char* fragShaderSrc)
	{
		createShader(vertShaderSrc, fragShaderSrc);
	}

	Shader() {	}


	void createShader(const char* vertShaderSrc, const char* fragShaderSrc)
	{
		ID = glCreateProgram();

		// create vertex and fragment shaders
		GLuint vertId = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragId = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertCode = loadShaderSrc(vertShaderSrc);
		std::string fragCode = loadShaderSrc(fragShaderSrc);

		const char* pVertCode = vertCode.c_str();
		const char* pFragCode = fragCode.c_str();

		glShaderSource(vertId, 1, &pVertCode, nullptr);
		glShaderSource(fragId, 1, &pFragCode, nullptr);


		// compile shaders and check for compile errors
		glCompileShader(vertId);
		checkForErrors(vertId, vertShaderSrc, GL_COMPILE_STATUS);
		glCompileShader(fragId);
		checkForErrors(fragId, fragShaderSrc, GL_COMPILE_STATUS);


		// create shader program and check for link errors
		glAttachShader(ID, vertId);
		glAttachShader(ID, fragId);
		glLinkProgram(ID);
		

		// causes GL_ERROR INVALID_OPERATION
		//checkForErrors(ID, "shader program", GL_LINK_STATUS);


		// delete shaders because a copy of them exists in the final shader program
		glDeleteShader(vertId);
		glDeleteShader(fragId);
	}


	void useShader() { glUseProgram(ID); }
	void unbindShader() { glUseProgram(0); }
	void deleteShader() { glDeleteProgram(ID); }

private:

	std::string loadShaderSrc(const char* src)
	{
		std::ifstream in(src);

		std::string contents;

		if (in)
		{
			// get file size
			in.seekg(0, std::ios::end);
			contents.resize((const unsigned int)in.tellg());

#ifdef _DEBUG
			std::cout << in.tellg() << std::endl;
#endif // _DEBUG

			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			return contents;
		}
		
#ifdef _DEBUG
		std::cout << "Shader: File not Found  " << __FILE__ << "   " << src << std::endl;
#endif // _DEBUG


		return contents;
	}


	void checkForErrors(GLuint shaderId, const char* src, GLuint errorType)
	{
		int success;
		char infoLog[512];
		glGetShaderiv(shaderId, errorType, &success);

		if (!success)
		{
			glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
			std::cout << "Shader creation error! " << src << "\n" << infoLog << std::endl;
		}
	}
};


#endif // !SHADER_HPP