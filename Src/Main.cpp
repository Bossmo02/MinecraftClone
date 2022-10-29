#include <glad/glad.h>
#include <GLFW/glfw3.h>

// matrices and vectors
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#include <STB/stb_image.h>

#include "World/World.h"
#include "Util/UI/UI.h"
#include "Player/Player.h"


#include "Util/Timer.h"




void* operator new(size_t size)
{
	return malloc(size);
}




float deltaTime = 0;
float lastTime = 0;
float scale = 1;
float mixTex = 0.3f;
float fov = 125.0f;
bool mouseVisible = false;
glm::mat4 view = glm::mat4(1.0f);
void handleInput(GLFWwindow* window);
void resizeCallback(GLFWwindow* wnd, int width, int height);
void mouseMovementCallback(GLFWwindow* wnd, double xPos, double yPos);


World* world;
UI* ui;
Player* player;


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



void setupUi(UI* ui)
{
	float size = 0.12f;
	float yPos = -0.85f;

	// Crosshair
	ui->addUIElement("crosshair", glm::vec2(0, 0), glm::vec2(0.03, 0.03), glm::vec2(4, 0));

	// Grass Icon
	ui->addUIElement("grass", glm::vec2(-(size * 1.5), yPos), glm::vec2(size, size), glm::vec2(0, 0));
	ui->setHighlight("grass");
	// Dirt Icon
	ui->addUIElement("dirt", glm::vec2(-(size * 0.5), yPos), glm::vec2(size, size), glm::vec2(1, 0));
	// Stone Icon
	ui->addUIElement("stone", glm::vec2( (size * 0.5), yPos), glm::vec2(size, size), glm::vec2(2, 0));
	// Sand Icon
	ui->addUIElement("sand", glm::vec2( (size * 1.5), yPos), glm::vec2(size, size), glm::vec2(3, 0));
}




int main()
{
	int width = 1500;
	int height = 1000;

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

	glm::vec3 playerSpwanPos = glm::vec3(0, 0, 0);

	player = new Player(playerSpwanPos);
	world = new World(2);
	ui = new UI("Res/sprites.png", GL_TEXTURE1, glm::vec2(128, 128), glm::vec2(640, 128));
	setupUi(ui);
	

	// projection from camera
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(fov), (float)(width / height), 0.1f, 800.0f);

	//glm::mat4 model = glm::mat4(1.0f);




	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);

	float passedTime = 0;

	// initial chunk loading
	std::cout << "Building World";
	do
	{
		world->update(player->getPlayerPos());
		std::this_thread::sleep_for((std::chrono::milliseconds)100);
		std::cout << ".";
	} while (world->isLoadingChunks());

	std::cout << "Done\n";

	player->setPlayerPos(glm::vec3(player->getPlayerPos().x, world->getHeightestPointAt(player->getPlayerPos()) + 20, player->getPlayerPos().z));
	player->enablePlayerMovement(true);

	//---------------------------------------------------------------------------
	//Loop
	//
	while (!glfwWindowShouldClose(pWindow))
	{
		deltaTime = (float)glfwGetTime() - lastTime;
		lastTime = (float)glfwGetTime();
		passedTime += deltaTime;


		projection = glm::perspective(glm::radians(fov), (float)(width / height), 0.1f, 1000.0f);


		handleInput(pWindow);
		glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		world->update(player->getPlayerPos());
		player->update(deltaTime, world->getBlockPositionsAroundPlayerPos(player->getPlayerPos()));

		glm::mat4 mvp = projection * player->getPlayerViewMatrix();// *model;

		world->render(mvp, passedTime);
		ui->render();


		glfwSwapBuffers(pWindow);
		glfwPollEvents();

	}


	delete world;
	delete ui;

	glfwTerminate();

	return 0;


}//---------------------------------------
// main end




void handleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	

	// player movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player->move(CAMERA_MOVEMENT_DIR::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player->move(CAMERA_MOVEMENT_DIR::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		player->move(CAMERA_MOVEMENT_DIR::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		player->move(CAMERA_MOVEMENT_DIR::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		player->move(CAMERA_MOVEMENT_DIR::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		player->move(CAMERA_MOVEMENT_DIR::DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		player->setSprinting(true);
	else
		player->setSprinting(false);

	// block breaking
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		world->destroyBlock(player->getCamera());

	// block selection
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		ui->setHighlight("grass");
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		ui->setHighlight("dirt");
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		ui->setHighlight("stone");
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		ui->setHighlight("sand");
	}

	// debug controls
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		toggleMouse(window);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		world->resetHighlighting();
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		world->reloadAllChunks();
	
	if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS)
		player->setPlayerMovementType(true);
	if (glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS)
		player->setPlayerMovementType(false);
	if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS)
		player->printPlayerPosition(true);
	if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_PRESS)
		player->printPlayerPosition(false);
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
		player->printPlayerChunkLocalPosition();
}


void resizeCallback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseMovementCallback(GLFWwindow* wnd, double xPos, double yPos)
{
	player->handleMouseInput(xPos, yPos);
}
