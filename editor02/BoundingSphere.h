#ifndef BOUNDING_SPHERE_H
#define BOUNDING_SPHERE_H

#include <glm.hpp>

struct BoundingSphere
{
	glm::vec3 center;
	float radius;
};

#endif