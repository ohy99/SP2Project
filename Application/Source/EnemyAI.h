#ifndef _ENEMYAI_H
#define _ENEMYAI_H

#include "NPC.h"
#include "Vector3.h"
#include <string>
#include "MatrixStack.h"

class EnemyAI abstract : public NPC
{

public:
	EnemyAI(const std::string& name) : NPC(name) { moveSpd = 0.5f; dmgMultiplier = 1.0f; attSpdDelayMultiplier = 1.0f; }
	virtual ~EnemyAI() {}
	virtual void update(double dt) {}
	virtual void render() {}
	virtual void isHitUpdate(int dmg) {};

	virtual void setMoveSpd(float) {}
	virtual void setDmgMultipler(float) {}
	virtual void setAttSpd(float) {}

	virtual int getHp() = 0;
protected:
	float moveSpd;
	float dmgMultiplier;
	float attSpdDelayMultiplier;

	//bool check = true;
};

#endif