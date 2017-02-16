#ifndef _NPC_DOC_H
#define _NPC_DOC_H

#include "AllyNPC.h"
#include "Mesh.h"

class NPC_DOC : public AllyNPC
{
	enum NPC_DOCPARTS
	{
		BODY,
		LEFTARM,
		RIGHTARM,
		NPC_DOCPARTS_COUNT
	};
	Mesh* NPC_Doc_mesh[NPC_DOCPARTS::NPC_DOCPARTS_COUNT];
	double anim_TimeElapsed;
public:
	NPC_DOC();
	~NPC_DOC();
	void update(double dt);
	void render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
};

#endif