#ifndef USABLES_H
#define USABLES_H

#include "Item.h"

class Usables : public Item
{
public:
	Usables(std::string& name, signed int count) : Item(name), count(count) { ; }
	virtual ~Usables() { ; }

	// Getters
	int getCount() { return count; } // Use this function to render number of health, speed, attack boost potion count...

	// Setters
	// To set the number of count of the potions, for example when player uses 1 pot, pass in -1 as param
	//                                                     when player gain n potions, pass in n as param
	void setCount(signed int count) 
	{
		this->count += count;
	}

protected:
	signed int count;
};

#endif