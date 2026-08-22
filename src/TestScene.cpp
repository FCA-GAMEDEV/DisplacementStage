#include "TestScene.h"


TestScene::TestScene(DisplacementStage * displacementStage)
	: Scene(displacementStage)
{
	//cout << "TestScene::Construtor" << endl;

	this->terrain->setCameraPosition(0,40,60);
	this->terrain->setCameraTarget(0,0,0);

	this->displacementMap = new DisplacementMap("data/testMap.png");

	this->interoperability->registerDisplacementMap(this->displacementMap->getID(), this->displacementStage);
	
	this->displacementStage->setListSize(this->displacementMap->getWidth() * this->displacementMap->getHeight());
	this->displacementStage->setKernel("testKernel");
	this->displacementStage->createMemoryBufferToCustomKernel();
	this->displacementStage->setCustomArg();

	this->decorator = new Interface;
}

TestScene::~TestScene(void)
{
	//cout << "TestScene::Destruidor" << endl;

	if (this->decorator != NULL)
	{
		delete this->decorator;
		this->decorator = NULL;
	}

		this->displacementStage->releaseCustomBuffer();
}

void TestScene::update(void)
{
	//cout << "TestScene::update" << endl;

	//if (!this->bPause)
	//{

	this->decorator->update();
	
	if (this->decorator->getUpdate())
	{
		float mouse[4] = {this->decorator->getMouse()[0], this->decorator->getMouse()[1], 0, 0};
		float normal[4] = {this->decorator->getNormal()[0], this->decorator->getNormal()[1], this->decorator->getNormal()[2], 0};
		float radius[4] = {this->decorator->getRadius(),0,0,0};
		float null[4] = {0};

		//float mouse[4] = {rand()%64, rand()%64, 0, 0};
		//float normal[4] = {(rand()%3-1)/255.f, (rand()%3-1)/255.f, (rand()%3-1)/255.f, 0};
		//float radius[4] = {rand()%15+1,0,0,0};
		//float null[4] = {0};

		this->displacementStage->copyCustomToMemory(mouse, normal, radius, null);
		this->displacementStage->runKernel();
	}
	//}
}

void TestScene::draw(void)
{
	//cout << "TestScene::draw" << endl;

	this->terrain->draw(this->shaderManager, this->texture, this->displacementMap);

	this->decorator->draw(this->shaderManager, this->displacementMap, true);
}


void TestScene::keyPressed(int key)
{
	this->decorator->keyPressed(key);

	Scene::keyPressed(key);
}


void TestScene::keyReleased(int key)
{
	this->decorator->keyReleased(key);

	if (key == ' ')
		SceneManager::getInstance().changeScene();

	Scene::keyReleased(key);
}

void TestScene::mouseMoved(int x, int y)
{
	this->decorator->mouseMoved(x,y);
}


void TestScene::mouseDragged(int x, int y, int button)
{
	this->decorator->mouseDragged(x, y, button);
}


void TestScene::mousePressed(int x, int y, int button)
{
	this->decorator->mousePressed(x, y, button);
}


void TestScene::mouseReleased(int x, int y, int button)
{
	this->decorator->mouseReleased(x, y, button);
}