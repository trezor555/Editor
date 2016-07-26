#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#define GLEW_STATIC

#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


class BoundingBox
{
public:
	BoundingBox();

	BoundingBox(glm::vec3 minimum, glm::vec3 maximum, glm::vec3 position = glm::vec3(0.0f));

	glm::vec3 getPositiveVertex(const glm::vec3 &normal) const;
	glm::vec3 getNegativeVertex(const glm::vec3 &normal) const;

	void Init();
	void Draw();
	void CreateFromPoints(const glm::vec3* points, int count, unsigned int stride);

	struct Vertex
	{
		Vertex(glm::vec3 position, glm::vec3 col)
		{
			this->pos = position;
			this->color = col;
		}

		glm::vec3 pos;
		glm::vec3 color;
	};

	GLuint VAO;
	GLuint VBO;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 position;
	//TestResult testIntersection(BoundingSphere& sphere) const;
};

#endif 
