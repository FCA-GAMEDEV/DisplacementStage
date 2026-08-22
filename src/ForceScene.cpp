#include "ForceScene.h"


ForceScene::ForceScene(DisplacementStage * displacementStage)
	: Scene(displacementStage)
{
	//cout << "ForceScene::Construtor" << endl;

	this->FORCE = 5.f;

	this->start = 0;//ofGetElapsedTimeMillis();
	this->end = 0;//ofGetElapsedTimeMillis();
	this->mouse[0] = this->mouse[1] = 0;
	this->force[0] = this->force[1] = this->force[2] = 0;
	this->force[3] = this->FORCE;
	this->count = 0;

	//this->terrain->setWireframe(false);
	//this->terrain->setRotate(false);
	this->terrain->setCameraPosition(0,30,60);
	this->terrain->setCameraTarget(0,0,0);

	this->decorator = new Interface;
	
	this->setup();
}

ForceScene::~ForceScene(void)
{
	//cout << "ForceScene::Destruidor" << endl;

	this->displacementStage->releaseForceBuffer();
}

void ForceScene::setup(void)
{
	this->displacementMap = new DisplacementMap("data/forceMap.png");

	this->interoperability->registerDisplacementMap(this->displacementMap->getID(), this->displacementStage);

	this->displacementStage->setListSize(this->displacementMap->getWidth() * this->displacementMap->getHeight());
	this->displacementStage->setKernel(this->displacementStage->KRNL_FORCE);
	this->displacementStage->createMemoryBufferToForcetKernel();
	this->displacementStage->setForceArg();
}

void ForceScene::update(void)
{
	//cout << "ForceScene::update" << endl;

	if (!this->bPause)
	{	
		this->end += TimeManager::getInstance().getulDeltaTime(); //ofGetElapsedTimeMillis();

		if (end - start > 500)
		{
			//if (count < 5)
			{
				do {
					this->force[0] = (rand() % 2001 - 1000) / 1000.f;
					this->force[1] = (rand() % 2001 - 1000) / 1000.f;
					this->force[2] = (rand() % 2001 - 1000) / 1000.f;
					//this->force[2] = (rand() %  751  + 250) / 1000.f;
				} while (this->force[0] < 0.2f && this->force[0] > -0.2f ||
						 this->force[1] < 0.2f && this->force[1] > -0.2f ||
						 this->force[2] < 0.2f && this->force[2] > -0.2f);	
			}
			//else
			//{
			//	do {
			//		this->force[0] = (rand() % 2001 - 1000) / 1000.f;
			//		this->force[1] = (rand() % 2001 - 1000) / 1000.f;
			//		this->force[2] = (rand() %  751 - 1000) / 1000.f;
			//	} while (this->force[0] < 0.2f && this->force[0] > -0.2f ||
			//			 this->force[1] < 0.2f && this->force[1] > -0.2f ||
			//			 this->force[2] < 0.2f && this->force[2] > -0.2f);	
			//}

			this->mouse[0]  = (1 + this->count % 3) << 4;
			this->mouse[1]  = (1 + this->count / 3) << 4;

			this->displacementStage->copyForceToMemory(this->mouse, this->force);
			this->displacementStage->runKernel();

			this->start = this->end;
			this->count++;
		}

		if (this->count > 9)
		{
			this->count = 0;
			this->setup();
		}
	}
}

void ForceScene::draw(void)
{
	//cout << "ForceScene::draw" << endl;

	this->terrain->draw(this->shaderManager, this->texture, this->displacementMap);

	Scene::draw();
}


void ForceScene::keyPressed(int key)
{
	Scene::keyPressed(key);
}


void ForceScene::keyReleased(int key)
{
	if (key == ' ') 
		SceneManager::getInstance().changeScene();

	Scene::keyReleased(key);
}
