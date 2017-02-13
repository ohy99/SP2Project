#ifndef RENDERMESH_H
#define RENDERMESH_H

#include "Mesh.h"
#include "MeshBuilder.h"
#include "MatrixStack.h"
//#include <string>
//#include "Vertex.h"

enum GTEXT_TYPE
{
	Calibri,
	Century,
	Chiller,
	ScriptMTBold,
	SegoeMarker,
	GText_Count
};
struct GText_Data
{
	Mesh* Text_Mesh;
	float TextWidth[256];
};


void initGlobalRenderMesh();
void RenderMesh(Mesh *mesh, bool enableLight, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
void RenderText(GText_Data* TextData, std::string text, Color color, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
void RenderTextOnScreen(GText_Data* TextData, std::string text, Color color, float size, float x, float y, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);

#endif