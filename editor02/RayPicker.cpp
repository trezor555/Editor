#include "RayPicker.h"
#include <limits>
#include <utility>
void RayPicker::CalculateRays(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight)
{
	glm::tvec4<double> viewport = glm::vec4(0, 0, screenwidth, screenheight);
	double winX = (double)mouseXPos;
	double winY = viewport[3] - mouseYPos;

	// get point on the 'near' plane (third param is set to 0.0)
	glm::tvec3<double> start = glm::unProject<double, double>(glm::tvec3<double>(winX, winY, 0.0), view, projection,
		viewport);

	// get point on the 'far' plane (third param is set to 1.0)
	glm::tvec3<double> end = glm::unProject<double, double>(glm::tvec3<double>(winX, winY, 1.0), view, projection,
		viewport);

	end = end - start;
	end = glm::normalize(end);

	ray.direction = end;
	ray.origin = start;
}

bool RayPicker::Intersects(const BoundingBox& bbox, glm::vec3& intersection, float& tmin)
{
	tmin = (bbox.min.x - ray.origin.x) / ray.direction.x;
	float tmax = (bbox.max.x - ray.origin.x) / ray.direction.x;
	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (bbox.min.y - ray.origin.y) / ray.direction.y;
	float tymax = (bbox.max.y - ray.origin.y) / ray.direction.y;
	if (tymin > tymax) std::swap(tymin, tymax);
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	float tzmin = (bbox.min.z - ray.origin.z) / ray.direction.z;
	float tzmax = (bbox.max.z - ray.origin.z) / ray.direction.z;
	if (tzmin > tzmax) std::swap(tzmin, tzmax);
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	if ((tmin > 9999999) || (tmax < 0)) return false;

	intersection = ray.origin + (ray.direction * tmin);
	return true;
}

Ray RayPicker::GetRay()
{
	return ray;
}
