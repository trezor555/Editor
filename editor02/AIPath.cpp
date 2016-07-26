#include "AIPath.h"

void AIPath::AddWaypoint(glm::vec3 waypoint)
{
	m_WayPoints.push_back(waypoint);
}

std::vector<glm::vec3>& AIPath::GetWaypoints()
{
	return m_WayPoints;
}

unsigned int AIPath::GetID() const
{
	return m_uID;
}

std::string AIPath::GetName() const
{
	return m_szName;
}

void AIPath::SetName(std::string name)
{
	m_szName = name;
}

bool AIPath::IsClosed() const
{
	return m_bClosed;
}

bool AIPath::IsVisible() const
{
	return m_bVisible;
}

void AIPath::SetVisible(bool visible)
{
	m_bVisible = visible;
}

void AIPath::SetClosed(bool closed)
{
	m_bClosed = closed;
}

void AIPath::SetID(unsigned int id)
{
	m_uID = id;
}

