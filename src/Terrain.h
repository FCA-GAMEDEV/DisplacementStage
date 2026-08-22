#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include "ShaderManager.h"
#include "Texture.h"
#include "DisplacementMap.h"

class Terrain
{
public:
    Terrain(void);
    ~Terrain(void);

    void draw(ShaderManager* shaderManager, Texture* texture, DisplacementMap* displacementMap);

    void setRotate(bool rotate);
    bool getRotate(void);
    void setWireframe(bool wireframe);
    bool getWireframe(void);
    void setCameraPosition(float x, float y, float z);
    void setCameraTarget(float x, float y, float z);
    void setTessellationFactor(int inner, int outer);
    void increaseTessellationFactor(int innerPass, int outerPass);
    void decreaseTessellationFactor(int innerPass, int outerPass);
    void setAngle(float angle);
    float getAngle(void);
    void increaseCameraPosition(float xPass, float yPass, float zPass);
    void decreaseCameraPosition(float xPass, float yPass, float zPass);

private:
    void initVAO(void);

    int   tessLevelInner;
    int   tessLevelOuter;

    float x, y, z;      // posição da câmera
    float tx, ty, tz;   // alvo da câmera

    bool  bWireframe;
    bool  bRotate;
    float angle;

    // VAO/VBO para o plano de 2 triângulos
    GLuint vao = 0;
    GLuint vbo = 0;
};
