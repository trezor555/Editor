#ifndef STATIC_MESH_SUBSET_H
#define STATIC_MESH_SUBSET_H
#define GLEW_STATIC
#include <glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "Shader.h"
#include "TextureManager.h"

#include "VertexDataTypes.h"


class MeshSubset
{
public:
	void Draw();
	void Setup(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
	void Clear();
	void AddTexture(Texture tex);
	
private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	std::vector<Texture> m_textures;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;
};

#endif