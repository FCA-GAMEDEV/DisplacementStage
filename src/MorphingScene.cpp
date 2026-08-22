#include "MorphingScene.h"


MorphingScene::MorphingScene(DisplacementStage * displacementStage)
	: Scene(displacementStage)
{
	//cout << "MorphingScene::Construtor" << endl;

	this->angle = 0;
	this->time = 0;

	//this->terrain->setWireframe(false);
	//this->terrain->setRotate(false);
	this->terrain->setCameraPosition(0,20,60);
	this->terrain->setCameraTarget(0,0,0);

	//this->morphingStart = NULL;
	//for (int i = 0; i < 1000; i++)
	//{
	//	if (this->morphingStart != NULL)
	//	{
	//		delete this->morphingStart;
	//		this->morphingStart = NULL;
	//	}
	//	this->morphingStart = new Texture("testMap.png");
	//}

	this->morphingStart = new Texture("data/textures/sourceMorphing.png");
	this->morphingEnd   = new Texture("data/textures/destinyMorphing.png");
	this->displacementMap = new DisplacementMap("data/textures/morphingMap.png");

	//for (int i = 0; i < 1000; i++)
	//	this->interoperability->registerSourceMorphing( this->morphingStart->getID(),   this->displacementStage);

	this->interoperability->registerSourceMorphing( this->morphingStart->getID(),   this->displacementStage);
	this->interoperability->registerDestinyMorphing(this->morphingEnd->getID(),     this->displacementStage);
	this->interoperability->registerDisplacementMap(this->displacementMap->getID(), this->displacementStage);
	
	this->displacementStage->setListSize(this->displacementMap->getWidth() * this->displacementMap->getHeight());
	this->displacementStage->setKernel(this->displacementStage->KRNL_MORPHING);
	this->displacementStage->createMemoryBufferToMorphingtKernel();
	this->displacementStage->setMorphingArg();

	this->decorator = new Interface;
}

MorphingScene::~MorphingScene(void)
{
	//cout << "MorphingScene::Destruidor" << endl;

	if (this->morphingStart != NULL)
	{
		delete this->morphingStart;
		this->morphingStart = NULL;
	}

	if (this->morphingEnd != NULL)
	{
		delete this->morphingEnd;
		this->morphingEnd = NULL;
	}

	this->displacementStage->releaseMorphingBuffer();
}

void MorphingScene::update(void)
{
	//cout << "MorphingScene::update" << endl;

	if (!this->bPause)
	{	
		this->time = sinf(this->angle += TimeManager::getInstance().getfDeltaTime());

		this->displacementStage->copyMorphingToMemory(&this->time);
		this->displacementStage->runKernel();
	}
}

void MorphingScene::draw(void)
{
	//cout << "MorphingScene::draw" << endl;
	
	this->terrain->draw(this->shaderManager, this->texture, this->displacementMap);

	Scene::draw();
}


void MorphingScene::keyPressed(int key)
{
	Scene::keyPressed(key);
}


void MorphingScene::keyReleased(int key)
{
	if (key == ' ') 
		SceneManager::getInstance().changeScene();

	Scene::keyReleased(key);
}
