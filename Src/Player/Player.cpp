#include "Player.h"

#include "../Util/HelperFunctions.h"


#include <iostream>
#include <algorithm>


Player::Player(glm::vec3 spawnPos)
	:m_cam(spawnPos, glm::vec3(1.0f, 0.0f, 0.0f), 500, CAMERA_TYPES::CAM_FLY)
{
	m_currentSpeedMultiplier = m_DEFAULT_SPEED_MULTIPLIER;
	m_playerPos = spawnPos;
	m_velocity = glm::vec3(0.f, 0.f, 0.f);
	m_selectedBlockType = BLOCK_ID::NONE;
	m_flying = false;
	m_onGround = false;
	m_enabledPlayerMovement = false;
}

Player::~Player()
{

}

void Player::move(CAMERA_MOVEMENT_DIR dir, const float& deltaTime)
{
	// TODO
	// implement FP movement with acceleration and stuff
	if (m_enabledPlayerMovement)
	{
		if (!m_flying)
		{
			glm::vec3 playerLookingVector = m_cam.getCamFront();

			switch (dir)
			{
			case CAMERA_MOVEMENT_DIR::FORWARD:
				m_velocity += m_currentSpeedMultiplier * glm::normalize(glm::cross(/*up-vector->*/ glm::vec3(0.0f, 1.0f, 0.0f), glm::cross(playerLookingVector, /*up-vector->*/ glm::vec3(0.0f, 1.0f, 0.0f))));
				break;
			case CAMERA_MOVEMENT_DIR::BACKWARD:
				m_velocity -= m_currentSpeedMultiplier * glm::normalize(glm::cross(/*up-vector->*/ glm::vec3(0.0f, 1.0f, 0.0f), glm::cross(playerLookingVector, /*up-vector->*/ glm::vec3(0.0f, 1.0f, 0.0f))));;
				break;
			case CAMERA_MOVEMENT_DIR::LEFT:
				m_velocity -= m_currentSpeedMultiplier * glm::normalize(glm::cross(playerLookingVector, /*up-vector->*/ glm::vec3(0.0f, 1.0f, 0.0f)));
				break;
			case CAMERA_MOVEMENT_DIR::RIGHT:
				m_velocity += m_currentSpeedMultiplier * glm::normalize(glm::cross(playerLookingVector, /*up-vector->*/ glm::vec3(0.0f, 1.0f, 0.0f)));
				break;
			case CAMERA_MOVEMENT_DIR::UP:
				if (m_onGround)
				{
					m_velocity.y = m_JUMPSPEED;
					m_onGround = false;
				}
				break;
			case CAMERA_MOVEMENT_DIR::DOWN:
				break;
			default:
				break;
			}

			limitSpeed();
		}
		else
		{
			m_cam.moveRelativeToCamFront(dir, deltaTime * m_currentSpeedMultiplier);
		}
	}
}

void Player::handleMouseInput(double xPos, double yPos)
{
	m_cam.mouseMovement(xPos, yPos);
}

void Player::setSprinting(bool sprinting)
{
	m_currentSpeedMultiplier = sprinting ? m_SPRINT_SPEED_MULTIPLIER : m_DEFAULT_SPEED_MULTIPLIER;
}

void Player::setPlayerPos(glm::vec3 newPos)
{
	m_playerPos = newPos;
}

void Player::destroyBlock()
{

}

void Player::update(const float& deltaTime, std::array<glm::vec3, 10> adjacentBlocks)
{
	if (m_enabledPlayerMovement && !m_flying)
	{
		updateMovement(deltaTime);

		checkCollisions(adjacentBlocks);
	}
}

Camera* Player::getCamera()
{
	return &m_cam;
}

glm::vec3 Player::getPlayerPos()
{
	return m_cam.getCamPos();
}

glm::mat4 Player::getPlayerViewMatrix()
{
	return m_cam.getViewMatrix();
}

void Player::setPlayerMovementType(bool fly)
{
	m_flying = fly;

	if (!fly)
	{
		m_playerPos = m_cam.getCamPos();
	}
}

void Player::enablePlayerMovement(bool enable)
{
	m_enabledPlayerMovement = enable;
}

void Player::printPlayerPosition(bool rounded)
{
	if (rounded)
		std::cout << "PLAYER::POS\tX: " << (int)m_cam.getCamPos().x << "\tY: " << (int)m_cam.getCamPos().y << "\tZ: " << (int)m_cam.getCamPos().z << "\n";
	else
		std::cout << "PLAYER::POS\tX: " << m_cam.getCamPos().x << "\tY: " << m_cam.getCamPos().y << "\tZ: " << m_cam.getCamPos().z << "\n";
}

void Player::printPlayerChunkLocalPosition()
{
	glm::vec3 pos = m_cam.getCamPos();
	// needs offset in the negative to work
	if (pos.x < 0)
		pos.x--;
	if (pos.z < 0)
		pos.z--;

	glm::ivec2 iPos = translateGlobalToLocalCoords(glm::ivec2(pos.x, pos.z), 32, 32);
	std::cout << "PLAYER::POS\tX: " << iPos.x << "\tZ: " << iPos.y << "\n";
}

void Player::updateMovement(const float& deltaTime)
{
	applyGravity(deltaTime);
	applyFriction(deltaTime);
	m_playerPos += m_velocity;
	m_cam.setCamPos(m_playerPos);
	
}

void Player::applyGravity(const float& deltaTime)
{
	// gravity is constant for now
	if (m_velocity.y > -m_MAX_SPEED && !m_onGround)
		m_velocity.y -= m_GRAVITY_MULTIPLIER * deltaTime;
}

void Player::applyFriction(const float& deltaTime)
{
	//if (m_onGround)
	{
		m_velocity.x *= 0.5f;
		m_velocity.z *= 0.5f;
	}
}

void Player::limitSpeed()
{
	float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z);

	if (speed > m_MAX_SPEED)
	{
		m_velocity.x = (m_velocity.x / speed) * m_MAX_SPEED;
		m_velocity.z = (m_velocity.z / speed) * m_MAX_SPEED;
	}
}


// ---------------------------------- Collsision stuff --------------------------------------------

void Player::checkCollisions(std::array<glm::vec3, 10> adjacentBlocks)
{
	// adjacentBlocks array layout:
	// 0: bottom block
	// 1: top block
	// 2-3: x-1
	// 4-5: z-1
	// 6-7: x+1
	// 8-9: z+1

	const float BLOCK_HALFSIZE = 0.5f;

	if (adjacentBlocks[0] != BLOCK_NOTHING)
	{
		float dist = 0;

		if (adjacentBlocks[0].y > 0)
			dist = m_playerPos.y - m_PLAYER_HEIGHT - adjacentBlocks[0].y;  //(m_playerPos.y - m_PLAYER_HEIGHT) - adjacentBlocks[0].y - 1;
		else
			dist = m_playerPos.y - m_PLAYER_HEIGHT - adjacentBlocks[0].y;

		if (dist < 0)
		{
			m_onGround = true;
			m_velocity.y = 0;
						
			m_playerPos -= glm::vec3(0, dist, 0);
			
			std::cout << dist << "CD: BOTTOM\n";
			m_cam.setCamPos(m_playerPos);
		}
	}
	else
	{
		m_onGround = false;
	}

	if (adjacentBlocks[1] != BLOCK_NOTHING)
	{
		glm::vec3 centerBlockPos = glm::vec3(adjacentBlocks[1].x + BLOCK_HALFSIZE, adjacentBlocks[1].y - BLOCK_HALFSIZE, adjacentBlocks[1].z + BLOCK_HALFSIZE);

		float dist = centerBlockPos.y - (m_playerPos.y + m_PLAYER_WIDTH / 2) + BLOCK_HALFSIZE;

		if (dist < 0)
		{
			m_velocity.y = 0;
			m_playerPos += glm::vec3(0, dist, 0);
			std::cout << dist << "CD: TOP\n";
			m_cam.setCamPos(m_playerPos);
		}

	}

	// x-1
	if (adjacentBlocks[2] != BLOCK_NOTHING || adjacentBlocks[3] != BLOCK_NOTHING)
	{
		glm::vec3 bPos = adjacentBlocks[2] != BLOCK_NOTHING ? adjacentBlocks[2] : adjacentBlocks[3];

		glm::vec3 centerBlockPos = glm::vec3(bPos.x + BLOCK_HALFSIZE, bPos.y - BLOCK_HALFSIZE, bPos.z + BLOCK_HALFSIZE);

		float dist = centerBlockPos.x - m_playerPos.x + (BLOCK_HALFSIZE + m_PLAYER_WIDTH);

		if (dist > 0)
		{
			m_velocity.x = 0;
			m_playerPos += glm::vec3(dist, 0, 0);
			std::cout << dist << "CD: X-1\n";
			m_cam.setCamPos(m_playerPos);
		}
	}

	// z-1
	if (adjacentBlocks[4] != BLOCK_NOTHING || adjacentBlocks[5] != BLOCK_NOTHING)
	{
		glm::vec3 bPos = adjacentBlocks[4] != BLOCK_NOTHING ? adjacentBlocks[4] : adjacentBlocks[5];

		glm::vec3 centerBlockPos = glm::vec3(bPos.x + BLOCK_HALFSIZE, bPos.y - BLOCK_HALFSIZE, bPos.z + BLOCK_HALFSIZE);

		float dist = centerBlockPos.z - m_playerPos.z + (BLOCK_HALFSIZE + m_PLAYER_WIDTH);

		if (dist > 0)
		{
			m_velocity.z = 0;
			m_playerPos += glm::vec3(0, 0, dist);
			std::cout << dist << "CD: Z-1\n";
			m_cam.setCamPos(m_playerPos);
		}
	}

	// x+1
	if (adjacentBlocks[6] != BLOCK_NOTHING || adjacentBlocks[7] != BLOCK_NOTHING)
	{
		glm::vec3 bPos = adjacentBlocks[6] != BLOCK_NOTHING ? adjacentBlocks[6] : adjacentBlocks[7];

		glm::vec3 centerBlockPos = glm::vec3(bPos.x + BLOCK_HALFSIZE, bPos.y - BLOCK_HALFSIZE, bPos.z + BLOCK_HALFSIZE);

		float dist = centerBlockPos.x - m_playerPos.x - (BLOCK_HALFSIZE + m_PLAYER_WIDTH);

		if (dist < 0)
		{
			m_velocity.x = 0;
			m_playerPos += glm::vec3(dist, 0, 0);
			std::cout << dist << "CD: X+1\n";
			m_cam.setCamPos(m_playerPos);
		}
	}

	// z+1
	if (adjacentBlocks[8] != BLOCK_NOTHING || adjacentBlocks[9] != BLOCK_NOTHING)
	{
		glm::vec3 bPos = adjacentBlocks[8] != BLOCK_NOTHING ? adjacentBlocks[8] : adjacentBlocks[9];

		glm::vec3 centerBlockPos = glm::vec3(bPos.x + BLOCK_HALFSIZE, bPos.y - BLOCK_HALFSIZE, bPos.z + BLOCK_HALFSIZE);

		float dist = centerBlockPos.z - m_playerPos.z - (BLOCK_HALFSIZE + m_PLAYER_WIDTH);

		if (dist < 0)
		{
			m_velocity.z = 0;
			m_playerPos += glm::vec3(0, 0, dist);
			std::cout << dist << "CD: Z+1\n";
			m_cam.setCamPos(m_playerPos);
		}
	}
}