#ifndef CREATURE_H
#define CREATURE_H

#include "SkinnedMesh.h"
#include "IEventListener.h"

class Creature : public IEventListener
{
public:
	enum STATE
	{
		NORMAL,
		ATTACK
	};

	Creature(SkinnedMesh* model, unsigned int health);
	void Attack();
	void SetEnemy(IEventListener* enemyListener);
	bool IsAlive() { return m_bAlive; };
private:
	unsigned int m_iHealth;
	SkinnedMesh* m_pModel;
	IEventListener* enemyListener;
	glm::vec3 m_Position;
	bool m_bAlive;
};

#endif