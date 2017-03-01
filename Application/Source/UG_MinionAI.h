#ifndef UG_MINIONAI_H
#define UG_MINIONAI_H

#include "EnemyAI.h"
#include <string>
#include "MatrixStack.h"
#include <vector>

class UG_MinionAI : public EnemyAI
{
	int minDmg_, maxDmg_;
	int hp_;
	double doHitCD;
	const double doHitDelay;
	bool chasePlayer;
	//bool isHit;

public:

	UG_MinionAI();
	virtual ~UG_MinionAI();

	bool active;

	virtual void update(double dt);
	virtual void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	virtual void isHitUpdate(int dmg);
	virtual void setMoveSpd(float in) { moveSpd = in; }
	virtual void setDmgMultipler(float in) { dmgMultiplier = in; }
	virtual void setAttSpd(float in) { attSpdDelayMultiplier = in; }

	virtual int getHp();
	void runToPlayer(double dt);
	void resetMinion();

	int getDmg();

	std::vector<GameObject*> Minion_CollisionObj;

	double deadTime;

};

#endif