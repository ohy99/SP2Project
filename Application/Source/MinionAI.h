#ifndef MINIONAI_H
#define MINIONAI_H

#include "EnemyAI.h"
#include <string>
#include "MatrixStack.h"
#include <vector>

class MinionAI : public EnemyAI
{
	int minDmg_, maxDmg_, Dmg_;
	int hp_;
	double doHitCD;
	const double doHitDelay;
	bool chasePlayer;
	//bool isHit;
	
public:

	MinionAI();
	virtual ~MinionAI();

	bool active;
	bool test = false;

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