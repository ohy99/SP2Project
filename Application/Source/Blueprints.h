#ifndef BLUEPRINTS_H
#define BLUEPRINTS_H

#include "Mesh.h"
#include "MatrixStack.h"

class Blueprints
{
public:
	Blueprints() { }
	~Blueprints() { }

	static int numberOfBlueprints;
	static int GetBlueprintNumber();
	static void AddBlueprintNumber();
};

#endif