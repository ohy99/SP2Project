#ifndef _FPSCAM_H
#define _FPSCAM_H

#include "Camera.h"

class FPSCam : public Camera
{
	Vector3 position;
	//	Vector3 target;
	Vector3 dir;
	Vector3 up;
	Vector3 Right;

	float Cam_Phi = 0.0f;
	float Cam_Theta = 180.0f;

	FPSCam();
	static FPSCam* Instance;
public:
	static FPSCam* getInstance() { if (Instance) return Instance; else return(Instance = new FPSCam); }
	~FPSCam();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Reset();
	virtual void Update(double dt) {};
	virtual void Update(double dt, double dx, double dy);
	virtual Vector3 getPosition() { return position; };
	virtual Vector3 getTarget() { return position + dir; };
	virtual Vector3 getDir() { return dir; };
	virtual Vector3 getUp() { return up; };
	virtual Vector3 getRight() { return Right; };

};

#endif