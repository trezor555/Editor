#ifndef AI_PATH_H
#define AI_PATH_H

#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//#include "ISelectable.h"

class AIPath //: public ISelectable
{

public:
	AIPath() { m_bVisible = true;  m_bClosed = true;  };
	void AddWaypoint(glm::vec3 waypoint);
	std::vector<glm::vec3>& GetWaypoints();
	
	std::string GetName() const;
	void SetName(std::string name);
	bool IsClosed() const;
	bool IsVisible() const;
	void SetClosed(bool closed);
	void SetVisible(bool visible);
	unsigned int GetID() const;
	void SetID(unsigned int id);

private:
	bool m_bVisible;
	bool m_bClosed;
	std::string m_szName;
	unsigned int m_uID;
	std::vector<glm::vec3> m_WayPoints;
};

#endif