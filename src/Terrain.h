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

    void draw(ShaderManager* shaderManager, Texture* texture, DisplacementMap* displacementMap, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    void setRotate(bool rotate);
    bool getRotate(void);
    void setWireframe(bool wireframe);
    bool getWireframe(void);
    void setCullFace(bool cullFace);
    bool getCullFace(void);
    void setTessellationFactor(int inner, int outer);
    void increaseTessellationFactor(int innerPass, int outerPass);
    void decreaseTessellationFactor(int innerPass, int outerPass);
    void setAngle(float angle);
    float getAngle(void);

private:
    void initVAO(void);

    int   tessLevelInner;
    int   tessLevelOuter;

    bool  bWireframe;
    bool  bRotate;
    bool  bCullFace;
    float angle;

    // VAO/VBO para o plano de 2 triângulos
    GLuint vao = 0;
    GLuint vbo = 0;
};
