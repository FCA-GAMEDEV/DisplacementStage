#include "SceneManager.h"


SceneManager * SceneManager::instance = NULL;


SceneManager::SceneManager(void)
{
	this->scene = NULL;
	this->displacementStage = NULL;
	this->setup();
}


SceneManager::~SceneManager(void)
{
	if (this->displacementStage)
	{
		delete this->displacementStage;
		this->displacementStage = NULL;
	}

	if (this->scene)
	{
		this->scene->clear();
		delete this->scene;
		this->scene = NULL;
	}

	if (SceneManager::instance != NULL)
	{
		delete SceneManager::instance;
		SceneManager::instance = NULL;
	}
}


SceneManager & SceneManager::getInstance()
{
	if (!SceneManager::instance)
		SceneManager::instance = new SceneManager;
	return * SceneManager::instance;
}


void SceneManager::setup(void)
{
	//for (int i = 0; i < 1000; i++)
	//{
	//	if (this->displacementStage)
	//	{
	//		delete this->displacementStage;
	//		this->displacementStage = NULL;
	//	}
		this->displacementStage = new DisplacementStage("data/displacementStage.cl");
		
	//}

	this->scene = new MenuScene(this->displacementStage);
	this->currentScene = this->SCN_MENU;
}


void SceneManager::update(void)
{
	this->scene->update();
}


void SceneManager::draw(void)
{
	this->scene->draw();
}


void SceneManager::changeScene(int value)
{
	if (this->scene != NULL)
	{
		delete this->scene;
		this->scene = NULL;
	}

	switch (this->currentScene)
	{
		case this->SCN_MENU:
			switch (value)
			{
				case 1: // CONTACT SCENE					
					this->scene = new ContactScene(this->displacementStage);
					this->currentScene = this->SCN_CONTACT;
					break;

				case 2: // FORCE SCENE
					this->scene = new ForceScene(this->displacementStage);
					this->currentScene = this->SCN_FORCE;
					break;

				case 3: // MORPHING SCENE
					this->scene = new MorphingScene(this->displacementStage);
					this->currentScene = this->SCN_MORPHING;
					break;

				case 4: // CUSTOM SCENE
					this->scene = new CustomScene(this->displacementStage);
					this->currentScene = this->SCN_CUSTOM;
					break;

				case 5: // TEST SCENE
					this->scene = new TestScene(this->displacementStage);
					this->currentScene = this->SCN_TEST;
					break;
			}
			break;

		case this->SCN_CONTACT:
		case this->SCN_FORCE:
		case this->SCN_MORPHING:
		case this->SCN_CUSTOM:
		case this->SCN_TEST:
			this->scene = new MenuScene(this->displacementStage);
			this->currentScene = this->SCN_MENU;
			break;
	}
}


SceneManager::SCENE_TYPE SceneManager::getCurrentScene(void)
{
	return this->currentScene;
}


DisplacementStage & SceneManager::getDisplacementStage(void)
{
	return * this->displacementStage;
}


void SceneManager::keyPressed(int key)
{
	this->scene->keyPressed(key);
}


void SceneManager::keyReleased(int key)
{
	this->scene->keyReleased(key);
}


void SceneManager::mouseMoved(int x, int y)
{
	this->scene->mouseMoved(x,y);
}


void SceneManager::mouseDragged(int x, int y, int button)
{
	this->scene->mouseDragged(x,y,button);
}


void SceneManager::mousePressed(int x, int y, int button)
{
	this->scene->mousePressed(x,y,button);
}


void SceneManager::mouseReleased(int x, int y, int button)
{
	this->scene->mouseReleased(x,y,button);
}

void SceneManager::mouseScrolled(double xoffset, double yoffset)
{
	this->scene->mouseScrolled(xoffset, yoffset);
}
