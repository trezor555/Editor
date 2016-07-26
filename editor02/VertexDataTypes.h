#ifndef VERTEX_DATA_TYPES_H
#define VERTEX_DATA_TYPES_H
#include <glm.hpp>
struct Vertex
{
	glm::vec3 m_pos;
	glm::vec3  m_normal;
	glm::vec2 m_tex;
	glm::vec3  m_tangent;
	glm::vec3  m_binormal;

	Vertex() {}

	Vertex(const glm::vec3 pos, const glm::vec2 tex, const glm::vec3 normal, const glm::vec3 tangent, const glm::vec3 binormal)
	{
		m_pos = pos;
		m_normal = normal;
		m_tex = tex;
		m_tangent = tangent;
		m_binormal = binormal;
	}
};

struct ObjectData
{
	std::string name;
	unsigned int id;
	bool Visible;
};

struct ModelData
{
	std::string path;
	glm::vec3 pos;
	glm::vec3 rot;
};

#endif