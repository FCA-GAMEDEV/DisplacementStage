#pragma once

#include "Scene.h"
#include "SceneManager.h"
#include "TimeManager.h"

class CustomScene : public Scene
{
public:
	CustomScene(DisplacementStage * displacementStage);
	virtual ~CustomScene(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void keyPressed(int key);
	virtual void keyReleased(int key);

private:
	//Texture * morphingStart;
	//Texture * morphingEnd;

	//float angle;
	//float time;
};
