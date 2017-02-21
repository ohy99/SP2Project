#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "GameObject.h"
#include <string>

class Projectile :public GameObject
{
	double pSpeed_;
public:
	Projectile(const std::string& name, double spd) : GameObject(name),pSpeed_(spd){}
	virtual ~Projectile() {};
	void setSpeed(double spd) { pSpeed_ = spd; }
	double getSpeed() { return pSpeed_; }
};

#endif