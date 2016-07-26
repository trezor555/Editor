#include "Creature.h"
#include "EventManager.h"

Creature::Creature(SkinnedMesh* model, unsigned int health)
{
	//EventManager::RegisterEvent("ATTACK", this, std::bind(&BasicMonster::Attack, this, std::placeholders::_1));
}

void Creature::Attack()
{
	float m = 40;
	EventManager::SendOutEvent("DAMAGE", enemyListener,  (Event*)&m);
}

void Creature::SetEnemy(IEventListener* enemyListener)
{
	this->enemyListener = enemyListener;
}
