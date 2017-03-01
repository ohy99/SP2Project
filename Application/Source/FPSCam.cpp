#include "FPSCam.h"
#include "Player.h"

FPSCam* FPSCam::Instance = 0;

#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"

FPSCam::FPSCam()
{
	this->position = Player::getInstance()->CollisionMesh_->pos;
	this->position.x = Player::getInstance()->CollisionMesh_->pos.x + (float)(0.1f * dir.x);
	this->position.y = Player::getInstance()->CollisionMesh_->pos.y + 1.825f;
	this->position.z = Player::getInstance()->CollisionMesh_->pos.z + (float)(0.1f * dir.z);
	//this->target = target;
	this->up = Player::getInstance()->CollisionMesh_->up;
	this->dir = Player::getInstance()->CollisionMesh_->dir;
	Cam_Phi = Math::RadianToDegree(sin(dir.y));
	Cam_Theta = (dir.x < 0 ? 360.0f - Math::RadianToDegree(acos(dir.z)) : Math::RadianToDegree(acos(dir.z)));
}

FPSCam::~FPSCam()
{
	Instance = NULL;
}

void FPSCam::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{

}

void FPSCam::Reset()
{
}
#include <GLFW\glfw3.h>
//#include "Application.h"

/******************************************************************************/
/*!
\brief
Update Camera3 based on displacement of Cursor
*/
/**
/******************************************************************************/
void FPSCam::Update(double dt, double dx, double dy)
{
	dt *= 2.0f; //speedz

	float mouseSpeed = 0.05f;

	float horizontalDISAngle = 0;
	float verticalDISAngle = 0;
	horizontalDISAngle += mouseSpeed * dx; //taking displacement as pi
	verticalDISAngle += mouseSpeed * dy;


	if (Cam_Theta + Math::RadianToDegree(horizontalDISAngle) > 360.0f)
		Cam_Theta += Math::RadianToDegree(horizontalDISAngle) - 360.0f;
	else if (Cam_Theta + Math::RadianToDegree(horizontalDISAngle) < 0.0f)
		Cam_Theta += 360.0f + Math::RadianToDegree(horizontalDISAngle);
	else
		Cam_Theta += Math::RadianToDegree(horizontalDISAngle);

	if (Cam_Phi + Math::RadianToDegree(verticalDISAngle) > 75.0f)
		Cam_Phi += 0.0f;
	else if (Cam_Phi + Math::RadianToDegree(verticalDISAngle) < -75.0f)
		Cam_Phi += 0.0f;
	else
		Cam_Phi += Math::RadianToDegree(verticalDISAngle);


	this->position.x = Player::getInstance()->CollisionMesh_->pos.x + (float)(0.1f * dir.x);
	this->position.y = Player::getInstance()->CollisionMesh_->pos.y + 1.825f;
	this->position.z = Player::getInstance()->CollisionMesh_->pos.z + (float)(0.1f * dir.z);


	dir.Set(
		cos(Math::DegreeToRadian(Cam_Phi)) * sin(Math::DegreeToRadian(Cam_Theta)),
		sin(Math::DegreeToRadian(Cam_Phi)),
		cos(Math::DegreeToRadian(Cam_Phi)) * cos(Math::DegreeToRadian(Cam_Theta))
		);
	dir.Normalize();

	//Vector3 Right = dir.Cross(this->up).Normalized();
	up.Set(
		sin(Math::DegreeToRadian(Cam_Phi)) * -sin(Math::DegreeToRadian(Cam_Theta)),
		cos(Math::DegreeToRadian(Cam_Phi)),
		sin(Math::DegreeToRadian(Cam_Phi)) * -cos(Math::DegreeToRadian(Cam_Theta))
		);
	up.Normalize();
	Right = dir.Cross(up).Normalized();
	//up = Right.Cross(dir).Normalized();
}
