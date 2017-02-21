#include "RenderMesh.h"
#include "GL\glew.h"
#include "Application.h"

void RenderMeshClass::RenderMesh(Mesh* mesh, bool enableLight, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	if (mesh->textureID > 0)
	{
		glUniform1i(Scene::m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(Scene::m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(Scene::m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 0);
	}


	MVP = projectionStack->Top() * viewStack->Top() * modelStack->Top();
	glUniformMatrix4fv(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack->Top() * modelStack->Top();
	glUniformMatrix4fv(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{

		glUniform1i(Scene::m_parameters[Scene::UNIFORM_TYPE::U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(Scene::m_parameters[Scene::UNIFORM_TYPE::U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(Scene::m_parameters[Scene::UNIFORM_TYPE::U_LIGHTENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void RenderMeshClass::RenderText(Scene::Text_Data* TextData, std::string text, Color color, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[Scene::UNIFORM_TYPE::U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	float nextCharWidth = 0.0f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		char Character = text[i];
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
		nextCharWidth += TextData->TextWidth[Character];
		Mtx44 MVP = projectionStack->Top() * viewStack->Top() * modelStack->Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[Scene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);

		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}
void RenderMeshClass::RenderTextOnScreen(Scene::Text_Data* TextData, std::string text, Color color, float size, float x, float y, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	if (!TextData->Text_Mesh || TextData->Text_Mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack->PushMatrix();
	projectionStack->LoadMatrix(ortho);
	viewStack->PushMatrix();
	viewStack->LoadIdentity(); //No need camera for ortho mode
	modelStack->PushMatrix();
	modelStack->LoadIdentity(); //Reset modelStack
	modelStack->Translate(x, y, 0);
	modelStack->Scale(size, size, size);

	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[Scene::UNIFORM_TYPE::U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextData->Text_Mesh->textureID);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_COLOR_TEXTURE], 0);
	float nextCharWidth = 0.0f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		char Character = text[i];
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(nextCharWidth, 0, 0); //1.0f is the spacing of each character, you may change this value
		nextCharWidth += TextData->TextWidth[Character];
		Mtx44 MVP = projectionStack->Top() * viewStack->Top() * modelStack->Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[Scene::UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP.a[0]);

		TextData->Text_Mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[Scene::UNIFORM_TYPE::U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
	projectionStack->PopMatrix();
	viewStack->PopMatrix();
	modelStack->PopMatrix();
}
void RenderMeshClass::RenderMeshOnScreen(Mesh* mesh, int x, int y, int z, int sizex, int sizey, MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::getWindowWidth(), 0, Application::getWindowHeight(), -10, 10); //size of screen UI
	projectionStack->PushMatrix();
	projectionStack->LoadMatrix(ortho);
	viewStack->PushMatrix();
	viewStack->LoadIdentity(); //No need camera for ortho mode
	modelStack->PushMatrix();
	modelStack->LoadIdentity();
	//to do: scale and translate accordingly

	modelStack->Translate(x, y, z);
	modelStack->Scale(sizex, sizey, 1);
	RenderMesh(mesh, false, projectionStack, viewStack, modelStack, m_parameters); //UI should not have light
	projectionStack->PopMatrix();
	viewStack->PopMatrix();
	modelStack->PopMatrix();
	glEnable(GL_DEPTH_TEST);
}