#ifndef ROTATION_TOOL_H
#define ROTATION_TOOL_H

#include <glm.hpp>
#include "Ray.h"
#include "Shader.h"

class RotationTool
{
public:
	void Draw();
	void Update(glm::vec3& cameraPos);
	void Init();
	void MousePressDown();
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition();
	glm::vec3 GetScale();
	void SetRay(Ray& ray);
	void MouseMove(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight);
	float dist;
	void ReColorAxis(int id_1, int id_2, float color_1, float color_2, float color_3);
	bool RayToLineIsInBounds(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight);
	void LeftMouseButtonReleased();
	glm::dvec3 m_FirstClickedPosition;
	float dot;

	void SetVisible(bool visible);
	bool GetVisible();

	enum AXIS
	{
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		AXIS_NONE

	};

private:
	double rayToLineSegment(glm::dvec3& rayOrigin, glm::dvec3& rayVec, const glm::dvec3& lineStart, glm::dvec3& lineEnd);
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};

	GLuint VAO;
	GLuint VBO;
	glm::dvec3 m_position;

	glm::dvec3 m_scale;

	Ray m_ray;
	float m_Treshold;
	AXIS m_currentAxis;
	glm::dvec3 m_cameraPos;
	bool m_MouseDown;
	bool m_bVisible;
};


#endif


