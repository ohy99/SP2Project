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

	botSTATES currState_;
	Vector3 pos_;

	//HP VALUE
	int hp_ = 5000;
	const int maxHp_;
	int MinProjDmg_, MaxProjDmg_;
	int minGsDmg_, maxGsDmg_;
	Mesh* mesh[botParts_count];
	BossAttack* attack;
	float resistance_ = 0.5f;
	bool hasSecondWind;

	double animTime;
	bool attReturn;
	bool playPartAnim = false;
	double attackStateCD;
	double GroundSmashCD, ShootProjCD, DropGoatCD;

	static GoatBoss* instance;
	GoatBoss();
public:

	static Mesh* AOESmash;
	static double particleAnimTime;
	static Vector3 dirBossToPlayer;
	static double BossProjSpeed;

	void resetBoss();
	void isHitUpdate(int dmg);
	static GoatBoss* getInstance();
	~GoatBoss();
	void update(double dt);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	int getHp() { return hp_; }
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

	static enum BOSSHPSTATUS
	{
		BHP_FULL = 0,
		BHP_BELOW100,
		BHP_BELOW75,
		BHP_BELOW50,
		BHP_BELOW25,
		BHP_DEAD
	};
	static BOSSHPSTATUS Bhp_status;
	void updateHpStatus();

	inline int getProjectileDmg();
	inline int getGSDmg();
};

#endif