#ifndef _GOATMINION_H
#define _GOATMINION_H

#include "EnemyAI.h"
#include <string>
#include "MatrixStack.h"
#include <vector>

class GoatMinion : public EnemyAI
{
	int minDmg_, maxDmg_;
	int hp_;
	double doHitCD;
	const double doHitDelay;
public:
	bool active;
	GoatMinion();
	virtual ~GoatMinion();
	virtual void update(double dt);
	virtual void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	virtual void isHitUpdate(int dmg);
	int getDmg();
	virtual int getHp();
	void resetGoat();
	std::vector<GameObject*> CollisionObj;
};


#endif