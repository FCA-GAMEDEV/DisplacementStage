#pragma once

#include <GL/glew.h>
#include "ShaderManager.h"
#include "DisplacementMap.h"
#include <string>

class Interface
{
public:
    Interface(void);
    ~Interface(void);
    void update(void);
    void draw(ShaderManager* shaderManager, DisplacementMap* displacementMap, bool drawCircle = false, bool isWireframe = false, bool isRotate = false, bool isCullFace = true, int tessScheme = 1, bool drawMinimap = false);
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
    bool   checkAndResetWireframeToggle(void);
    bool   checkAndResetRotationToggle(void);
    bool   checkAndResetCullFaceToggle(void);
    bool   checkAndResetEvenSpacingToggle(void);
    bool   checkAndResetOddSpacingToggle(void);

private:
    enum { RED, GREEN, BLUE };

    int   mouse[2];
    int   radius;
    int   color;
    float normal[3];
    int   button;
    bool  bUpdate;
    bool  bToggleWireframe = false;
    bool  bToggleRotation = false;
    bool  bToggleCullFace = false;
    bool  bToggleEvenSpacing = false;
    bool  bToggleOddSpacing = false;

    // VAO/VBO para o quad do mini-mapa 2D
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    GLuint circleVAO = 0;
    GLuint circleVBO = 0;
    GLuint rectVAO = 0;
    GLuint rectVBO = 0;

    void paint(int x, int y, int button);
    void initQuad(void);
    void drawRect(ShaderManager* shaderManager, float x, float y, float w, float h, const glm::vec4& color);
    void drawStrokeText(ShaderManager* shaderManager, const std::string& text, float x, float y, float char_w, float char_h, float spacing, const glm::vec4& color);
};
