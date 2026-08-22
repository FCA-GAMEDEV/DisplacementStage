#include "CustomScene.h"


CustomScene::CustomScene(DisplacementStage * displacementStage)
	: Scene(displacementStage)
{
	//cout << "CustomScene::Construtor" << endl;

	//this->angle = 0;
	//this->time = 0;

	////this->terrain->setWireframe(false);
	////this->terrain->setRotate(false);
	this->terrain->setCameraPosition(0,20,60);
	this->terrain->setCameraTarget(0,0,0);

	//this->morphingStart = new Texture("sourceMorphing.png");
	//this->morphingEnd   = new Texture("destinyMorphing.png");
	this->displacementMap = new DisplacementMap("data/customMap.png");

	//this->interoperability->registerSourceMorphing( this->morphingStart->getID(),   this->displacementStage);
	//this->interoperability->registerDestinyMorphing(this->morphingEnd->getID(),     this->displacementStage);
	this->interoperability->registerDisplacementMap(this->displacementMap->getID(), this->displacementStage);
	
	this->displacementStage->setListSize(this->displacementMap->getWidth() * this->displacementMap->getHeight());
	this->displacementStage->setKernel(this->displacementStage->KRNL_CUSTOM);
	this->displacementStage->createMemoryBufferToCustomKernel();
	this->displacementStage->setCustomArg();

	this->decorator = new Interface;
}

CustomScene::~CustomScene(void)
{
	//cout << "CustomScene::Destruidor" << endl;

	//if (this->morphingStart != NULL)
	//{
	//	delete this->morphingStart;
	//	this->morphingStart = NULL;
	//}

	//if (this->morphingEnd != NULL)
	//{
	//	delete this->morphingEnd;
	//	this->morphingEnd = NULL;
	//}

	this->displacementStage->releaseCustomBuffer();
}

void CustomScene::update(void)
{
	//cout << "MorphingScene::update" << endl;

	if (!this->bPause)
	{	
	//	this->time = sinf(this->angle += TimeManager::getInstance().getfDeltaTime());

		float null[4] = {0};

		this->displacementStage->copyCustomToMemory(null,null,null,null);
		this->displacementStage->runKernel();
	}
}

void CustomScene::draw(void)
{
	//cout << "MorphingScene::draw" << endl;
	
	this->terrain->draw(this->shaderManager, this->texture, this->displacementMap);

	Scene::draw();
}


void CustomScene::keyPressed(int key)
{
	Scene::keyPressed(key);
}


void CustomScene::keyReleased(int key)
{
	if (key == ' ') 
		SceneManager::getInstance().changeScene();

	Scene::keyReleased(key);
}
