#include "Player.h"

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


void Player::update(double dt)
{

}

void Player::render()
{

}