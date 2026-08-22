#include "ContactScene.h"


ContactScene::ContactScene(DisplacementStage * displacementStage)
	: Scene(displacementStage)
{
	//cout << "ContactScene::Construtor" << endl;

	this->radius = 2;
	this->mouse[0] = radius + 2;
	this->mouse[1] = radius + 2;
	this->depth = 0.02f;
	this->pass = 0;
	this->time = 0;
	this->dir = true;
	this->vertical = false;

	this->decorator = new Interface;

	//this->terrain->setWireframe(false);
	//this->terrain->setRotate(false);
	this->terrain->setCameraPosition(0,40,60);
	this->terrain->setCameraTarget(0,0,0);

	this->setup();
}

ContactScene::~ContactScene(void)
{
	//cout << "ContactScene::Destruidor" << endl;

	this->displacementStage->releaseContactBuffer();
}


void ContactScene::setup(void)
{
	this->displacementMap = new DisplacementMap("data/textures/contactMap.png");

	this->interoperability->registerDisplacementMap(this->displacementMap->getID(), this->displacementStage);

	this->displacementStage->setListSize(this->displacementMap->getWidth() * this->displacementMap->getHeight());
	this->displacementStage->setKernel(this->displacementStage->KRNL_CONTACT);
	this->displacementStage->createMemoryBufferToContactKernel();
	this->displacementStage->setContactArg();
}

void ContactScene::update(void)
{
	//cout << "CustomScene::update" << endl;

	if (!this->bPause)
	{	
		TimeManager * timeManager = &TimeManager::getInstance();

		if (this->dir) this->time += timeManager->getfDeltaTime();
		else this->time -= timeManager->getfDeltaTime();

		if (this->time < 0)
		{
			this->time = 0;
			this->dir = !this->dir;
			this->pass++;
		}
		else if (this->time > 1)
		{
			this->time = 1;
			this->dir = !this->dir;
			this->pass++;
		}

		if (this->vertical)
		{
			if (this->pass > 3)
			{
				this->mouse[0]  += 2 + this->radius * 2;
				this->pass = 0;
				//this->terrain->setWireframe(!this->terrain->getWireframe());
			}
			this->mouse[1]  = this->radius + 2 + this->time * (64 - 4 - 2 * this->radius);
		}
		else
		{
			if (this->pass > 3)
			{
				this->mouse[1]  += 2 + radius * 2;
				this->pass = 0;
				//this->terrain->setWireframe(!this->terrain->getWireframe());
			}
			this->mouse[0]  = this->radius + 2 + this->time * (64 - 4 - 2 * this->radius);
		}

		if (this->mouse[0] > 62 || this->mouse[1] > 62)
		{
			this->mouse[0] = this->mouse[1] = this->radius+2;
			this->vertical = !this->vertical;
			if (!this->vertical)
			{
				this->setup();
				//this->terrain->setRotate(!this->terrain->getRotate());
			}
		}
	
		this->displacementStage->copyContactToMemory(this->mouse, &this->depth, &this->radius);
		this->displacementStage->runKernel();
	}
}

void ContactScene::draw(void)
{
	//cout << "ContactScene::draw" << endl;

	this->terrain->draw(this->shaderManager, this->texture, this->displacementMap);

	Scene::draw();
}


void ContactScene::keyPressed(int key)
{
	Scene::keyPressed(key);
}


void ContactScene::keyReleased(int key)
{
	if (key == ' ') 
		SceneManager::getInstance().changeScene();

	Scene::keyReleased(key);
}
