#include <glad/glad.h>
#include <GLFW/glfw3.h>

// matrices and vectors
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#include <STB/stb_image.h>
#include "Util/OpenGL/Texture.h"

#include "Util/OpenGL/Camera.h"

#include "World/World.h"



#include "Util/Timer.h"



float deltaTime = 0;
float lastTime = 0;
float scale = 1;
float mixTex = 0.3f;
bool mouseVisible = false;
glm::mat4 view = glm::mat4(1.0f);
void handleInput(GLFWwindow* window);
void resizeCallback(GLFWwindow* wnd, int width, int height);
void mouseMovementCallback(GLFWwindow* wnd, double xPos, double yPos);

Camera cam;

float sprintSpeedMulti = 20.f;
float currentSpeedMulti = 1.f;


void toggleMouse(GLFWwindow* wnd)
{
	if (mouseVisible)
	{
		glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		mouseVisible = false;
	}
	else
	{
		glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		mouseVisible = true;
	}
}

void getQuadVertices(std::vector<GLfloat>* vector, float x, float y, float z)
{
	GLfloat cubeVerticies[] =
	{
		//-----------pos----------,----texCoord------
		-0.5f + x, -0.5f + y, -0.5f + z,		0.0f, 0.0f,
		 0.5f + x, -0.5f + y, -0.5f + z,		10.0f, 0.0f,
		-0.5f + x,  0.5f + y, -0.5f + z,		0.0f, 10.0f,
		 0.5f + x,  0.5f + y, -0.5f + z,		10.0f, 10.0f
	};

	for (auto& x : cubeVerticies)
		vector->push_back(x);
}

void getCubeVerticiesAt(std::vector<GLfloat>* vector, float x, float y, float z)
{
	GLfloat cubeVerticies[] =
	{
		//-----------pos----------,----texCoord------
		-0.5f + x, -0.5f + y, -0.5f + z,		0.0f, 0.0f,
		 0.5f + x, -0.5f + y, -0.5f + z,		1.0f, 0.0f,
		-0.5f + x,  0.5f + y, -0.5f + z,		0.0f, 1.0f,
		 0.5f + x,  0.5f + y, -0.5f + z,		1.0f, 1.0f,
			  
		-0.5f + x, -0.5f + y,  0.5f + z,		0.0f, 0.0f,
		 0.5f + x, -0.5f + y,  0.5f + z,		1.0f, 0.0f,
		-0.5f + x,  0.5f + y,  0.5f + z,		0.0f, 1.0f,
		 0.5f + x,  0.5f + y,  0.5f + z,		1.0f, 1.0f,

		-0.5f + x,  0.5f + y,  0.5f + z,		1.0f, 0.0f,
		-0.5f + x,  0.5f + y, -0.5f + z,		1.0f, 1.0f,
		-0.5f + x, -0.5f + y, -0.5f + z,		0.0f, 1.0f,
		-0.5f + x, -0.5f + y,  0.5f + z,		0.0f, 0.0f,
			  
		 0.5f + x,  0.5f + y,  0.5f + z,		1.0f, 0.0f,
		 0.5f + x,  0.5f + y, -0.5f + z,		1.0f, 1.0f,
		 0.5f + x, -0.5f + y, -0.5f + z,		0.0f, 1.0f,
		 0.5f + x, -0.5f + y,  0.5f + z,		0.0f, 0.0f,
			 
		-0.5f + x, -0.5f + y, -0.5f + z,		0.0f, 1.0f,
		 0.5f + x, -0.5f + y, -0.5f + z,		1.0f, 1.0f,
		 0.5f + x, -0.5f + y,  0.5f + z,		1.0f, 0.0f,
		-0.5f + x, -0.5f + y,  0.5f + z,		0.0f, 0.0f,
			  
		 0.5f + x,  0.5f + y, -0.5f + z,		1.0f, 1.0f,
		 0.5f + x,  0.5f + y,  0.5f + z,		1.0f, 0.0f,
		-0.5f + x,  0.5f + y,  0.5f + z,		0.0f, 0.0f,
		-0.5f + x,  0.5f + y, -0.5f + z,		0.0f, 1.0f

	};

	for(auto& x : cubeVerticies)
		vector->push_back(x);
}

void getQuadIndices(std::vector<GLuint>* vector, int index)
{
	GLuint offset = index * 6;

	GLuint cubeIndices[] =
	{
		//Back
		0 + offset, 1 + offset, 3 + offset,
		0 + offset, 2 + offset, 3 + offset
	};

	for (auto& x : cubeIndices)
		vector->push_back(x);
}

void getCubeIndicesAt(std::vector<GLuint>* vector, int index)
{
	GLuint offset = index * 24;

	GLuint cubeIndices[] =
	{
		//Back
		0 + offset, 1 + offset, 3 + offset,
		0 + offset, 2 + offset, 3 + offset,

		//Front
		4 + offset, 5 + offset, 7 + offset,
		4 + offset, 6 + offset, 7 + offset,

		// Left
		11 + offset, 8 + offset,  9 + offset,
		11 + offset, 10 + offset, 9 + offset,

		//Right
		15 + offset, 12 + offset, 13 + offset,
		15 + offset, 14 + offset, 13 + offset,

		// Down
		19 + offset, 16 + offset, 17 + offset,
		19 + offset, 18 + offset, 17 + offset,

		// Up
		22 + offset, 21 + offset, 20 + offset,
		22 + offset, 23 + offset, 20 + offset
	};
	
	for (auto& x : cubeIndices)
		vector->push_back(x);
}


struct Vertex
{
	float pos[3];
	float texCoords[2];
};



int main()
{
	int width = 1024;
	int height = 640;

	//Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//---------------------------------------------------------------------
	//Window creation
	GLFWwindow* pWindow = glfwCreateWindow(width, height, "Minecraft?", NULL, NULL);
	if (pWindow == NULL)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(pWindow);
	glfwSetFramebufferSizeCallback(pWindow, resizeCallback);
	glfwSetCursorPosCallback(pWindow, mouseMovementCallback);

	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//-----------------------------------------------------------------------------
	//GLAD initialization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!\n";
		return -2;
	}
	glViewport(0, 0, width, height);

	//-----------------------------------------------------------------------------
	//Creating Buffers

	World* world;
	world = new World(2, &cam);
	


	//VAO firstVao;
	//firstVao.bindVAO();


	//std::vector<GLuint> eboData;
	//std::vector<GLfloat> vboData;
	//std::vector<GLfloat> vboTexData;


	//// load vbo
	//for (size_t i = 0; i < blockData.size(); ++i)
	//{
	//	for (size_t j = 0; j < blockData[i].vertices.size(); ++j)
	//	{
	//		vboData.push_back(blockData[i].vertices[j]);
	//	}
	//	
	//	for (size_t j = 0; j < blockData[i].texCoords.size(); ++j)
	//	{
	//		vboTexData.push_back(blockData[i].texCoords[j]);
	//	}
	//}

	//// load ebo
	//for (size_t i = 0; i < blockData.size(); ++i)
	//{
	//	for (size_t j = 0; j < blockData[i].indices.size(); ++j)
	//	{
	//		eboData.push_back(blockData[i].indices[j]);
	//	}
	//}


	//VBO vbo(vboData.data(), sizeof(GLfloat) * vboData.size(), GL_STATIC_DRAW);
	//VBO vboTex(vboTexData.data(), sizeof(GLfloat) * vboTexData.size(), GL_STATIC_DRAW);
	//EBO ebo(eboData.data(), sizeof(GLuint) * eboData.size(), GL_STATIC_DRAW);


	//firstVao.linkVBO(vbo, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	//firstVao.linkVBO(vboTex, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);


	Texture tex1(GL_TEXTURE_2D, GL_TEXTURE0, "Res/spritesheet_tiles.png");

	// projection from camera
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(75.0f), (float)(width / height), 0.1f, 1000.0f);

	glm::mat4 model = glm::mat4(1.0f);


	glEnable(GL_DEPTH_TEST);


	float passedTime = 0;

	//---------------------------------------------------------------------------
	//Loop
	//
	while (!glfwWindowShouldClose(pWindow))
	{
		deltaTime = (float)glfwGetTime() - lastTime;
		lastTime = (float)glfwGetTime();
		passedTime += deltaTime;

		//std::cout << "Frametime: " << deltaTime << "\n";


		handleInput(pWindow);
		glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		/*if (passedTime > 5.f)
		{
			passedTime = 0;
			world = World(glfwGetTime());
		}*/
		world->updateChunksAroundCam();

		glm::mat4 mvp = projection * cam.getViewMatrix() * model;

		world->draw(mvp);


		//// create transformations
		//glm::mat4 transform2 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));
		//transform2 = glm::scale(transform2, glm::vec3(sin(glfwGetTime()), sin(glfwGetTime()), sin(glfwGetTime())));

		//glUniformMatrix4fv(glGetUniformLocation(shader.m_shaderId, "transform"), 1, GL_FALSE, glm::value_ptr(transform2));

		//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);


		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	/*firstVao.deleteVAO();
	vbo.deleteVBO();
	ebo.deleteEBO();*/

	tex1.deleteTexture();
	delete world;

	glfwTerminate();


}//---------------------------------------
// main end


void handleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		toggleMouse(window);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.moveRelativeToCamFront(CAMERA_MOVEMENT_DIR::FORWARD, deltaTime * currentSpeedMulti);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.moveRelativeToCamFront(CAMERA_MOVEMENT_DIR::BACKWARD, deltaTime * currentSpeedMulti);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.moveRelativeToCamFront(CAMERA_MOVEMENT_DIR::LEFT, deltaTime * currentSpeedMulti);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.moveRelativeToCamFront(CAMERA_MOVEMENT_DIR::RIGHT, deltaTime * currentSpeedMulti);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam.moveRelativeToCamFront(CAMERA_MOVEMENT_DIR::UP, deltaTime * currentSpeedMulti);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cam.moveRelativeToCamFront(CAMERA_MOVEMENT_DIR::DOWN, deltaTime * currentSpeedMulti);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		currentSpeedMulti = sprintSpeedMulti;
	else
		currentSpeedMulti = 1.f;

}


void resizeCallback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseMovementCallback(GLFWwindow* wnd, double xPos, double yPos)
{
	cam.mouseMovement(xPos, yPos);
}
