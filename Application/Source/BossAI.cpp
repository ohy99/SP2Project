#include "BossAI.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"
#include "Player.h"
#include "LoadATOM.h"

//temp
#include "Application.h"


GoatBoss* GoatBoss::instance = NULL;
std::vector<Projectile*> GoatBoss::bossProj_;
double GoatBoss::BossProjSpeed;
Projectile* GoatBoss::projMesh = NULL;
Mesh* GoatBoss::AOESmash = NULL;
Vector3 GoatBoss::dirBossToPlayer;
double GoatBoss::particleAnimTime = 0.0;
Mesh* GoatBoss::goatMinionMesh;
GoatBoss::BOSSHPSTATUS GoatBoss::Bhp_status = GoatBoss::BOSSHPSTATUS::BHP_FULL;

GoatBoss::GoatBoss() : EnemyAI("GoatBoss"), maxHp_(hp_)
{
	BossProjSpeed = 5.0;
	mesh[botParts::body] = MeshBuilder::GenerateOBJ("", "OBJ//GoatBossTemp.obj");
	CollisionMesh_ = mesh[botParts::body];
	attack = NULL;
	currState_ = BS_IDLE;
	animTime = 0.0;
	particleAnimTime = 0.0;
	CollisionMesh_->up.Set(0, 1, 0);
	goatMinionMesh = MeshBuilder::GenerateOBJ("Goat Minion", "OBJ//goat.obj");
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
	attackStateCD = 0.0;
	GroundSmashCD = ShootProjCD = DropGoatCD = 0.0;

	Projectile* temp_proj;
	temp_proj = new Projectile("", BossProjSpeed);
	temp_proj->CollisionMesh_ = MeshBuilder::GenerateOBJ("BossProj", "OBJ//goat.obj");
	//temp_proj->setSpeed(BossProjSpeed);
	projMesh = temp_proj;

	AOESmash = MeshBuilder::GenerateOBJ("", "OBJ//GSAOE.obj");

	srand(time(NULL));
}
GoatBoss::~GoatBoss()
{
	delete mesh[botParts::body];
	delete attack;
	delete AOESmash;
	delete goatMinionMesh;
	delete[] goatMinionPool;
	while (bossProj_.size())
		bossProj_.pop_back();
	delete projMesh;
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
			attack = new BossFlank;
		//	attack = new BossGSmash;
		else if (getInactiveGoatMinion() && (rand() % 10 == 9))
			attack = new BossDGoat;
		else if (rand() % 10 <= 8)
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

void GoatBoss::addProjectiles(Projectile* inProj)
{
	bossProj_.push_back(inProj);
}
void GoatBoss::updateProjectiles(double dt)
{
	for (auto it : bossProj_)
	{
		it->CollisionMesh_->pos += (it->CollisionMesh_->dir * (float)dt * BossProjSpeed);
		if (it->CollisionMesh_->isCollide(Player::getInstance()->CollisionMesh_))
		{
			Player::getInstance()->isHitUpdate(50);
		}
		for (size_t i = 0; i < (sizeof goatMinionPool) / sizeof(*goatMinionPool); ++i)
		{
			if (it->CollisionMesh_->isCollide(goatMinionPool[i]->CollisionMesh_) && goatMinionPool[i]->active)
			{
				goatMinionPool[i]->isHitUpdate(-10);
			}

		}
	}
}
void GoatBoss::renderProjectiles(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	for (auto it : bossProj_)
	{
		modelStack->PushMatrix();
		modelStack->Translate(it->CollisionMesh_->pos.x, it->CollisionMesh_->pos.y, it->CollisionMesh_->pos.z);
		RenderMeshClass::RenderMesh(it->CollisionMesh_, true, projectionStack, viewStack, modelStack, m_parameters);
		modelStack->PopMatrix();
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
	hp_ -= dmg;
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
//GoatMinion* GoatBoss::getMinionPool()
//{
//	return *goatMinionPool;
//}

void GoatBoss::moveToRandomLocation(double dt)
{

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
		//if (!projectilePool[i]->active)
			return projectilePool[i];
	}
	return NULL;
}