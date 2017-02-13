#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = pos;
	//this->target = target;
	this->up = up;
	this->dir = (target - this->position).Normalized();
	Cam_Phi = Math::RadianToDegree(sin(dir.y));
	Cam_Theta = (dir.x < 0 ? 360.0f - Math::RadianToDegree(acos(dir.z)) : Math::RadianToDegree(acos(dir.z)));
	Bounds.Set(10.0f, 10.0f, 10.0f);
}

/******************************************************************************/
/*!
\brief
Reset the Camera3 settings
*/
/******************************************************************************/
void Camera3::Reset()
{
}

#include <iostream>
#include <string>
void Camera3::Update(double dt)
{
	dt *= 2.0f; //speedz
	Right = dir.Cross(this->up).Normalized();


	if (Application::IsKeyPressed('W') && boundCheck(dir, dt))
		//	abs(float(position.x + (dir.x * dt))) <= Bounds.x && abs(float(position.y + (dir.y * dt))) <= Bounds.y && abs(float(position.z + (dir.z * dt))) <= Bounds.z)
	{
		this->position.x += (float)(dir.x * dt);
		this->position.y += (float)(dir.y * dt);
		this->position.z += (float)(dir.z * dt);
	}
	if (Application::IsKeyPressed('S') && boundCheck(-dir, dt))
		//	abs(position.x + (-dir.x * dt)) <= abs(Bounds.x && position.y + (-dir.y * dt)) <= Bounds.y && abs(position.z + (-dir.z * dt)) <= Bounds.z)
	{
		this->position.x += (float)(-dir.x * dt);
		this->position.y += (float)(-dir.y * dt);
		this->position.z += (float)(-dir.z * dt);
	}
	if (Application::IsKeyPressed('A') && boundCheck(-Right, dt))
	{
		this->position.x += (float)(-Right.x * dt);
		this->position.z += (float)(-Right.z * dt);
	}
	if (Application::IsKeyPressed('D') && boundCheck(Right, dt))
	{
		this->position.x += (float)(Right.x * dt);
		this->position.z += (float)(Right.z * dt);
	}

	static const float CAMERA_SPEED = 50.0f;

	if (Application::IsKeyPressed(VK_LSHIFT) && boundCheck(up, dt))
	{
		this->position.y += (float)(up.y * dt);
	}
	if (Application::IsKeyPressed(VK_LCONTROL) && boundCheck(-up, dt))
	{
		this->position.y += (float)(-up.y * dt);
	}

	/*system("CLS");
	std::cout << "Position Vector: " << position.x << ", " << position.y << ", " << position.z << std::endl;*/
	//std::cout << "target Vector: " << target.x << ", " << target.y << ", " << target.z << std::endl;
	//std::cout << "Dir Vector: " << dir.x << ", " << dir.y << ", " << dir.z << std::endl;
	//std::cout << "Up Vector: " << up.x << ", " << up.y << ", " << up.z << std::endl;
	//Vector3 RightV = (target-position).Cross(up);
	//RightV.Normalize();
	//std::cout << "Right Vector: " << RightV.x << ", " << RightV.y << ", " << RightV.z << std::endl;
	//std::cout << "Cam_Phi: " << Cam_Phi << std::endl;


	if (Application::IsKeyPressed(VK_UP))
	{
		if (Cam_Phi < 50.0f)
		{
			float pitch = (float)(CAMERA_SPEED *dt);
			Mtx44 rotation;
			rotation.SetToRotation(pitch, Right.x, Right.y, Right.z);
			dir = rotation * dir;
			up = rotation * up;
			Cam_Phi += (CAMERA_SPEED * dt);
		}

	}
	else if (Application::IsKeyPressed(VK_DOWN))
	{
		if (Cam_Phi > -50.0f)
		{
			float pitch = (float)(-CAMERA_SPEED *dt);
			Mtx44 rotation;
			rotation.SetToRotation(pitch, Right.x, Right.y, Right.z);
			dir = rotation * dir;
			up = rotation * up;
			Cam_Phi += (-CAMERA_SPEED * dt);
		}
	}

	else if (Application::IsKeyPressed(VK_LEFT))
	{
		float yaw = (float)(CAMERA_SPEED *dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, up.x, up.y, up.z);
		dir = rotation * dir;
		up = rotation * up;
		Cam_Theta += (CAMERA_SPEED * dt);

	}
	else if (Application::IsKeyPressed(VK_RIGHT))
	{
		float yaw = (float)(-CAMERA_SPEED *dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, up.x, up.y, up.z);
		dir = rotation * dir;
		up = rotation * up;
		Cam_Theta += (-CAMERA_SPEED * dt);
	}

}
bool Camera3::boundCheck(Vector3 path, double dt)
{
	if (abs(float(position.x + (path.x * dt))) <= Bounds.x && abs(float(position.y + (path.y * dt))) <= Bounds.y && abs(float(position.z + (path.z * dt))) <= Bounds.z)
		return true;
	else
		return false;
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
void Camera3::Update(double dt, double dx, double dy)
{
	dt *= 2.0f; //speedz

	float mouseSpeed = 0.05f;

	float horizontalDISAngle = 0;
	float verticalDISAngle = 0;
	horizontalDISAngle += mouseSpeed * dx; //taking displacement as pi
	verticalDISAngle += mouseSpeed * dy;
	Cam_Phi += Math::RadianToDegree(verticalDISAngle);

	if (Cam_Theta + Math::RadianToDegree(horizontalDISAngle) > 360.0f)
		Cam_Theta += Math::RadianToDegree(horizontalDISAngle) - 360.0f;
	else if (Cam_Theta + Math::RadianToDegree(horizontalDISAngle) < 0.0f)
		Cam_Theta += 360.0f + Math::RadianToDegree(horizontalDISAngle);
	else
		Cam_Theta += Math::RadianToDegree(horizontalDISAngle);

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
	//up = Right.Cross(dir).Normalized();

	int state = glfwGetMouseButton(Application::m_window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		this->position.x += (float)(dir.x * dt * 10.0f);
		this->position.y += (float)(dir.y * dt * 10.0f);
		this->position.z += (float)(dir.z * dt * 10.0f);
	}
	//if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	//{
	//	this->position.x += (float)(-dir.x * dt);
	//	this->position.y += (float)(-dir.y * dt);
	//	this->position.z += (float)(-dir.z * dt);
	//}
}
