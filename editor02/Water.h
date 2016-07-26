#ifndef WATER_H
#define WATER_H
#define GLEW_STATIC
#include "Shader.h"
#include "Texture2D.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>

class Water
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texcoord;
	};

	Water();
	void Setup(unsigned int terrainSize, unsigned int cellWidth);
	void Draw(glm::mat4& view, glm::mat4& proj, GLuint skyboxID, glm::vec3& camerapos, float deltatime, int sec);
	void Resize(unsigned int terrainSize, unsigned int cellWidth);

	void SetWaterLevel(int waterLevel);
	void SetWaterRedColor(unsigned int val);
	void SetWaterGreenColor(unsigned int val);
	void SetWaterBlueColor(unsigned int val);
	void SetSpecularPower(float val);
	void SetOpacity(float val);
	void SetDefaultValues();

	int GetWaterLevel() const;
	float GetWaterRedColor() const;
	float GetWaterGreenColor() const;
	float GetWaterBlueColor() const;
	float GetSpecularPower() const;
	float GetOpacity() const;
	
private:
	GLuint VBO, VAO, EBO;

	GLint worldLoc;
	GLint viewLoc;
	GLint projectionLoc;
	GLint cameraLoc;
	GLint textureLerpLoc;
	GLint timeLoc;
	Shader shader;
	glm::mat4 world;
	float accumulator;
	Texture2D normalMaps[4];
	GLuint t1Location;
	GLuint t2Location;
	GLuint t3Location;
	GLuint t4Location;

	GLuint RColorLocation;
	GLuint GColorLocation;
	GLuint BColorLocation;

	GLuint SpecularPowerLocation;
	GLuint OpacityLocation;

	float m_WaterLevel;
	int m_sign;

	float m_WaterRColor;
	float m_WaterGColor;
	float m_WaterBColor;

	float m_SpecularPower;
	float m_Opacity;
};

#endif