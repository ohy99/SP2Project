#ifndef Test_H
#define Test_H

#include "Scene.h"

#include "Camera.h"
#include "Camera3.h"
#include "Mesh.h"
//#include "MeshBuilder.h"
#include "MatrixStack.h"
#include "Light.h"

#include <vector>


class Test : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBE,
		GEO_CIRCLE,
		GEO_RING,
		GEO_SPHERE,
		GEO_HEMISPHERE,
		GEO_TORUS,
		GEO_CYLINDER,
		GEO_LIGHTBALL,
		GEO_LIGHTBALL1,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		NUM_GEOMETRY,
	};
	enum TEXT_TYPE
	{
		Calibri,
		Century,
		Chiller,
		ScriptMTBold,
		SegoeMarker,
		Text_Count
	};
	struct Text_Data
	{
		Mesh* Text_Mesh;
		float TextWidth[256];
	};


public:
	Test();
	~Test();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	void RenderMesh(Mesh *mesh, bool enableLight);



	static enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

		U_LIGHTENABLED,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		U_TOTAL,
	};

	static Mesh* GroundMesh;

private:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	MS modelStack, viewStack, projectionStack;
	double FramesPerSec;

	Camera *camera;

	void RenderA3Skybox();

	Light light[8];

	Text_Data Text[TEXT_TYPE::Text_Count];
	void RenderText(Text_Data* TextData, std::string text, Color color);
	void RenderTextOnScreen(Text_Data* TextData, std::string text, Color color, float size, float x, float y);
	void RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey);

};

#endif