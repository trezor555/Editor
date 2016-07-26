#ifndef INSTANCED_MESH_SUBSET_H
#define INSTANCED_MESH_SUBSET_H
#define GLEW_STATIC
#include <glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "Shader.h"
#include "TextureManager.h"
#include "VertexDataTypes.h"
class InstancedMeshSubset
{
public:
	void Draw(unsigned int numInstances, std::vector<glm::mat4>& positions);
	void Setup(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
	void Clear();
	void AddTexture(Texture tex);
	
private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint InstanceDataBuffer;
	std::vector<Texture> m_textures;
	unsigned int m_NumVertices;
	unsigned int m_NumIndices;
	unsigned int m_NumInstances;
};

#endif