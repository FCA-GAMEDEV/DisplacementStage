#pragma once

#include "Scene.h"
#include "SceneManager.h"
#include "TimeManager.h"

class ForceScene : public Scene
{
public:
	ForceScene(DisplacementStage * displacementStage);
	~ForceScene(void);
	void setup(void);
	void update(void);
	void draw(void);
	void keyPressed(int key);
	void keyReleased(int key);

private:
	long start;
	long end;
	float mouse[2];
	float force[4];
	unsigned short count;

	float FORCE;
};

