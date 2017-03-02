#include "SandStorm.h"
#include "MeshBuilder.h"
#include "RenderMesh.h"
#include "Player.h"

SandStorm* SandStorm::instance_ = 0;

#define dustQuadDefaultDir Vector3(1,0,0)
#define dustQuadDefaultUp Vector3(0,1,0)

const float SandStorm::outerBorderX = 150.f;
const float SandStorm::outerBorderZ = 150.f;
SandStorm::DustParticles::QUAD SandStorm::spawnAtQuad;
Vector3* SandStorm::dustDir = new Vector3(0,0,0);
//Mesh* SandStorm::dustQuad = NULL;
SandStorm::SandStorm()
{
	int numOfDusts = 30 + rand() % 20;

	dustDir->x = -1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f + 1.f)));
	dustDir->z = -1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f + 1.f)));
	try
	{
		dustDir->Normalize();
	}
	catch (DivideByZero())
	{
		dustDir->x = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f - .1f)));
		dustDir->z = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f - .1f)));
		dustDir->Normalize();
	}
	checkQuadrants(*dustDir);
	//dustQuad = MeshBuilder::GenerateQuad("Dust" + std::to_string(i), Color(1, 1, 1), 1, 1);


	for (size_t i = 0; i < numOfDusts; i++)
	{
		DustParticles* temp = new DustParticles;
		temp->dustQuad = MeshBuilder::GenerateQuad("Dust" + std::to_string(i), Color(1, 1, 1), 1, 1);
		temp->dustQuad->textureID = LoadTGA("Image//Sandy.tga");
		temp->dustQuad->dir = *dustDir;
		temp->resetDust();
		dust_.push_back(temp);
	}
	rotateAngleDir = Math::RadianToDegree(acos(dustQuadDefaultDir.Dot(*dustDir)));
	try
	{
		rotateAxis = dustQuadDefaultDir.Cross(*dustDir).Normalized();
	}
	catch (DivideByZero())
	{
		rotateAxis = dustQuadDefaultUp;
	}
}
SandStorm::~SandStorm()
{
	delete dustDir;
	delete instance_;
}
SandStorm* SandStorm::getInstance()
{
	if (instance_)
		return instance_;
	else
		return (instance_ = new SandStorm);
}
void SandStorm::update(double dt)
{
	for (auto it : dust_)
	{
		it->dustQuad->pos += it->dustQuad->dir * (float)dt * it->speed;
		if (abs(it->dustQuad->pos.x) > outerBorderX + 5 || abs(it->dustQuad->pos.z) > outerBorderZ + 5)
			it->resetDust();
	}
	for (auto it : Player::getInstance()->enemies_)
	{
		if (it->getHp() < 500)
			it->isHitUpdate(-2);//heals all enemies in the scene with sandstorm at ~120hp / s
		it->setAttSpd(0.5f);
		it->setDmgMultipler(1.5f);
		it->setMoveSpd(1.5f);
	}
}
void SandStorm::render(MS* projectionStack, MS* viewStack, MS* modelStack, unsigned * m_parameters)
{
	glDisable(GL_CULL_FACE);
	
	for (auto it : dust_){
		modelStack->PushMatrix();
		modelStack->Translate(it->dustQuad->pos.x, it->dustQuad->pos.y, it->dustQuad->pos.z);
		modelStack->Rotate(it->rotationUp, it->dustQuad->dir.x, it->dustQuad->dir.y, it->dustQuad->dir.z);
		modelStack->Rotate(rotateAngleDir, rotateAxis.x, rotateAxis.y, rotateAxis.z);
		modelStack->Scale(it->scaleX, it->scaleY, 1);
		RenderMeshClass::RenderMesh(it->dustQuad, false, projectionStack, viewStack, modelStack, m_parameters);
		modelStack->PopMatrix();
	}
	glEnable(GL_CULL_FACE);
}

void SandStorm::DustParticles::resetDust()
{
	this->scaleX = 15.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (40.f - 15.f)));
	this->scaleY = 10.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (25.f - 10.f)));
	this->rotationUp = -75.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (75.f + 75.f)));
	this->speed = 10.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (30.f - 10.f)));
	this->dustQuad->pos.y = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.f - 0.f)));

	if (spawnAtQuad == DustParticles::pXAXIS)
	{
		this->dustQuad->pos.x = 150;
		this->dustQuad->pos.z = -outerBorderZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (outerBorderZ + outerBorderZ)));
	}
	else if (spawnAtQuad == DustParticles::nXAXIS)
	{
		this->dustQuad->pos.x = -150;
		this->dustQuad->pos.z = -outerBorderZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (outerBorderZ + outerBorderZ)));
	}
	else if (spawnAtQuad == DustParticles::pZAXIS)
	{
		this->dustQuad->pos.x = -outerBorderX + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (outerBorderX + outerBorderX)));
		this->dustQuad->pos.z = outerBorderZ;
	}
	else if (spawnAtQuad == DustParticles::nZAXIS)
	{
		this->dustQuad->pos.x = -outerBorderX + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (outerBorderX + outerBorderX)));
		this->dustQuad->pos.z = -outerBorderZ;
	}
	else
	{
		if (rand() % 2 == 0)
		{
			//Spawn at axis parallel to x-axis
			this->dustQuad->pos.x = -outerBorderX + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (outerBorderX + outerBorderX)));
			if (spawnAtQuad == DustParticles::QUADRANT1 || spawnAtQuad == DustParticles::QUADRANT2)//z is always positive at these 2 quads
				this->dustQuad->pos.z = 150.f;
			else
				this->dustQuad->pos.z = -150.f;
		}
		else
		{
			//Spawn at axis parallel to z-axis
			this->dustQuad->pos.z = -outerBorderZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (outerBorderZ + outerBorderZ)));
			if (spawnAtQuad == DustParticles::QUADRANT1 || spawnAtQuad == DustParticles::QUADRANT4)//z is always positive at these 2 quads
				this->dustQuad->pos.x = 150.f;
			else
				this->dustQuad->pos.x = -150.f;
		}
	}
}
void SandStorm::checkQuadrants(Vector3 randDir)
{
	if (randDir.x < 0 && randDir.z < 0)
		spawnAtQuad = DustParticles::QUADRANT1;
	else if (randDir.x > 0 && randDir.z < 0)
		spawnAtQuad = DustParticles::QUADRANT2;
	else if (randDir.x > 0 && randDir.z > 0)
		spawnAtQuad = DustParticles::QUADRANT3;
	else if (randDir.x < 0 && randDir.z > 0)
		spawnAtQuad = DustParticles::QUADRANT4;
	else if (randDir.x < 0 && randDir.z == 0)
		spawnAtQuad = DustParticles::pXAXIS;
	else if (randDir.x > 0 && randDir.z == 0)
		spawnAtQuad = DustParticles::nXAXIS;
	else if (randDir.x == 0 && randDir.z < 0)
		spawnAtQuad = DustParticles::pZAXIS;
	else if (randDir.x == 0 && randDir.z > 0)
		spawnAtQuad = DustParticles::nZAXIS;
	//spawnAtQuad
}