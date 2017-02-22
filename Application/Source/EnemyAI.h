#ifndef _ENEMYAI_H
#define _ENEMYAI_H

#include "NPC.h"
#include "Vector3.h"
#include <string>

class EnemyAI abstract : public NPC
{

public:
	EnemyAI(const std::string& name) : NPC(name) {}
	virtual ~EnemyAI() {}
	virtual void update(double dt) {}
	virtual void render() {}
	virtual void isHitUpdate(int dmg) {};
};

#endif