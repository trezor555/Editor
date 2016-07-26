#ifndef RAY_H
#define RAY_H
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "BoundingBox.h"

class Ray
{
public:
	glm::vec3 origin;
	glm::vec3 direction;

	
	//bool Intersects(const BoundingSphere& bsphere, float  &i1, float &i2);
};

#endif