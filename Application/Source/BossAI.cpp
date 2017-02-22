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


GoatBoss::GoatBoss() : EnemyAI("GoatBoss"), maxHp_(hp_)
{
	BossProjSpeed = 5.0;
	mesh[botParts::body] = MeshBuilder::GenerateOBJ("", "OBJ//goat.obj");
	CollisionMesh_ = mesh[botParts::body];
	attack = NULL;
	currState_ = BS_IDLE;
	animTime = 0.0;
	particleAnimTime = 0.0;
	CollisionMesh_->up.Set(0, 1, 0);

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
	if (Player::getInstance()->CollisionMesh_->pos != GoatBoss::getInstance()->CollisionMesh_->pos)
		dirBossToPlayer = (Player::getInstance()->CollisionMesh_->pos -
		GoatBoss::getInstance()->CollisionMesh_->pos).Normalized();
	else
		dirBossToPlayer.Set(0, 0, 1);
	CollisionMesh_->dir = dirBossToPlayer;
	CollisionMesh_->right = CollisionMesh_->dir.Cross(CollisionMesh_->up).Normalized();

	static bool once = false;
	//static bool playPartAnim = false;
	//static bool playPartAnimOnce = false;

	//animTime is for boss animation

	//Randomise attack style
	if (currState_ == BS_IDLE && !once || Application::IsKeyPressed('N'))// && !once)
	{
		//if (hp_ < 0.75 * maxHp_)
		//particleAnimTime = 0.0;
		delete attack;
		

		if (rand() % 10 == 0)
			attack = new BossGSmash;
		else
			attack = new BossShootProj;
		currState_ = BS_ATTACK;
		once = true;
		animTime = 0.0;
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
}
void GoatBoss::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	modelStack->PushMatrix();
	modelStack->Translate(0, 0, 0);
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
	modelStack->Translate(0.15f * CollisionMesh_->right.x, 1.2f, (0.15f * CollisionMesh_->right.z));
	modelStack->Translate(CollisionMesh_->pos.x, CollisionMesh_->pos.y, CollisionMesh_->pos.z);//y always 0
	modelStack->Rotate((CollisionMesh_->dir.x < 0 ? -1.0f : 1.0f) * Math::RadianToDegree(acos(CollisionMesh_->dir.Dot(Vector3(0.f, 0.f, 1.f)))), 0.f, 1.f, 0.f);
	modelStack->Scale(0.3f, 0.3f, 0.3f);
	RenderMeshClass::RenderText(&Scene::Text[Scene::TEXT_TYPE::Chiller], std::to_string(hp_), Color(1, 0, 0), projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();
	
	renderProjectiles(projectionStack, viewStack, modelStack, m_parameters);
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