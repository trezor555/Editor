#include "BoundingBox.h"
#include <vector>

BoundingBox::BoundingBox(glm::vec3 minimum, glm::vec3 maximum, glm::vec3 position /*= glm::vec3(0.0f)*/)
{
	this->min = minimum;
	this->max = maximum;

	this->position = position;
}

BoundingBox::BoundingBox()
{
	min = glm::vec3(99999, 99999, 99999);
	max = glm::vec3(-99999, -99999, -99999);
}



glm::vec3 BoundingBox::getPositiveVertex(const glm::vec3& normal) const
{
	glm::vec3 positiveVertex = min;

	if (normal.x >= 0.0f) positiveVertex.x = max.x;
	if (normal.y >= 0.0f) positiveVertex.y = max.y;
	if (normal.z >= 0.0f) positiveVertex.z = max.z;

	return position + positiveVertex;
}

glm::vec3 BoundingBox::getNegativeVertex(const glm::vec3& normal) const
{
	glm::vec3 negativeVertex = max;

	if (normal.x >= 0.0f) negativeVertex.x = min.x;
	if (normal.y >= 0.0f) negativeVertex.y = min.y;
	if (normal.z >= 0.0f) negativeVertex.z = min.z;

	return position + negativeVertex;
}

void BoundingBox::Init()
{
	std::vector<Vertex> tempvertices;
	glm::vec3 n1;
	glm::vec3 n2;

	glm::vec3 n3;
	glm::vec3 n4;

	glm::vec3 n5;
	glm::vec3 n6;

	glm::vec3 n7;
	glm::vec3 n8;

	glm::vec3 n9;
	glm::vec3 n10;

	glm::vec3 n11;
	glm::vec3 n12;

	glm::vec3 n13;
	glm::vec3 n14;

	glm::vec3 n15;
	glm::vec3 n16;

	glm::vec3 n17;
	glm::vec3 n18;

	glm::vec3 n19;
	glm::vec3 n20;

	glm::vec3 n21;
	glm::vec3 n22;

	glm::vec3 n23;
	glm::vec3 n24;

	

	n1.x = min.x;
	n1.y = max.y;
	n1.z = max.z;

	n2.x = max.x;
	n2.y = max.y;
	n2.z = max.z;

	n3 = n2;
	n4.x = max.x;
	n4.y = max.y;
	n4.z = min.z;

	n5 = n4;
	n6.x = min.x;
	n6.y = max.y;
	n6.z = min.z;

	n7 = n6;
	n8 = n1;

	n9 = n1;
	n10.x = min.x;
	n10.y = min.y;
	n10.z = max.z;

	n11 = n2;
	n12.x = max.x;
	n12.y = min.y;
	n12.z = max.z;

	n13 = n4;
	n14.x = max.x;
	n14.y = min.y;
	n14.z = min.z;

	n15 = n6;
	n16.x = min.x;
	n16.y = min.y;
	n16.z = min.z;

	n17 = n10;
	n18 = n12;

	n19 = n18;
	n20 = n14;

	n21 = n20;
	n22 = n16;

	n23 = n22;
	n24 = n10;
	tempvertices.push_back(Vertex(n1, glm::vec3(1,1,0)));
	tempvertices.push_back(Vertex(n2, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n3, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n4, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n5, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n6, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n7, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n8, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n9, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n10, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n11, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n12, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n13, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n14, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n15, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n16, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n17, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n18, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n19, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n20, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n21, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n22, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n23, glm::vec3(1, 1, 0)));
	tempvertices.push_back(Vertex(n24, glm::vec3(1, 1, 0)));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, tempvertices.size()*sizeof(Vertex), &tempvertices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void BoundingBox::Draw()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
}

void BoundingBox::CreateFromPoints(const glm::vec3* points, int count, unsigned int stride)
{
	int step = (stride - sizeof(glm::vec3)) / sizeof(float);

	float* ptr = (float*)points;
	for (int i = 0; i < count; i++)
	{
		if (*ptr > max.x)
		{
			max.x = *ptr;
		}

		if (*ptr  < min.x)
		{
			min.x = *ptr;
		}

		++ptr;

		if (*ptr > max.y)
		{
			max.y = *ptr;
		}

		if (*ptr < min.y)
		{
			min.y = *ptr;
		}


		++ptr;

		if (*ptr> max.z)
		{
			max.z = *ptr;
		}

		if (*ptr < min.z)
		{
			min.z = *ptr;
		}

		if (step != 0)
		{
			ptr += step+1;
		}
		else
			++ptr;

	}
}

