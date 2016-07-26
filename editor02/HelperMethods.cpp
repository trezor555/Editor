#include "HelperMethods.h"

void PlainRayIntersection(glm::vec3& intersectionPoint, glm::vec4& plane, glm::dvec3& origin, glm::dvec3& direction)
{
	glm::dvec3 normal;
	float dot, temp;

	normal.x = plane.x;
	normal.y = plane.y;
	normal.z = plane.z;
	dot = glm::dot(normal, direction);

	temp = (plane.w + glm::dot(normal, origin)) / dot;
	intersectionPoint.x = origin.x - temp * direction.x;
	intersectionPoint.y = origin.y - temp * direction.y;
	intersectionPoint.z = origin.z - temp * direction.z;
}
