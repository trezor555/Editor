#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

enum TestResult
{
	TEST_OUTSIDE,
	TEST_INTERSECT,
	TEST_INSIDE
};


class Frustum
{
public:

	enum Plane
	{
		PLANE_BACK,
		PLANE_FRONT,
		PLANE_RIGHT,
		PLANE_LEFT,
		PLANE_TOP,
		PLANE_BOTTOM
	};
	void ExtractFrustum(glm::mat4& view, glm::mat4& projection);
	TestResult Intersect(const BoundingBox& box) const;
	TestResult Intersect(const BoundingSphere& box) const;
private:
	glm::vec4 frustumPlanes[6];
	float m_Dist[6];
	void NormalizePlanes();
};

#endif