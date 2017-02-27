#include "BossAI.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"
#include "Player.h"
#include "LoadATOM.h"

//temp
#include "Application.h"


GoatBoss* GoatBoss::instance = NULL;
double GoatBoss::BossProjSpeed;
Mesh* GoatBoss::AOESmash = NULL;
Vector3 GoatBoss::dirBossToPlayer;
double GoatBoss::particleAnimTime = 0.0;
GoatBoss::BOSSHPSTATUS GoatBoss::Bhp_status = GoatBoss::BOSSHPSTATUS::BHP_FULL;

GoatBoss::GoatBoss() : EnemyAI("GoatBoss"), maxHp_(hp_)
{
	BossProjSpeed = 10.0;
	mesh[botParts::body] = MeshBuilder::GenerateOBJ("", "OBJ//GoatBossTemp.obj");
	CollisionMesh_ = mesh[botParts::body];
	attack = NULL;
	currState_ = BS_IDLE;
	animTime = 0.0;
	particleAnimTime = 0.0;
	CollisionMesh_->up.Set(0, 1, 0);
	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		goatMinionPool[i] = new GoatMinion;
		goatMinionPool[i]->CollisionMesh_ = MeshBuilder::GenerateOBJ("Goat Minion", "OBJ//goat.obj");
		goatMinionPool[i]->active = false;
		goatMinionPool[i]->CollisionMesh_->collisionEnabled = true;
	}
	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		TeleParticles[i] = MeshBuilder::GenerateOBJ("Particles", "OBJ//BossTeleParticles.obj");
	}

	//Projectile* temp_proj  = new Projectile("", BossProjSpeed, 10);
	//temp_proj->CollisionMesh_ = MeshBuilder::GenerateOBJ("BossProj", "OBJ//goat.obj");
	////temp_proj->setSpeed(BossProjSpeed);
	//projMesh = temp_proj;

	for (size_t i = 0; i < (sizeof projectilePool) / sizeof(*projectilePool); ++i)
	{
		projectilePool[i] = new Projectile("", BossProjSpeed, 10);//projecitle DEAD TIME =======================
		projectilePool[i]->CollisionMesh_ = MeshBuilder::GenerateOBJ("BossProj", "OBJ//goat.obj");
		projectilePool[i]->CollisionMesh_->collisionEnabled = false;
	}

	attackStateCD = 0.0;
	GroundSmashCD = ShootProjCD = DropGoatCD = 0.0;



	AOESmash = MeshBuilder::GenerateOBJ("", "OBJ//GSAOE.obj");

	srand(time(NULL));
}
GoatBoss::~GoatBoss()
{
	delete mesh[botParts::body];
	delete attack;
	delete AOESmash;
	delete[] goatMinionPool;
	delete instance;
	delete[] TeleParticles;
}
GoatBoss* GoatBoss::getInstance()
{
	if (instance)
		return instance;
	else
		return (instance = new GoatBoss);
}
void GoatBoss::update(double dt)
{
	updateHpStatus();
	try{
		dirBossToPlayer = (Player::getInstance()->CollisionMesh_->pos -
		GoatBoss::getInstance()->CollisionMesh_->pos).Normalized();
	}
	catch (DivideByZero what)
	{
		dirBossToPlayer.Set(0, 0, 1);
	}
	CollisionMesh_->dir = dirBossToPlayer;
	CollisionMesh_->right = CollisionMesh_->dir.Cross(CollisionMesh_->up).Normalized();

	static const double AttackCDTime = 0.5;
	if (attackStateCD < AttackCDTime)
		attackStateCD += dt;

	//static bool once = false;
	//static bool playPartAnim = false;
	//static bool playPartAnimOnce = false;

	//animTime is for boss animation

	static const double GroundSmashCDTime = 5;
	static const double ShootProjCDTime = 0;
	static const double DropGoatCDTime = 10;

	//Randomise attack style
	if (currState_ == BS_IDLE && attackStateCD >= AttackCDTime || Application::IsKeyPressed('N'))// && !once)
	{
		//if (hp_ < 0.75 * maxHp_)
		//particleAnimTime = 0.0;
		delete attack;
		attack = NULL;

		if (rand() % 10 <= 8 && (Player::getInstance()->CollisionMesh_->pos -
			GoatBoss::getInstance()->CollisionMesh_->pos).Length() < 5)
		//	attack = new BossFlank;
			attack = new BossGSmash;
		else if (getInactiveGoatMinion() && (rand() % 10 == 9))
			attack = new BossDGoat;
		else if (rand() % 10 <= 6)
			attack = new BossShootProj;
		else
			attack = new BossFlank;

		if (attack)
		{
			currState_ = BS_ATTACK;
			//once = true;
			animTime = 0.0;
			attackStateCD = 0.0;
		}
	}

	//if (Application::IsKeyPressed('N'))
	//{
	//	currState_ = BS_ATTACK;
	//	animTime = 0.0;
	//}
	if (currState_ == BS_ATTACK)
	{
		//execute attack
		if (attack)
		{
			attReturn = attack->performAttack(dt, animTime);
			if (attReturn)
				playPartAnim = true;
		}
	}


	if (playPartAnim)
	{
		particleAnimTime += dt;
	}





	updateProjectiles(dt);


	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		if (goatMinionPool[i]->active)
		{
			if (goatMinionPool[i]->getHp() <= 0)
			{
				goatMinionPool[i]->active = false;
				auto it = std::find(Player::enemies_.begin(), Player::enemies_.end(), goatMinionPool[i]);
				std::swap(*it, Player::enemies_.back());
				Player::enemies_.back() = NULL;
				Player::enemies_.pop_back();
			}
			else
				goatMinionPool[i]->update(dt);
		}
	}
}
void GoatBoss::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	modelStack->PushMatrix();
	modelStack->Translate(CollisionMesh_->pos.x, CollisionMesh_->pos.y, CollisionMesh_->pos.z);
	RenderMeshClass::RenderMesh(CollisionMesh_, true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();

	//if (attReturn && playPartAnim)
	//{
	if (attack)
		attack->renderAttack(projectionStack, viewStack, modelStack, m_parameters);
	//	if (playPartAnim && particleAnimTime == 0.0)
	//		playPartAnim = false;
	//	
	//}

	modelStack->PushMatrix();
	modelStack->Translate(0.15f * CollisionMesh_->right.x, 2.f, (0.15f * CollisionMesh_->right.z));
	modelStack->Translate(CollisionMesh_->pos.x, CollisionMesh_->pos.y, CollisionMesh_->pos.z);//y always 0
	modelStack->Rotate((CollisionMesh_->dir.x < 0 ? -1.0f : 1.0f) * Math::RadianToDegree(acos(CollisionMesh_->dir.Dot(Vector3(0.f, 0.f, 1.f)))), 0.f, 1.f, 0.f);
	modelStack->Scale(0.3f, 0.3f, 0.3f);
	RenderMeshClass::RenderText(&Scene::Text[Scene::TEXT_TYPE::Chiller], std::to_string(hp_), Color(1, 0, 0), projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();
	
	renderProjectiles(projectionStack, viewStack, modelStack, m_parameters);


		for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
		{
			if (goatMinionPool[i]->active)
				goatMinionPool[i]->render(projectionStack, viewStack, modelStack, m_parameters);
		}
}

void GoatBoss::updateProjectiles(double dt)
{


	for (size_t i = 0; i < (sizeof projectilePool) / sizeof(*projectilePool); ++i)
	{
		if (projectilePool[i]->CollisionMesh_->collisionEnabled)
		{
			projectilePool[i]->CollisionMesh_->pos += projectilePool[i]->CollisionMesh_->dir * (float)dt * (float)projectilePool[i]->getSpeed();
			projectilePool[i]->aliveTime += dt;
			if (projectilePool[i]->CollisionMesh_->isCollide(Player::getInstance()->CollisionMesh_))
			{
				Player::getInstance()->isHitUpdate(50);
				projectilePool[i]->CollisionMesh_->collisionEnabled = false;
			}
			for (size_t j = 0; j < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++j)
			{
				if (projectilePool[i]->CollisionMesh_->isCollide(goatMinionPool[j]->CollisionMesh_) && goatMinionPool[j]->active)
				{
					if (goatMinionPool[j]->getHp() < 1000)
						goatMinionPool[j]->isHitUpdate(-10);
				}

			}
			if (projectilePool[i]->aliveTime >= projectilePool[i]->deadTime)//if projectile travel longer than set-ed dead time, unactivate it
			{
				projectilePool[i]->CollisionMesh_->collisionEnabled = false;
				projectilePool[i]->aliveTime = 0.0;
			}
		}
	}
}
void GoatBoss::renderProjectiles(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	//for (auto it : bossProj_)
	//{
	//	modelStack->PushMatrix();
	//	modelStack->Translate(it->CollisionMesh_->pos.x, it->CollisionMesh_->pos.y, it->CollisionMesh_->pos.z);
	//	RenderMeshClass::RenderMesh(it->CollisionMesh_, true, projectionStack, viewStack, modelStack, m_parameters);
	//	modelStack->PopMatrix();
	//}

	for (size_t i = 0; i < (sizeof projectilePool) / sizeof(*projectilePool); ++i)
	{
		if (projectilePool[i]->CollisionMesh_->collisionEnabled)
		{
			modelStack->PushMatrix();
			modelStack->Translate(projectilePool[i]->CollisionMesh_->pos.x, projectilePool[i]->CollisionMesh_->pos.y, projectilePool[i]->CollisionMesh_->pos.z);
			RenderMeshClass::RenderMesh(projectilePool[i]->CollisionMesh_, true, projectionStack, viewStack, modelStack, m_parameters);
			modelStack->PopMatrix();
		}
	}

}

void GoatBoss::resetState()
{
	currState_ = BS_IDLE;
	delete attack;
	attack = NULL;
}

void GoatBoss::isHitUpdate(int dmg)
{
	hp_ -= (int)((1.0f - resistance_) * (float)dmg);
	if (hp_ < 0)
		hp_ = 0;
}


GoatMinion* GoatBoss::getInactiveGoatMinion()
{
	for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
	{
		if (!goatMinionPool[i]->active)
			return goatMinionPool[i];
	}
	return NULL;
}

void GoatBoss::updateHpStatus()
{
	if (hp_ <= 0)
		Bhp_status = BHP_DEAD;
	else if (hp_ < 0.25 * maxHp_)
		Bhp_status = BHP_BELOW25;
	else if (hp_ < 0.5 * maxHp_)
		Bhp_status = BHP_BELOW50;
	else if (hp_ < 0.75 * maxHp_)
		Bhp_status = BHP_BELOW75;
	else if (hp_ < maxHp_)
		Bhp_status = BHP_BELOW100;
	else 
		Bhp_status = BHP_FULL;
}

Projectile* GoatBoss::getInactiveProjectile()
{
	for (size_t i = 0; i < (sizeof projectilePool) / sizeof(*projectilePool); ++i)
	{
		if (!projectilePool[i]->CollisionMesh_->collisionEnabled)
			return projectilePool[i];
	}
	return NULL;
}