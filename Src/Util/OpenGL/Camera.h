#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>

// matrices and vectors
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum CAMERA_TYPES
{
	CAM_FLY,
	CAM_FP
};

enum class CAMERA_MOVEMENT_DIR
{
    FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
};

class Camera
{
public:

	Camera(glm::vec3 camPos, glm::vec3 camFront, float camSpeed, CAMERA_TYPES camType)	: m_camPos(camPos), m_camFront(camFront), m_camUp(glm::vec3(0.0f, 1.0f, 0.0f)), m_camSpeed(camSpeed), m_camType(camType) {}
    Camera() : m_camPos(glm::vec3(0.0f, 120.0f, 0.0f)), m_camFront(glm::vec3(1.0f, 0.0f, 0.0f)), m_camUp(glm::vec3(0.0f, 1.0f, 0.0f)), m_camType(CAMERA_TYPES::CAM_FLY) {}

	void mouseMovement(double xPos, double yPos)
	{
        if (m_firstMouse)
        {
            m_lastX = (float)xPos;
            m_lastY = (float)yPos;
            m_firstMouse = false;
        }

        float xoffset = (float)xPos - m_lastX;
        float yoffset = m_lastY - (float)yPos; // reversed since y-coordinates go from bottom to top
        m_lastX = (float)xPos;
        m_lastY = (float)yPos;

        xoffset *= m_sensitivity;
        yoffset *= m_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_camFront = glm::normalize(front);
	}

	void moveCamera(float xOff, float yOff, float zOff)
	{
        // All inputs are inverted, because when a camera moves up everything moves down by the same amount
        // When a camera moves to the left everything moves to the right

		m_camPos.x += -xOff;
		m_camPos.y += -yOff;
		m_camPos.z += -zOff;
	}

    void moveRelativeToCamFront(CAMERA_MOVEMENT_DIR dir, float deltaTime)
    {
        float speed = m_camSpeed * deltaTime;

        if (m_camType == CAMERA_TYPES::CAM_FLY)
        {
            switch (dir)
            {
            case CAMERA_MOVEMENT_DIR::FORWARD:
                m_camPos += speed * m_camFront;
                break;
            case CAMERA_MOVEMENT_DIR::BACKWARD:
                m_camPos -= speed * m_camFront;
                break;
            case CAMERA_MOVEMENT_DIR::LEFT:
                m_camPos -= speed * glm::normalize(glm::cross(m_camFront, m_camUp));
                break;
            case CAMERA_MOVEMENT_DIR::RIGHT:
                m_camPos += speed * glm::normalize(glm::cross(m_camFront, m_camUp));
                break;
            case CAMERA_MOVEMENT_DIR::UP:
                m_camPos += speed * m_camUp;
                break;
            case CAMERA_MOVEMENT_DIR::DOWN:
                m_camPos -= speed * m_camUp;
                break;
            default:
                break;
            }
        }
        else if (m_camType == CAMERA_TYPES::CAM_FP)
        {
            switch (dir)
            {
            case CAMERA_MOVEMENT_DIR::FORWARD:
                m_camPos += speed * glm::normalize(glm::cross(m_camUp, glm::cross(m_camFront, m_camUp)));
                break;
            case CAMERA_MOVEMENT_DIR::BACKWARD:
                m_camPos -= speed * glm::normalize(glm::cross(m_camUp, glm::cross(m_camFront, m_camUp)));
                break;
            case CAMERA_MOVEMENT_DIR::LEFT:
                m_camPos -= speed * glm::normalize(glm::cross(m_camFront, m_camUp));
                break;
            case CAMERA_MOVEMENT_DIR::RIGHT:
                m_camPos += speed * glm::normalize(glm::cross(m_camFront, m_camUp));
                break;
            case CAMERA_MOVEMENT_DIR::UP:
                m_camPos += speed * m_camUp;
                break;
            case CAMERA_MOVEMENT_DIR::DOWN:
                m_camPos -= speed * m_camUp;
                break;
            default:
                break;
            }
        }

#ifdef _DEBUG
        //std::cout << "Cam-Pos: X: " << m_camPos.x << "  Y: " << m_camPos.y << "  Z: " << m_camPos.z << "\n";
#endif // _DEBUG

        
    }


    void setCamPos(const glm::vec3 pos)
    {
        m_camPos = pos;
    }

    glm::vec3 castRay(const float multi)
    {
        return m_camPos + (multi * m_camFront);
    }

	const glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_camPos, m_camPos + m_camFront, m_camUp);
	}

    const glm::vec3 getCamPos() 
    {
        return m_camPos;
    }

    glm::vec3 getCamFront()
    {
        return m_camFront;
    }

private:

	glm::vec3 m_camPos;
	glm::vec3 m_camFront;
	glm::vec3 m_camUp;

    CAMERA_TYPES m_camType;

    bool m_firstMouse = true;
    float m_yaw = 0.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float m_pitch = 0.0f;
    float m_lastX = 500;
    float m_lastY = 500;
    float m_camSpeed = 10.f;
    float m_sensitivity = 0.5f;

};


#endif // !CAMERA_HPP

