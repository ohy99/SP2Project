#include "RenderMesh.h"
#include "GL\glew.h"
#include "LoadTGA.h"
#include "LoadTextData.h"
#include "A3Scene.h"

GText_Data GlobalText[GTEXT_TYPE::GText_Count];
void RenderMesh(Mesh *mesh, bool enableLight, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 0);
	}


	MVP = projectionStack->Top() * viewStack->Top() * modelStack->Top();
	glUniformMatrix4fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack->Top() * modelStack->Top();
	glUniformMatrix4fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{

		glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[A3Scene::UNIFORM_TYPE::U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_LIGHTENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void initGlobalRenderMesh()
{
	GlobalText[GTEXT_TYPE::Calibri].Text_Mesh = MeshBuilder::GenerateText("Calibri", 16, 16);
	GlobalText[GTEXT_TYPE::Calibri].Text_Mesh->textureID = LoadTGA("Image//calibri.tga");
	LoadTextData("Image//Calibri Data.csv", GlobalText[GTEXT_TYPE::Calibri].TextWidth);
	GlobalText[GTEXT_TYPE::Chiller].Text_Mesh = MeshBuilder::GenerateText("Chiller", 16, 16);
	GlobalText[GTEXT_TYPE::Chiller].Text_Mesh->textureID = LoadTGA("Image//Chiller.tga");
	LoadTextData("Image//Chiller Data.csv", GlobalText[GTEXT_TYPE::Chiller].TextWidth);
	GlobalText[GTEXT_TYPE::Century].Text_Mesh = MeshBuilder::GenerateText("Century", 16, 16);
	GlobalText[GTEXT_TYPE::Century].Text_Mesh->textureID = LoadTGA("Image//Century.tga");
	LoadTextData("Image//Century Data.csv", GlobalText[GTEXT_TYPE::Century].TextWidth);
	GlobalText[GTEXT_TYPE::ScriptMTBold].Text_Mesh = MeshBuilder::GenerateText("ScriptMTBold", 16, 16);
	GlobalText[GTEXT_TYPE::ScriptMTBold].Text_Mesh->textureID = LoadTGA("Image//Script MT Bold.tga");
	LoadTextData("Image//Script MT Bold Data.csv", GlobalText[GTEXT_TYPE::ScriptMTBold].TextWidth);
	GlobalText[GTEXT_TYPE::SegoeMarker].Text_Mesh = MeshBuilder::GenerateText("SegoeMarker", 16, 16);
	GlobalText[GTEXT_TYPE::SegoeMarker].Text_Mesh->textureID = LoadTGA("Image//Segoe Marker.tga");
	LoadTextData("Image//Segoe Marker Data.csv", GlobalText[GTEXT_TYPE::SegoeMarker].TextWidth);
}
void RenderText(GText_Data* TextData, std::string text, Color color, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[A3Scene::UNIFORM_TYPE::U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	float nextCharWidth = 0.0f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		char Character = text[i];
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
		nextCharWidth += TextData->TextWidth[Character];
		Mtx44 MVP = projectionStack->Top() * viewStack->Top() * modelStack->Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);

		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}
void RenderTextOnScreen(GText_Data* TextData, std::string text, Color color, float size, float x, float y, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI CHANGEABLE
	projectionStack->PushMatrix();
	projectionStack->LoadMatrix(ortho);
	viewStack->PushMatrix();
	viewStack->LoadIdentity(); //No need camera for ortho mode
	modelStack->PushMatrix();
	modelStack->LoadIdentity(); //Reset modelStack
	modelStack->Scale(size, size, size);
	modelStack->Translate(x, y, 0);

	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[A3Scene::UNIFORM_TYPE::U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	float nextCharWidth = 0.0f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		char Character = text[i];
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
		nextCharWidth += TextData->TextWidth[Character];
		Mtx44 MVP = projectionStack->Top() * viewStack->Top() * modelStack->Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[A3Scene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);

		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[A3Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
	projectionStack->PopMatrix();
	viewStack->PopMatrix();
	modelStack->PopMatrix();
}
