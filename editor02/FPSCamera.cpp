#include "FPSCamera.h"
#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <algorithm>
FPSCamera::FPSCamera(float ScreenWidth, float ScreenHeight, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp)
{
	this->cameraPos = cameraPos;
	this->cameraFront = cameraFront;
	this->cameraUp = cameraUp;
	yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	pitch = 0.0f;
	lastX = ScreenWidth / 2.0;
	lastY = ScreenHeight / 2.0;
	firstMouse = true;
	m_cameraSpeed = 1000.0f;
}

FPSCamera::FPSCamera(float ScreenWidth, float ScreenHeight)
{
	cameraPos = glm::vec3(1500.0f, 1500.0f, 1500.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = 50.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	pitch = 0.0f;
	lastX = ScreenWidth / 2.0;
	lastY = ScreenHeight / 2.0;
	firstMouse = true;
	m_cameraSpeed = 1000.0f;
	MouseMove(0, 0);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}



void FPSCamera::Update(float deltaTime)
{
	//glm::vec3 sphereCenter = glm::vec3(bbox.minimum.x + (bbox.maximum.x - bbox.minimum.x) / 2, bbox.minimum.y + (bbox.maximum.y - bbox.minimum.y) / 2, bbox.minimum.z + (bbox.maximum.z - bbox.minimum.z) / 2);
	
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);//glm::lookAt(glm::vec3(0, sphereCenter.y, 60), glm::vec3(0, 0, 0), cameraUp);
}

void FPSCamera::MouseMove(double xpos, double ypos)
{
	GLfloat sensitivity = 0.05;	// Change this value to your liking
	
	yaw += xpos*sensitivity;
	pitch += ypos*sensitivity;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void FPSCamera::KeyboardMove(bool* keys, float deltaTime)
{
	
}

glm::mat4 FPSCamera::GetView()
{
	return view;
}

glm::vec3 FPSCamera::GetPosition()
{
	return cameraPos;
}

void FPSCamera::SetScreenSize(int width, int height)
{
	lastX = width / 2.0f;
	lastY = height / 2.0f;
}

float FPSCamera::GetCameraSpeed()
{
	return m_cameraSpeed;
}

void FPSCamera::SetCameraSpeed(float val)
{
	m_cameraSpeed = val;
}

void FPSCamera::CreatePerspectiveProjection(int screenWidth, int screentHeight, float fov, float nearPlane, float farPlane)
{
	m_ProjectionMatrix = glm::perspective(fov, (float)screenWidth / (float)screentHeight, nearPlane, farPlane);
}

glm::mat4& FPSCamera::GetProjectionMatrix()
{
	return m_ProjectionMatrix;
}
