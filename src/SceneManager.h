#pragma once

#include <iostream>
#include "Scene.h"
#include "MenuScene.h"
#include "ContactScene.h"
#include "ForceScene.h"
#include "MorphingScene.h"
#include "CustomScene.h"
#include "TestScene.h"

using namespace std;

class SceneManager
{
public:
	enum SCENE_TYPE
	{
		SCN_MENU,
		SCN_CONTACT,
		SCN_FORCE,
		SCN_MORPHING,
		SCN_CUSTOM,
		SCN_TEST
	};

private:
	SceneManager(void);
	SceneManager(const SceneManager& other) {}
	SceneManager& operator=(const SceneManager& other) {}
	~SceneManager(void);
	void setup(void);

//public:
	static SceneManager * instance;
	Scene * scene;
	SCENE_TYPE currentScene;

	DisplacementStage * displacementStage;

public:
	static SceneManager & getInstance(void);
	void update(void);
	void draw(void);
	void changeScene(int value = 0);
	SCENE_TYPE getCurrentScene(void);
	DisplacementStage & getDisplacementStage(void);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(double xoffset, double yoffset);
};

