#ifndef BLUEPRINTS_H
#define BLUEPRINTS_H

#include "Mesh.h"
#include "MatrixStack.h"
#include "Item.h"

class Blueprints : public Item
{
public:
	Blueprints(std::string name) : Item(name) { ; }
	~Blueprints() { ; }

	static int numberOfBlueprints;
	static int GetBlueprintNumber();
	static void AddBlueprintNumber();
};

#endif