#include "RotationTool.h"
#include "HelperMethods.h"
void RotationTool::Init()
{
	GLfloat vertices[] =
	{
		0, 0, 0, 1.0f, 0.0f, 0.0f,
		5, 0, 0, 1.0f, 0.0f, 0.0f,

		0, 0, 0, 0.0f, 1.0f, 0.0f,
		0, 5, 0, 0.0f, 1.0f, 0.0f,

		0, 0, 0, 0.0f, 0.0f, 1.0f,
		0, 0, 5, 0.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	m_Treshold = 10 * (100 * 0.01);
	m_bVisible = false;
}

void RotationTool::Draw()
{
	if (!m_bVisible)
		return;

	glLineWidth(5.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
	glLineWidth(1.0f);
}

void RotationTool::SetPosition(glm::vec3 pos)
{
	m_position = pos;

}

glm::vec3 RotationTool::GetPosition()
{
	return m_position;
}

void RotationTool::Update(glm::vec3& cameraPos)
{
	if (!m_bVisible)
		return;

	m_cameraPos = cameraPos;
	double fov = 3.14f / 4.0;  //suppose 45 degrees FOV
	glm::vec3 pos = glm::vec3(m_position);
	double cameraObjectDistance = glm::length(cameraPos - pos);
	double worldSize1 = (2 * tan(fov / 2.0)) * cameraObjectDistance;
	double sssize = 0.25f * worldSize1;
	m_scale = glm::vec3(sssize / 3, sssize / 3, sssize / 3);
	m_Treshold = 5 * (sssize*0.01);
}

glm::vec3 RotationTool::GetScale()
{
	return m_scale;
}

double RotationTool::rayToLineSegment(glm::dvec3& rayOrigin, glm::dvec3& rayVec, const glm::dvec3& lineStart, glm::dvec3& lineEnd)
{
	double epsilon = 0.00000001;
	glm::dvec3 u = rayVec;
	glm::dvec3 v = lineEnd - lineStart;
	glm::dvec3 w = rayOrigin - lineStart;

	double a = glm::dot(u, u);	// always >= 0
	double b = glm::dot(u, v);
	double c = glm::dot(v, v);	// always >= 0
	double d = glm::dot(u, w);
	double e = glm::dot(v, w);
	double D = a*c - b*b;	// always >= 0
	double sc, sN, sD = D;	// sc = sN / sD, default sD = D >= 0
	double tc, tN, tD = D;	// tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (D < epsilon)
	{	// the lines are almost parallel
		sN = 0.0;			// force using point P0 on segment S1
		sD = 1.0;			// to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else
	{				// get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0)
		{	// sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
	}

	if (tN < 0.0)
	{		// tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD)
	{	  // tc > 1 => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else
		{
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < epsilon ? 0.0f : sN / sD);
	tc = (abs(tN) < epsilon ? 0.0f : tN / tD);

	// get the difference of the two closest points
	glm::dvec3 dP = w + (sc * u) - (tc * v);	// = S1(sc) - S2(tc)
	return glm::length(dP);	// return the closest distance
}

void RotationTool::SetRay(Ray& ray)
{
	m_ray = ray;
}

void RotationTool::MousePressDown()
{

}

void RotationTool::MouseMove(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight)
{
	if (!m_bVisible)
		return;

	glm::tvec4<double> viewport = glm::vec4(0, 0, screenwidth, screenheight);
	double winX = (double)mouseXPos;
	double winY = viewport[3] - mouseYPos;

	// get point on the 'near' plane (third param is set to 0.0)
	glm::dvec3 start = glm::unProject<double, double>(glm::tvec3<double>(winX, winY, 0.0), view, projection,
		viewport);

	// get point on the 'far' plane (third param is set to 1.0)
	glm::dvec3 end = glm::unProject<double, double>(glm::dvec3(winX, winY, 1.0), view, projection,
		viewport);

	end = end - start;
	end = glm::normalize(end);

	float distanceFromRay = 999;
	AXIS cAxis = AXIS_NONE;

	float d = rayToLineSegment(m_cameraPos, end, m_position, m_position + (glm::tvec3<double>(5, 0, 0)*m_scale.x));
	dist = d;
	if (d < distanceFromRay)
	{
		distanceFromRay = d;
		cAxis = X_AXIS;
	}

	d = rayToLineSegment(m_cameraPos, end, m_position, m_position + (glm::tvec3<double>(0, 5, 0)*m_scale.x));
	if (d < distanceFromRay)
	{
		distanceFromRay = d;
		cAxis = Y_AXIS;
	}


	d = rayToLineSegment(m_cameraPos, end, m_position, m_position + (glm::tvec3<double>(0, 0, 5)*m_scale.x));
	if (d < distanceFromRay)
	{
		distanceFromRay = d;
		cAxis = Z_AXIS;
	}

	if (distanceFromRay < m_Treshold)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (cAxis == X_AXIS)
		{
			ReColorAxis(0, 1, 1, 1, 0);
		}
		else
		{
			ReColorAxis(0, 1, 1, 0, 0);
		}

		if (cAxis == Y_AXIS)
		{
			ReColorAxis(2, 3, 1, 1, 0);
		}
		else
		{
			ReColorAxis(2, 3, 0, 1, 0);
		}

		if (cAxis == Z_AXIS)
		{
			ReColorAxis(4, 5, 1, 1, 0);
		}
		else
		{
			ReColorAxis(4, 5, 0, 0, 1);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	else
	{
		/*glBindBuffer(GL_ARRAY_BUFFER, VBO);
		ReColorAxis(0, 1, 1, 0, 0);
		ReColorAxis(2, 3, 0, 1, 0);
		ReColorAxis(4, 5, 0, 0, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	}


	if (m_currentAxis == X_AXIS)
	{
		glm::dvec3 planeNormal = glm::vec3(0, 1, 0);
		/*if (0 > glm::dot(m_cameraPos, planeNormal))
		{
		planeNormal = glm::vec3(0, -1, 0);
		}*/


		glm::vec4 plane = glm::vec4(planeNormal, -m_position.y);
		glm::vec3 intersectionPoint;
		double distanceFromPlane;
		PlainRayIntersection(intersectionPoint, plane, start, end);


		glm::dvec3 mousePosOnPlain = intersectionPoint;

		this->m_position.x += mousePosOnPlain.x - m_FirstClickedPosition.x;
		m_FirstClickedPosition = mousePosOnPlain;
	}

	if (m_currentAxis == Y_AXIS)
	{
		glm::dvec3 planeNormal = glm::dvec3(0, 0, 1);
		dot = glm::dot(glm::normalize(m_cameraPos), planeNormal);
		if (dot < 0)
		{
			//planeNormal = glm::dvec3(0, 0, -1);
		}
		dot = glm::dot(glm::normalize(m_cameraPos), planeNormal);


		glm::vec4 plane = glm::vec4(planeNormal, -m_position.z);
		glm::vec3 intersectionPoint;
		double distanceFromPlane;
		PlainRayIntersection(intersectionPoint, plane, start, end);

		glm::dvec3 mousePosOnPlain = intersectionPoint;

		this->m_position.y += mousePosOnPlain.y - m_FirstClickedPosition.y;
		m_FirstClickedPosition = mousePosOnPlain;
	}

	if (m_currentAxis == Z_AXIS)
	{
		glm::dvec3 planeNormal = glm::dvec3(0, 1, 0);
		dot = glm::dot(glm::normalize(m_cameraPos), planeNormal);
		if (dot < 0)
		{
			//planeNormal = glm::dvec3(0, -1, 0);
		}
		glm::vec4 plane = glm::vec4(planeNormal, -m_position.y);
		glm::vec3 intersectionPoint;
		double distanceFromPlane;
		PlainRayIntersection(intersectionPoint, plane, start, end);

		glm::dvec3 mousePosOnPlain = intersectionPoint;

		this->m_position.z += mousePosOnPlain.z - m_FirstClickedPosition.z;
		m_FirstClickedPosition = mousePosOnPlain;
	}

}

void RotationTool::ReColorAxis(int id_1, int id_2, float color_1, float color_2, float color_3)
{
	Vertex* PositionBuffer = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	PositionBuffer[id_1].color = glm::vec3(color_1, color_2, color_3);
	PositionBuffer[id_2].color = glm::vec3(color_1, color_2, color_3);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

bool RotationTool::RayToLineIsInBounds(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight)
{
	glm::tvec4<double> viewport = glm::vec4(0, 0, screenwidth, screenheight);
	double winX = (double)mouseXPos;
	double winY = viewport[3] - mouseYPos;

	// get point on the 'near' plane (third param is set to 0.0)
	glm::dvec3 start = glm::unProject<double, double>(glm::tvec3<double>(winX, winY, 0.0), view, projection,
		viewport);

	// get point on the 'far' plane (third param is set to 1.0)
	glm::dvec3 end = glm::unProject<double, double>(glm::dvec3(winX, winY, 1.0), view, projection,
		viewport);
	end = end - start;

	end = glm::normalize(end);

	float distanceFromRay = 999;
	AXIS cAxis = AXIS_NONE;

	float d = rayToLineSegment(start, end, m_position, m_position + (glm::tvec3<double>(5, 0, 0)*m_scale.x));
	if (d < distanceFromRay)
	{
		distanceFromRay = d;
		cAxis = X_AXIS;
	}


	d = rayToLineSegment(start, end, m_position, m_position + (glm::tvec3<double>(0, 5, 0)*m_scale.x));
	if (d < distanceFromRay)
	{
		distanceFromRay = d;
		cAxis = Y_AXIS;
	}


	d = rayToLineSegment(start, end, m_position, m_position + (glm::tvec3<double>(0, 0, 5)*m_scale.x));
	if (d < distanceFromRay)
	{
		distanceFromRay = d;
		cAxis = Z_AXIS;
	}

	if (distanceFromRay < m_Treshold)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (cAxis == X_AXIS)
		{
			m_currentAxis = X_AXIS;
		}

		if (cAxis == Y_AXIS)
		{
			m_currentAxis = Y_AXIS;
		}

		if (cAxis == Z_AXIS)
		{
			m_currentAxis = Z_AXIS;
		}

	}
	else
	{
		m_currentAxis = AXIS_NONE;
	}

	if (m_currentAxis == X_AXIS)
	{
		glm::dvec3 planeNormal = glm::dvec3(0, 1, 0);
		if (0 > glm::dot(m_cameraPos, planeNormal))
		{
			//planeNormal = glm::dvec3(0, -1, 0);
		}
		glm::vec4 plane = glm::vec4(planeNormal, -m_position.y);
		glm::vec3 intersectionPoint;
		double distanceFromPlane;
		PlainRayIntersection(intersectionPoint, plane, start, end);


		m_FirstClickedPosition = intersectionPoint;
		return true;

	}

	if (m_currentAxis == Y_AXIS)
	{

		glm::dvec3 planeNormal = glm::dvec3(0, 0, 1);
		dot = glm::dot(glm::normalize(m_cameraPos), planeNormal);
		if (dot < 0)
		{
			//planeNormal = glm::dvec3(0, 0, -1);
		}
		glm::vec4 plane = glm::vec4(planeNormal, -m_position.z);
		glm::vec3 intersectionPoint;
		double distanceFromPlane;
		PlainRayIntersection(intersectionPoint, plane, start, end);


		m_FirstClickedPosition = intersectionPoint;
		return true;
	}

	if (m_currentAxis == Z_AXIS)
	{

		glm::dvec3 planeNormal = glm::dvec3(0, 1, 0);
		dot = glm::dot(glm::normalize(m_cameraPos), planeNormal);
		if (dot < 0)
		{
			planeNormal = glm::dvec3(0, -1, 0);
		}


		glm::vec4 plane = glm::vec4(planeNormal, -m_position.y);
		glm::vec3 intersectionPoint;
		double distanceFromPlane;
		PlainRayIntersection(intersectionPoint, plane, start, end);

		m_FirstClickedPosition = intersectionPoint;
		return true;

	}

	return false;
}

void RotationTool::LeftMouseButtonReleased()
{
	m_currentAxis = AXIS_NONE;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	ReColorAxis(0, 1, 1, 0, 0);
	ReColorAxis(2, 3, 0, 1, 0);
	ReColorAxis(4, 5, 0, 0, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RotationTool::SetVisible(bool visible)
{
	m_bVisible = visible;
}

bool RotationTool::GetVisible()
{
	return m_bVisible;
}
