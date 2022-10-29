#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <array>

#include "../Util/OpenGL/Camera.h"
#include "../World/Blocks/Block.h"


class Player
{
public:
	Player(glm::vec3 spawnPos);
	~Player();

	void move(CAMERA_MOVEMENT_DIR dir, const float& deltaTime);
	void handleMouseInput(double xPos, double yPos);
	void setSprinting(bool sprinting);
	void setPlayerPos(glm::vec3 newPos);
	void destroyBlock();

	void update(const float& deltaTime, std::array<glm::vec3, 10> adjacentBlocks);

	Camera* getCamera();
	glm::vec3 getPlayerPos();
	glm::mat4 getPlayerViewMatrix();

	void setPlayerMovementType(bool fly);
	void enablePlayerMovement(bool enable);

	// DEBUG
	void printPlayerPosition(bool rounded);
	void printPlayerChunkLocalPosition();

private:

	void updateMovement(const float& deltaTime);
	void applyGravity(const float& deltaTime);
	void applyFriction(const float& deltaTime);
	void limitSpeed();

	void checkCollisions(std::array<glm::vec3, 10> adjacentBlocks);

	Camera m_cam;

	const float m_GRAVITY_MULTIPLIER = 0.2f;
	const float m_MAX_SPEED = 0.5f;
	const float m_JUMPSPEED = 0.1f;
	const float m_PLAYER_HEIGHT = 1.75f;
	const float m_PLAYER_WIDTH = 0.3f;
	const float m_SPRINT_SPEED_MULTIPLIER = 0.15f;
	const float m_DEFAULT_SPEED_MULTIPLIER = 0.075f;

	float m_currentSpeedMultiplier;
	glm::vec3 m_playerPos;
	glm::vec3 m_velocity;
	bool m_onGround;
	bool m_flying;
	bool m_enabledPlayerMovement;

	BLOCK_ID m_selectedBlockType;
};

#endif // !PLAYER_HPP