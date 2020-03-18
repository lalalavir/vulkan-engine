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
	//常规构造
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup);
	//欧拉角构造
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup);

	glm::vec3 Position;//摄像机位置
	glm::vec3 Forward;//摄像机方向
	glm::vec3 Right;//右轴
	glm::vec3 Up;//上轴
	glm::vec3 WorldUp;//世界上轴
	float Pitch;//欧拉_俯仰角
	float Yaw;//欧拉_偏航角
	float SenceX = 0.001f;//鼠标移动X速率
	float SenceY = 0.001f;//鼠标移动Y速率
	float speedX = 0.0f;//键盘X移动速率
	float speedY = 0.0f;//键盘Y移动速率
	float speedZ = 0.0f;//键盘Z移动速率
	//获取观察矩阵
	glm::mat4 GetViewMatrix();
	//鼠标移动
	void process_mouse(double xPos, double yPos);
	void ProcessMouseMovement(float deltaX, float deltaY);
	//更新摄像机位置
	void UpdataCameraPosition();
	void process_keyborad(SDL_Event g_event);
	//缩放视野
	float fov = 45.0f;
	void process_wheel(SDL_Event g_event);
	float wheel_y;
private:
	//更新摄像机角度
	void UpdataCameraVectors();
	//鼠标移动位置记录
	float lastX = 800 / 2, lastY = 600 / 2;

	bool firstMouse = true;
};