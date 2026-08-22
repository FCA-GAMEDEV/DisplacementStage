#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace std;

class ShaderManager
{
private:
    ShaderManager(void);
    ShaderManager(const ShaderManager&) {}
    ShaderManager& operator=(const ShaderManager&) { return *this; }
    ~ShaderManager(void);

    void createShader(void);
    void createVertexShader(void);
    void createTessControlShader(void);
    void createTessEvaluationShader(void);
    void createGeometryShader(void);
    void createFragmentShader(void);
    void compileShader(void);
    void addShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

    void createSimpleShader(void);
    void createSimpleVertexShader(void);
    void createSimpleFragmentShader(void);
    void compileSimpleShader(void);
    void addSimpleShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
    void compileColorShader(void);

    static ShaderManager* instance;

    const char* pVertexShader;
    const char* pTessControlShader;
    const char* pTessEvaluationShader;
    const char* pGeometryShader;
    const char* pFragmentShader;

    GLuint ShaderProgram  = 0;
    GLuint ShaderObject   = 0;

    GLuint samplerDisplacemetMap = 0;
    GLuint samplerTexture        = 0;
    GLuint tessLevelInner        = 0;
    GLuint tessLevelOuter        = 0;
    GLint  mvpLocation           = -1;       // uniform MVP (tess shader)

    const char* pSimpleVertexShader   = nullptr;
    const char* pSimpleFragmentShader = nullptr;

    GLuint simnpleShaderProgram  = 0;
    GLuint simpleShaderObject    = 0;
    GLuint simpleSamplerTexture  = 0;
    GLint  simpleMvpLocation     = -1;      // uniform MVP (simple shader)

    GLuint colorShaderProgram    = 0;       // [NEW] flat color shader
    GLint  colorMvpLocation      = -1;      // [NEW] uniform MVP
    GLint  colorColorLocation    = -1;      // [NEW] uniform Color

public:
    enum Shader_Type { SHADER_SIMPLE, SHADER_TESS, SHADER_COLOR };

    static ShaderManager& getInstance(void);
    void startShader(Shader_Type ID = SHADER_SIMPLE);
    void endShader(void);

    void setMVP(const glm::mat4& mvp);      // envia para o shader ativo
    void setColor(const glm::vec4& color);  // envia para o shader de cor ativa

    GLuint getTessLevelInner(void);
    GLuint getTessLevelOuter(void);
    GLuint getSamplerDisplacementMap(void);
    GLuint getSamplerTexture(void);
    GLuint getSimpleSamplerTexture(void);
};
