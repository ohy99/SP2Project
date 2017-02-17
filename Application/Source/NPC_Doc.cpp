#include "NPC_Doc.h"

#include "MeshBuilder.h"
#include "MatrixStack.h"
#include "LoadATOM.h"
#include "MainScene.h"

#include "RenderMesh.h"

NPC_DOC::NPC_DOC() : AllyNPC("NPC_DOC")
{
	NPC_Doc_mesh[NPC_DOCPARTS::BODY] = MeshBuilder::GenerateOBJ("DOC_BODY", "OBJ//DocBody.obj");
	NPC_Doc_mesh[NPC_DOCPARTS::LEFTARM] = MeshBuilder::GenerateOBJ("DOC_BODY", "OBJ//DocLeftArm.obj");
	NPC_Doc_mesh[NPC_DOCPARTS::RIGHTARM] = MeshBuilder::GenerateOBJ("DOC_BODY", "OBJ//DocRightArm.obj");
	anim_TimeElapsed = 0.0;
	NPC_Doc_mesh[NPC_DOCPARTS::BODY]->pos.Set(5, 0, 5);

	CollisionMesh_ = NPC_Doc_mesh[NPC_DOCPARTS::BODY];
}
NPC_DOC::~NPC_DOC()
{
	for (size_t i = 0; i < NPC_DOCPARTS::NPC_DOCPARTS_COUNT; i++)
	{
		delete NPC_Doc_mesh[i];
	}
}
void NPC_DOC::update(double dt)
{
	anim_TimeElapsed += dt;
}
void NPC_DOC::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	modelStack->PushMatrix();
	modelStack->Translate(NPC_Doc_mesh[NPC_DOCPARTS::BODY]->pos.x, NPC_Doc_mesh[NPC_DOCPARTS::BODY]->pos.y, NPC_Doc_mesh[NPC_DOCPARTS::BODY]->pos.z);
	modelStack->PushMatrix();
	RenderMeshClass::RenderMesh(NPC_Doc_mesh[NPC_DOCPARTS::BODY], true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();
	modelStack->PushMatrix();
	LoadAtom("ATOM//DocAnimation.atom", &MainScene::modelStack, &anim_TimeElapsed, "LeftArm");
	RenderMeshClass::RenderMesh(NPC_Doc_mesh[NPC_DOCPARTS::LEFTARM], true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();
	modelStack->PushMatrix();
	LoadAtom("ATOM//DocAnimation.atom", &MainScene::modelStack, &anim_TimeElapsed, "RightArm");
	RenderMeshClass::RenderMesh(NPC_Doc_mesh[NPC_DOCPARTS::RIGHTARM], true, projectionStack, viewStack, modelStack, m_parameters);
	modelStack->PopMatrix();
	modelStack->PopMatrix();
}