#ifndef FPSCAMERA_H
#define FPSCAMERA_H
#define GLEW_STATIC
#include <glm.hpp>
#include <GL/glew.h>
#include "BoundingBox.h"
class FPSCamera
{
public:
	FPSCamera(float ScreenWidth, float ScreenHeight);
	FPSCamera(float ScreenWidth, float ScreenHeight, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp);
	void Update(float deltaTime);
	void MouseMove(double xpos, double ypos);
	void KeyboardMove(bool* keys, float deltaTime);
	float GetCameraSpeed();
	void SetCameraSpeed(float val);
	glm::mat4 GetView();
	glm::vec3 GetPosition();
	void CreatePerspectiveProjection(int screenWidth, int screentHeight, float fov, float nearPlane, float farPlane);
	glm::mat4& GetProjectionMatrix();
	BoundingBox bbox;
	glm::mat4 m_ProjectionMatrix;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	GLfloat yaw;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	GLfloat pitch;
	GLfloat lastX;
	GLfloat lastY;
	bool firstMouse;
	glm::mat4 view;
	void SetScreenSize(int width, int height);
	float m_cameraSpeed;
};

#endif