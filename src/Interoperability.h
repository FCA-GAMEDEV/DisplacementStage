#pragma once

#include <GL/glew.h>
#include "DisplacementStage.h"

class DisplacementStage;

class Interoperability
{
public:
    Interoperability(void);
    ~Interoperability(void);

    void registerDisplacementMap (GLuint index, DisplacementStage* displacementStage);
    void registerSourceMorphing  (GLuint index, DisplacementStage* displacementStage);
    void registerDestinyMorphing (GLuint index, DisplacementStage* displacementStage);
};
