#include "Blueprints.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"

int Blueprints::numberOfBlueprints = 0;

int Blueprints::GetBlueprintNumber()
{
	return numberOfBlueprints;
}
void Blueprints::AddBlueprintNumber()
{
	numberOfBlueprints++;
}