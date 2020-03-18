#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <SDL.h>
#include <chrono>
class Camera {
public:
	//���湹��
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup);
	//ŷ���ǹ���
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup);

	glm::vec3 Position;//�����λ��
	glm::vec3 Forward;//���������
	glm::vec3 Right;//����
	glm::vec3 Up;//����
	glm::vec3 WorldUp;//��������
	float Pitch;//ŷ��_������
	float Yaw;//ŷ��_ƫ����
	float SenceX = 0.001f;//����ƶ�X����
	float SenceY = 0.001f;//����ƶ�Y����
	float speedX = 0.0f;//����X�ƶ�����
	float speedY = 0.0f;//����Y�ƶ�����
	float speedZ = 0.0f;//����Z�ƶ�����
	//��ȡ�۲����
	glm::mat4 GetViewMatrix();
	//����ƶ�
	void process_mouse(double xPos, double yPos);
	void ProcessMouseMovement(float deltaX, float deltaY);
	//���������λ��
	void UpdataCameraPosition();
	void process_keyborad(SDL_Event g_event);
	//������Ұ
	float fov = 45.0f;
	void process_wheel(SDL_Event g_event);
	float wheel_y;
private:
	//����������Ƕ�
	void UpdataCameraVectors();
	//����ƶ�λ�ü�¼
	float lastX = 800 / 2, lastY = 600 / 2;

	bool firstMouse = true;
};