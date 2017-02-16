#ifndef _RENDERMESH_H
#define _RENDERMESH_H

#include "Mesh.h"
#include "MatrixStack.h"
#include "Scene.h"

class RenderMeshClass
{
public:
	static void RenderMesh(Mesh *mesh, bool enableLight, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	static void RenderText(Scene::Text_Data* TextData, std::string text, Color color, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	static void RenderTextOnScreen(Scene::Text_Data* TextData, std::string text, Color color, float size, float x, float y, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
	static void RenderMeshOnScreen(Mesh* mesh, int x, int y, int z, int sizex, int sizey, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters);
};
#endif