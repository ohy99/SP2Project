#include "MGPlayer.h"
#include "Application.h"
#include "Mesh.h"
#include "MiniGame.h"
#include "Scene.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"

MGPlayer* MGPlayer::Instance = 0;
std::vector<GameObject*> MGPlayer::CollisionObjects;

MGPlayer::MGPlayer() : GameObject("Player"), lane(1.f), moveSpeed(30.f), isHit(false),
wasDPressed(false), wasAPressed(false), isGameStarted(false), isGameReset(false)
{

}

MGPlayer::~MGPlayer()
{
	//delete[] PMesh;
}

MGPlayer* MGPlayer::getInstance()
{
	if (!Instance)
		return (Instance = new MGPlayer());

	else
		return Instance;
}

void MGPlayer::setPlayerPosition(Vector3& pos_)
{
	this->pos_ = pos_;
}

void MGPlayer::Init()
{
	PMesh[MESH_TYPE::BODY] = MeshBuilder::GenerateOBJ("", "OBJ//goat.obj");
	PMesh[MESH_TYPE::BODY]->collisionEnabled = true;
	PMesh[MESH_TYPE::BODY]->up = Vector3(0.f, 1.f, 0.f);
	PMesh[MESH_TYPE::BODY]->dir = Vector3(0.f, 0.f, 1.f);
	PMesh[MESH_TYPE::BODY]->right = Vector3(1.f, 0.f, 0.f);
	dir_.Set(0.f, 0.f, 1.f);


	CollisionMesh_ = PMesh[MESH_TYPE::BODY];
}

void MGPlayer::Update(double dt, Camera* cam)
{
	right_ = dir_.Normalize().Cross(Vector3(0, 1, 0)).Normalized();

	isDPressed = Application::IsKeyPressed('D');
	isAPressed = Application::IsKeyPressed('A');

	MGPlayerMovements(dt);
}

void MGPlayer::Render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	//modelStack->PushMatrix();
	//modelStack->Translate(pos_.x, pos_.y, pos_.z);
	////modelStack->Rotate(dirRotateAngle, dirRotateAxis.x, dirRotateAxis.y, dirRotateAxis.z);
	//RenderMeshClass::RenderMesh(PMesh[MESH_TYPE::BODY], true, projectionStack, viewStack, modelStack, m_parameters);
	//modelStack->PopMatrix();
}

void MGPlayer::MGPlayerMovements(double dt)
{
	if (isGameStarted)
	{
		if (!isHit && !UI::getInstance()->isPauseOpen())
		{
			Mesh projected("Projected");
			projected.setHb(true, PMesh[MESH_TYPE::BODY]->Hitbox_Min, PMesh[MESH_TYPE::BODY]->Hitbox_Max, PMesh[MESH_TYPE::BODY]->pos, PMesh[MESH_TYPE::BODY]->dir);

			for (size_t i = 0; i < CollisionObjects.size(); i++)
			{
				if (projected.isCollide(CollisionObjects[i]->CollisionMesh_))
				{
					isHit = true;
					break;
				}
			}

		pos_.x += (dir_.x * (float)dt * moveSpeed);
		pos_.z += (dir_.z * (float)dt * moveSpeed);

			if (isDPressed && !wasDPressed)
			{
				--lane;

				if (lane < 0.f)
					lane = 0.f;

				pos_.x = -7.f + (lane * 7.f);

				if (pos_.x < -7.f)
					pos_.x = -7.f;

				wasDPressed = isDPressed;
			}

			if (!isDPressed && wasDPressed)
				wasDPressed = isDPressed;


			if (isAPressed && !wasAPressed)
			{
				++lane;

				if (lane > 2.f)
					lane = 2.f;

				pos_.x = -7.f + (lane * 7.f);

				if (pos_.x > 7.f)
					pos_.x = 7.f;

				wasAPressed = isAPressed;
			}

			if (!isAPressed && wasAPressed)
				wasAPressed = isAPressed;

			PMesh[MESH_TYPE::BODY]->pos = pos_;
		}
	}
}

float MGPlayer::playerScore(float scoreMultiplier)
{
	return (pos_.z * scoreMultiplier);
}

bool MGPlayer::gameStarted()
{
	return isGameStarted;
}

bool MGPlayer::isDead()
{
	return isHit;
}

bool MGPlayer::Reset()
{
	return isGameReset;
}

void MGPlayer::setGameState(bool gameStarted, bool gameReset, bool isHit)
{
	isGameStarted = gameStarted;
	isGameReset = gameReset;
	this->isHit = isHit;
}

void MGPlayer::clearOBJ()
{
	while (CollisionObjects.size() != 0)
		CollisionObjects.pop_back();
}