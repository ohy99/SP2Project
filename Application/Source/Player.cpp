#include "Player.h"
#include "Application.h"
#include "Mesh.h"
#include "MainScene.h"

Player* Player::Instance_ = 0;
Player::Player() : GameObject("Player") 
{
	//int hp_;
	//Vector3 pos_;
	//static Player* Instance_;
	//States state_;

	//Weapon* weapons_[Weapon_types::wt_Count];
	//Weapon* currentWeapon_;

}

Player* Player::getInstance() 
{ 
	if (Instance_) 
		return Instance_; 
	else 
		return (Instance_ = new Player()); 
}
void Player::setPosition(Vector3& pos)
{
	pos_ = pos;
}


void Player::update(double dt, Camera* cam)
{

	std::vector<GameObject*> nearObjs;

	//PreCheck collision
	//Mesh projected("Projected");
	//projected.setHb(true, PMesh[MESH_TYPE::Body]->TempMin, PMesh[MESH_TYPE::Body]->TempMax, PMesh[MESH_TYPE::Body]->pos, PMesh[MESH_TYPE::Body]->dir);

	if (Application::IsKeyPressed('W'))
	{
		/*bool bam = false;
		projected.pos.x += (dir_.x * dt * moveSPD);
		projected.pos.z += (dir_.z * dt * moveSPD);

		for (size_t i = 0; i < nearObjs.size(); i++)
		{
			if (projected.isCollide(nearObjs[i]->CollisionMesh_) == true)
			{
				bam = true;
				break;
			}
		}

		if (bam == false)
		{
			pos_.x += (dir_.x * dt * moveSPD);
			pos_.z += (dir_.z * dt * moveSPD);
		}*/
	}
	if (Application::IsKeyPressed('S'))
	{

	}
	if (Application::IsKeyPressed('A'))
	{

	}
	if (Application::IsKeyPressed('D'))
	{

	}
}

void Player::render()
{

	//if (Pointed_Obj)
	//	MainScene::RenderTextOnScreen(&MainScene::Text[MainScene::TEXT_TYPE::Century], Pointed_Obj->getName(), Color(1, 0, 0), 1, 1, 1);
}


void Player::getPointedObj(Camera* cam)
{
	Position TargetPoint;
	TargetPoint.Set(cam->getTarget().x, cam->getTarget().y, cam->getTarget().z);
	Pointed_Obj = NULL;


	for (auto it : MainScene::Game_Objects_)
	{
		if (it->CollisionMesh_->isPointInsideAABB(TargetPoint) == true)
		{
			Pointed_Obj = it;
			break;
		}

	}

}
//static Player* getInstance();
//void setPosition(Vector3& pos);
//
//void update(double dt, Camera* cam);
//void render();
//void getPointedObj(Camera* cam);
//
//void isHitUpdate(int dmg);
//
//
//bool isDead();
//~Player();