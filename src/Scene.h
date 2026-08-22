#pragma once


#include <iostream>
#include "ShaderManager.h"
#include "Terrain.h"
#include "Texture.h"
#include "DisplacementMap.h"
#include "DisplacementStage.h"
#include "TimeManager.h"
#include "Interoperability.h"
#include "Interface.h"

using namespace std;

class Scene
{
public:
	Scene(DisplacementStage * displacementStage);
	virtual ~Scene(void);

	virtual void update(void)=0;
	virtual void draw(void);
	virtual void keyPressed(int key);
	virtual void keyReleased(int key);
	virtual void mouseMoved(int x, int y );
	virtual void mouseDragged(int x, int y, int button);
	virtual void mousePressed(int x, int y, int button);
	virtual void mouseReleased(int x, int y, int button);
	virtual void mouseScrolled(double xoffset, double yoffset);
	virtual void clear(void);

protected:
	static DisplacementStage * displacementStage;
	static ShaderManager * shaderManager;
	static Terrain * terrain;
	static Texture * texture;
	static DisplacementMap * displacementMap;
	static Interoperability * interoperability;

	bool bPause;

	Interface * decorator;

	bool bShowDecorator;
};

