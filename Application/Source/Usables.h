#ifndef USABLES_H
#define USABLES_H

#include "Item.h"

class Usables : public Item
{
public:
	Usables(std::string& name, int count) : Item(name), count(count) { ; }
	virtual ~Usables();

	// Getters


	// Setters

protected:
	int count;
};

#endif