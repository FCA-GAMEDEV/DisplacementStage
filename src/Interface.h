#pragma once

#include <GL/glew.h>
#include "ShaderManager.h"
#include "DisplacementMap.h"

class Interface
{
public:
    Interface(void);
    ~Interface(void);
    void update(void);
    void draw(ShaderManager* shaderManager, DisplacementMap* displacementMap, bool drawCircle = false);
    void keyPressed   (int key);
    void keyReleased  (int key);
    void mouseMoved   (int x, int y);
    void mouseDragged (int x, int y, int button);
    void mousePressed (int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseScrolled(double xoffset, double yoffset);

    int*   getMouse(void);
    int    getRadius(void);
    float* getNormal(void);
    bool   getUpdate(void);

private:
    enum { RED, GREEN, BLUE };

    int   mouse[2];
    int   radius;
    int   color;
    float normal[3];
    int   button;
    bool  bUpdate;

    // VAO/VBO para o quad do mini-mapa 2D
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    void paint(int x, int y, int button);
    void initQuad(void);
};
