#ifndef _BOSSAI_H
#define _BOSSAI_H

#include "EnemyAI.h"
#include <vector>
#include "Projectile.h"

#include "BossAttackStyles.h"
#include "BossShootProj.h"
#include "BossGSmash.h"
#include "BossDropGoat.h"
#include "BossFlank.h"

#include "MatrixStack.h"

#include "GoatMinion.h"

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
	static enum BOSSHPSTATUS
	{
		BHP_FULL,
		BHP_BELOW100,
		BHP_BELOW75,
		BHP_BELOW50,
		BHP_BELOW25,
		BHP_DEAD
	};
	botSTATES currState_;
	Vector3 pos_;

	//HP VALUE
	int hp_ = 5000;
	const int maxHp_;
	int damage_;
	Mesh* mesh[botParts_count];
	BossAttack* attack;


	double animTime;
	bool attReturn;
	bool playPartAnim = false;
	double attackStateCD;
	double GroundSmashCD, ShootProjCD, DropGoatCD;

	static std::vector<Projectile*> bossProj_;

	static GoatBoss* instance;
	GoatBoss();
public:

	static Mesh* AOESmash;
	static double particleAnimTime;
	static Projectile* projMesh;
	static Vector3 dirBossToPlayer;
	static double BossProjSpeed;
	static Mesh* goatMinionMesh;

	void isHitUpdate(int dmg);
	static GoatBoss* getInstance();
	~GoatBoss();
	void update(double dt);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	static void addProjectiles(Projectile*);
	void updateProjectiles(double dt);
	void renderProjectiles(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

	GoatMinion* goatMinionPool[10];
	GoatMinion* getInactiveGoatMinion();
	Projectile* projectilePool[10];
	Projectile* getInactiveProjectile();
	Mesh* TeleParticles[4];
	//GoatMinion* getMinionPool();

	void moveToRandomLocation(double dt);

	void resetState();

	static BOSSHPSTATUS Bhp_status;
	void updateHpStatus();
};

#endif