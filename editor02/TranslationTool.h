#ifndef TRANSLATION_TOOL_H
#define TRANSLATION_TOOL_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Ray.h"
#include "Shader.h"



class TranslationTool
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

	void SetPositionX(float val);
	void SetPositionY(float val);
	void SetPositionZ(float val);

	void SetGridSnapWidth(unsigned int val);
	void SetGridSnapHeight(unsigned int val);
	void SetGridSnapDepth(unsigned int val);

	unsigned int GetGridSnapWidth() const;
	unsigned int GetGridSnapHeight() const;
	unsigned int GetGridSnapDepth() const;

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

	unsigned int m_iGridCubeWidth;
	unsigned int m_iGridCubeHeight;
	unsigned int m_iGridCubeDepth;
};

#endif