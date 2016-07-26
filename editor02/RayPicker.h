#ifndef RAYPICKER_H
#define RAYPICKER_H
#include "Ray.h"
class RayPicker
{
public:
	void CalculateRays(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight);
	bool Intersects(const BoundingBox& bbox, glm::vec3& intersection, float& tmin);
	//bool Intersects(const BoundingBox& bbox);
	Ray GetRay();
private:
	Ray ray;

};
#endif