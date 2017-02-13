#ifndef	WEAPON_H
#define WEAPON_H

#include "Item.h"

class Weapon : public Item
{
public: 
	Weapon(std::string& name) : Item(name) {}
	virtual ~Weapon() {}
};

#endif