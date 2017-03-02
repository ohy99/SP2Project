#ifndef _ENEMYAI_H
#define _ENEMYAI_H

#include "NPC.h"
#include "Vector3.h"
#include <string>
#include "MatrixStack.h"
#include <vector>


class EnemyAI abstract : public NPC
{

public:
	EnemyAI(const std::string& name) : NPC(name) { moveSpd = 0.5f; dmgMultiplier = 1.0f; attSpdDelayMultiplier = 1.0f; isHit_ = false; }
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
	
	bool isHit_;

	struct DMGTAKEN
	{
		double timeElapsed;
		int dmg;
		float tY;
		float Dscale;
		float tySpd;
	};
	std::vector<DMGTAKEN>dmgTakenVector;
	float showDmgDelay = 0.f;
	int dmgTakenBeforeShown = 0;

	void updateShowDmgTaken(double dt);
	void renderShowDmgTaken(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	void isHitShowDmgTaken(int dmg);
	//bool check = true;
};

#endif