#ifndef _BOSSAI_H
#define _BOSSAI_H

#include "EnemyAI.h"
#include <vector>
#include "Projectile.h"

#include "BossAttackStyles.h"
#include "BossShootProj.h"
#include "BossGSmash.h"

#include "MatrixStack.h"

class GoatBoss : public EnemyAI
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
		BS_IDLE,
		BS_ATTACK,
		BS_PATHFIND,

		botSTATES_Count,
	};
	botSTATES currState_;
	Vector3 pos_;

	//HP VALUE
	int hp_ = 100;
	const int maxHp_;
	int damage_;
	Mesh* mesh[botParts_count];
	BossAttack* attack;


	double animTime;
	bool attReturn;
	bool playPartAnim = false;

	static std::vector<Projectile*> bossProj_;
	static GoatBoss* instance;
	GoatBoss();
public:

	static Mesh* AOESmash;
	static double particleAnimTime;
	static Projectile* projMesh;
	static Vector3 dirBossToPlayer;
	static double BossProjSpeed;

	void isHitUpdate(int dmg);
	static GoatBoss* getInstance();
	~GoatBoss();
	void update(double dt);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	static void addProjectiles(Projectile*);
	void updateProjectiles(double dt);
	void renderProjectiles(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	void resetState();
};

#endif