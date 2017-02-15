#ifndef _ENEMYAI_H
#define _ENEMYAI_H

#include "NPC.h"
#include "Vector3.h"

class EnemyAI : public NPC
{
	enum botParts
	{
		body,
		lhorn,
		rhorn,
		botParts_count
	};
	enum botSTATES
	{
		Idle,
		Attack,
		PathFind,

		botSTATES_Count,
	};
	Vector3 pos_;
	Mesh* mesh[botParts_count];
	int hp_;
	int damage_;

public:
	EnemyAI(Vector3 pos);
	virtual ~EnemyAI();
	virtual void update(double dt);
	virtual void render();
};

#endif