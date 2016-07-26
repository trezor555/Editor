#ifndef TERRAIN_H
#define TERRAIN_H
#define GLEW_STATIC
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <vector>
#include "Shader.h"

class Terrain
{
public:
	Terrain() {};
	Terrain(int numCols, int numRows, float distanceX, float distanceZ, int offX, int offY);
	void Setup();
	void Draw(Shader& shader);
	void SetPosition(glm::vec3 Position);
	void SetLayerMap(GLuint tx);
	void SetHeightMap(char* heightMap);
	glm::tvec3<double> Terrain::GetMousePos(int mouseX, int mouseY, glm::dmat4 view, glm::dmat4 projection);
	void Edit2(glm::tvec3<double> ret, GLuint hmapID = -1);
	GLuint getTex();
	void SetVAO(GLuint vao);
	void SetupData();
	GLuint GetVAO();
private:

	int TerrainWidth;
	int TerrainDepth;
	char* HeightMapName;
	char* LayerName;
	bool LoadTerrainLayer(const char* Path, GLuint& layerTex);
	bool LoadTerrainHeightMap(const char* Path, GLuint& heightmapId);

	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texcoord;
	};

	std::vector<Vertex> m_terrainVertices;
	std::vector<GLuint> m_terrainIndices;

	unsigned int m_NumCols;
	unsigned int m_NumRows;
	float m_distanceX;
	float m_distanceZ;

	unsigned int m_VertexCount;
	unsigned int m_numTriangles;

	GLuint EBO;
	GLuint VBO;
	GLuint VAO;

	GLint worldLoc;
	GLint viewLoc;
	GLint projectionLoc;

	glm::mat4 worldM;

	GLuint TexLayer1;
	GLuint terrinHeightMap;

	Shader terrainShader;
	int offsetX;
	int offsetY;
};

#endif