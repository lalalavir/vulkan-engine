#include "camera.h"
//���湹��
Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup)
{
	Position = position;
	WorldUp = worldup;
	Forward = glm::normalize(target - position);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}

//ŷ���ǹ���
Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup)
{
	Position = position;
	WorldUp = worldup;
	Pitch = pitch;
	Yaw = yaw;
	Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}

//��ȡ��ͼ�۲����
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, WorldUp);
}

void Camera::process_mouse(double xPos, double yPos)
{
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	float deltaX, deltaY;
	deltaX = xPos - lastX;
	deltaY = yPos - lastY;
	lastX = xPos;
	lastY = yPos;
	ProcessMouseMovement(deltaX, deltaY);
}

void Camera::process_wheel(SDL_Event g_event)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= g_event.wheel.y-wheel_y ;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

//����������Ƕ�
void Camera::UpdataCameraVectors()
{
	Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}


//����ƶ�
void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
	Pitch += deltaY * SenceX;
	Yaw += deltaX * SenceY;
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;
	UpdataCameraVectors();
}

//���������λ��
void Camera::UpdataCameraPosition()
{	
	Position += Forward * speedZ * 0.01f + Right * speedX * 0.01f + Up * speedY * 0.01f;
}

void Camera::process_keyborad(SDL_Event g_event)
{
	switch (g_event.key.keysym.sym)
	{
	case  SDLK_w:
		speedZ = 3.0f;
		break;
	case SDLK_s:
		speedZ = -3.0f;
		break;
	case SDLK_a:
		speedX = -3.0f;
		break;
	case SDLK_d:
		speedX = 3.0f;
		break;
	case  SDLK_q:
		speedY = 3.0f;
		break;
	case SDLK_r:
		speedY = -3.0f;
	default:
		break;
	}
}

