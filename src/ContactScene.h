#pragma once

#include "Scene.h"
#include "SceneManager.h"

class ContactScene : public Scene
{
public:
	ContactScene(DisplacementStage * displacementStage);
	~ContactScene(void);
	void update(void);
	void draw(void);
	void keyPressed(int key);
	void keyReleased(int key);

private:
	void setup(void);

	float radius;
	float mouse[2];
	float depth;
	int pass;
	float time;
	bool dir;
	bool vertical;
};

