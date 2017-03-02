#include "EnemyAI.h"


#include "Scene.h"
#include "RenderMesh.h"
#include "Player.h"

void EnemyAI::updateShowDmgTaken(double dt){
	//for (std::vector<DMGTAKEN>::iterator it = dmgTakenVector.begin(); it != dmgTakenVector.end(); )
	for (size_t i = 0; i < dmgTakenVector.size(); ++i)
	{
		dmgTakenVector.at(i).timeElapsed += dt;
		dmgTakenVector.at(i).tY += dt * dmgTakenVector.at(i).tySpd;
		dmgTakenVector.at(i).tySpd -= dt;

		dmgTakenVector.at(i).Dscale = (this->CollisionMesh_->pos - Player::getInstance()->CollisionMesh_->pos).Length() / 10.0f;
		if (dmgTakenVector.at(i).Dscale <= 1.f)//if player and this is less than 10units apart
			dmgTakenVector.at(i).Dscale = 1.f;

		if (dmgTakenVector.at(i).timeElapsed >= 2.0)
		{
			if (i != dmgTakenVector.size() - 1)
				std::swap(dmgTakenVector.at(i), dmgTakenVector.back());
			dmgTakenVector.pop_back();
		}
	}
	showDmgDelay += (float)dt;
}
void EnemyAI::renderShowDmgTaken(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters) {
	for (auto it : dmgTakenVector)
	{
		modelStack->PushMatrix();
		modelStack->Translate(0, it.tY, 0);
		modelStack->Scale(it.Dscale, it.Dscale, 1);
		RenderMeshClass::RenderText(&Scene::Text[Scene::TEXT_TYPE::Chiller], std::to_string(it.dmg), Color(1, 0, 0), projectionStack, viewStack, modelStack, m_parameters);
		modelStack->PopMatrix();
	}
}
void EnemyAI::isHitShowDmgTaken(int dmg){
	dmgTakenBeforeShown += dmg;
	if (showDmgDelay > 0.25f)
	{
		DMGTAKEN temp;
		//temp.dmg = dmg;
		temp.dmg = dmgTakenBeforeShown;
		temp.timeElapsed = 0.0;
		temp.tY = 0.f;
		temp.tySpd = 2.f;
		temp.Dscale = 1.f;

		isHit_ = true;
		dmgTakenVector.push_back(temp);
		showDmgDelay = 0.f;
		dmgTakenBeforeShown = 0;
	}
}