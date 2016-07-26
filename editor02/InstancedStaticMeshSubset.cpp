#include "InstancedStaticMeshSubset.h"


#define WORLD_LOCATION 5

void InstancedMeshSubset::Setup(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
	m_NumInstances = 0;
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glGenBuffers(1, &InstanceDataBuffer);

	glBindVertexArray(this->VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_tex));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_binormal));

	glBindBuffer(GL_ARRAY_BUFFER, InstanceDataBuffer);

	for (unsigned int i = 0; i < 4; i++) 
	{
		glEnableVertexAttribArray(WORLD_LOCATION + i);
		glVertexAttribPointer(WORLD_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(WORLD_LOCATION + i, 1);
	}


	glBindVertexArray(0);

	m_NumVertices = vertices.size();
	m_NumIndices = indices.size();
}

void InstancedMeshSubset::Clear()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void InstancedMeshSubset::Draw(unsigned int numInstances, std::vector<glm::mat4>& positions)
{
	if (m_textures.size() > 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textures[0].textureID);
	}

	if (m_NumInstances != numInstances)
	{
		glBindBuffer(GL_ARRAY_BUFFER, InstanceDataBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numInstances, &positions[0], GL_DYNAMIC_DRAW);
	}
	

	glBindVertexArray(this->VAO);
	glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
		m_NumIndices,
		GL_UNSIGNED_INT,
		0,
		numInstances,
		0);
	glBindVertexArray(0);

}

void InstancedMeshSubset::AddTexture(Texture tex)
{
	m_textures.push_back(tex);
}
