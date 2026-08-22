#pragma once

#include "Scene.h"
#include "SceneManager.h"
#include "Texture.h"

class MenuScene : public Scene
{
public:
	MenuScene(DisplacementStage * displacementStage);
	~MenuScene(void);
	void update(void);
	void draw(void);
	void keyPressed(int key);
	void keyReleased(int key);

private:
	Texture* image = nullptr;
	GLuint menuVAO = 0;
	GLuint menuVBO = 0;

	void initQuad(void);
};
