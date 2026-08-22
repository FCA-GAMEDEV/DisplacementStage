#pragma once

#include "Scene.h"
#include "SceneManager.h"


class TestScene : public Scene
{
public:
	TestScene(DisplacementStage * displacementStage);
	~TestScene(void);
	void update(void);
	void draw(void);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
};

