#include "Frustum.h"

void Frustum::ExtractFrustum(glm::mat4& view, glm::mat4& projection)
{
	const glm::mat4 v = view;
	const glm::mat4 p = projection;

	glm::mat4 clipMatrix;

	clipMatrix[0][0] = v[0][0] * p[0][0] + v[0][1] * p[1][0] + v[0][2] * p[2][0] + v[0][3] * p[3][0];
	clipMatrix[1][0] = v[0][0] * p[0][1] + v[0][1] * p[1][1] + v[0][2] * p[2][1] + v[0][3] * p[3][1];
	clipMatrix[2][0] = v[0][0] * p[0][2] + v[0][1] * p[1][2] + v[0][2] * p[2][2] + v[0][3] * p[3][2];
	clipMatrix[3][0] = v[0][0] * p[0][3] + v[0][1] * p[1][3] + v[0][2] * p[2][3] + v[0][3] * p[3][3];
	clipMatrix[0][1] = v[1][0] * p[0][0] + v[1][1] * p[1][0] + v[1][2] * p[2][0] + v[1][3] * p[3][0];
	clipMatrix[1][1] = v[1][0] * p[0][1] + v[1][1] * p[1][1] + v[1][2] * p[2][1] + v[1][3] * p[3][1];
	clipMatrix[2][1] = v[1][0] * p[0][2] + v[1][1] * p[1][2] + v[1][2] * p[2][2] + v[1][3] * p[3][2];
	clipMatrix[3][1] = v[1][0] * p[0][3] + v[1][1] * p[1][3] + v[1][2] * p[2][3] + v[1][3] * p[3][3];
	clipMatrix[0][2] = v[2][0] * p[0][0] + v[2][1] * p[1][0] + v[2][2] * p[2][0] + v[2][3] * p[3][0];
	clipMatrix[1][2] = v[2][0] * p[0][1] + v[2][1] * p[1][1] + v[2][2] * p[2][1] + v[2][3] * p[3][1];
	clipMatrix[2][2] = v[2][0] * p[0][2] + v[2][1] * p[1][2] + v[2][2] * p[2][2] + v[2][3] * p[3][2];
	clipMatrix[3][2] = v[2][0] * p[0][3] + v[2][1] * p[1][3] + v[2][2] * p[2][3] + v[2][3] * p[3][3];
	clipMatrix[0][3] = v[3][0] * p[0][0] + v[3][1] * p[1][0] + v[3][2] * p[2][0] + v[3][3] * p[3][0];
	clipMatrix[1][3] = v[3][0] * p[0][1] + v[3][1] * p[1][1] + v[3][2] * p[2][1] + v[3][3] * p[3][1];
	clipMatrix[2][3] = v[3][0] * p[0][2] + v[3][1] * p[1][2] + v[3][2] * p[2][2] + v[3][3] * p[3][2];
	clipMatrix[3][3] = v[3][0] * p[0][3] + v[3][1] * p[1][3] + v[3][2] * p[2][3] + v[3][3] * p[3][3];

	frustumPlanes[PLANE_RIGHT].x = clipMatrix[3][0] - clipMatrix[0][0];
	frustumPlanes[PLANE_RIGHT].y = clipMatrix[3][1] - clipMatrix[0][1];
	frustumPlanes[PLANE_RIGHT].z = clipMatrix[3][2] - clipMatrix[0][2];
	frustumPlanes[PLANE_RIGHT].w = clipMatrix[3][3] - clipMatrix[0][3];

	frustumPlanes[PLANE_LEFT].x = clipMatrix[3][0] + clipMatrix[0][0];
	frustumPlanes[PLANE_LEFT].y = clipMatrix[3][1] + clipMatrix[0][1];
	frustumPlanes[PLANE_LEFT].z = clipMatrix[3][2] + clipMatrix[0][2];
	frustumPlanes[PLANE_LEFT].w = clipMatrix[3][3] + clipMatrix[0][3];

	frustumPlanes[PLANE_BOTTOM].x = clipMatrix[3][0] + clipMatrix[1][0];
	frustumPlanes[PLANE_BOTTOM].y = clipMatrix[3][1] + clipMatrix[1][1];
	frustumPlanes[PLANE_BOTTOM].z = clipMatrix[3][2] + clipMatrix[1][2];
	frustumPlanes[PLANE_BOTTOM].w = clipMatrix[3][3] + clipMatrix[1][3];

	frustumPlanes[PLANE_TOP].x = clipMatrix[3][0] - clipMatrix[1][0];
	frustumPlanes[PLANE_TOP].y = clipMatrix[3][1] - clipMatrix[1][1];
	frustumPlanes[PLANE_TOP].z = clipMatrix[3][2] - clipMatrix[1][2];
	frustumPlanes[PLANE_TOP].w = clipMatrix[3][3] - clipMatrix[1][3];

	frustumPlanes[PLANE_BACK].x = clipMatrix[3][0] - clipMatrix[2][0];
	frustumPlanes[PLANE_BACK].y = clipMatrix[3][1] - clipMatrix[2][1];
	frustumPlanes[PLANE_BACK].z = clipMatrix[3][2] - clipMatrix[2][2];
	frustumPlanes[PLANE_BACK].w = clipMatrix[3][3] - clipMatrix[2][3];

	frustumPlanes[PLANE_FRONT].x = clipMatrix[3][0] + clipMatrix[2][0];
	frustumPlanes[PLANE_FRONT].y = clipMatrix[3][1] + clipMatrix[2][1];
	frustumPlanes[PLANE_FRONT].z = clipMatrix[3][2] + clipMatrix[2][2];
	frustumPlanes[PLANE_FRONT].w = clipMatrix[3][3] + clipMatrix[2][3];

	for (int i = 0; i < 6; ++i)
	{
		m_Dist[i] = frustumPlanes[i].w;
		glm::vec3 n = glm::normalize(glm::vec3(frustumPlanes[i]));
		frustumPlanes[i] = glm::normalize(frustumPlanes[i]);

		/*frustumPlanes[i].x = n.x;
		frustumPlanes[i].y = n.y;
		frustumPlanes[i].z = n.z;*/
	}
}

TestResult Frustum::Intersect(const BoundingBox& box) const
{
	TestResult result = TEST_INSIDE;

	for (size_t i = 0; i < 6; i++)
	{
		const float pos = frustumPlanes[i].w;
		const glm::vec3 normal = glm::vec3(frustumPlanes[i]);

		if (glm::dot(normal, box.getPositiveVertex(normal)) + pos < 0.0f)
		{
			return TEST_OUTSIDE;
		}

		if (glm::dot(normal, box.getNegativeVertex(normal)) + pos < 0.0f)
		{
			result = TEST_INTERSECT;
		}
	}

	return result;
}

TestResult Frustum::Intersect(const BoundingSphere& boundingSphere) const
{
	float fDistance = 0;
	for (int i = 0; i < 6; ++i) 
	{
		
		/*float pos = frustumPlanes[i].w;
		glm::vec3 normal = glm::vec3(frustumPlanes[i]);
		glm::vec4 t = glm::vec4(frustumPlanes[i].x, frustumPlanes[i].y, frustumPlanes[i].z, -m_Dist[i]);
		fDistance = glm::dot(t, glm::vec4(boundingSphere.center, 1))+ m_Dist[];
	
		// if this distance is < -sphere.radius, we are outside
		if (fDistance < -boundingSphere.radius)
			return TEST_OUTSIDE;*/

	}

	return TEST_INSIDE;
}
