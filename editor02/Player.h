#ifndef PLAYER_H
#define PLAYER_H

#include "IEventListener.h"

class Player : public IEventListener
{
public:
	Player(unsigned int health, unsigned int dps);
	~Player();
	void ReceiveDamage();
private:
	unsigned int m_Dps;
	unsigned int m_Health;
};

#endif

