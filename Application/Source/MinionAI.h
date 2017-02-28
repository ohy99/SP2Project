#ifndef MINIONAI_H
#define MINIONAI_H

#include "EnemyAI.h"
#include <string>
#include "MatrixStack.h"
#include <vector>

class MinionAI : public EnemyAI
{
	int minDmg_, maxDmg_;
	int hp_;
	double doHitCD;
	const double doHitDelay;

public:

	MinionAI();
	virtual ~MinionAI();

	bool active;

	virtual void update(double dt);
	virtual void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	virtual void isHitUpdate(int dmg);

	void resetGoat();

	int getDmg();
	int getHp();

	std::vector<GameObject*> CollisionObj;



};



#endif