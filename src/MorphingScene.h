#pragma once

#include "Scene.h"
#include "SceneManager.h"
#include "TimeManager.h"

class MorphingScene : public Scene
{
public:
	MorphingScene(DisplacementStage * displacementStage);
	~MorphingScene(void);
	void update(void);
	void draw(void);
	void keyPressed(int key);
	void keyReleased(int key);

private:
	Texture * morphingStart;
	Texture * morphingEnd;

	float angle;
	float time;
};

