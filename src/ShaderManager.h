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
    GLuint compileShader(const string& vs, const string& tcs, const string& tes, const string& gs, const string& fs);
    void addShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

    void createSimpleShader(void);
    void compileSimpleShader(const string& vs, const string& fs);
    void addSimpleShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
    
    void compileColorShader(void);

    static ShaderManager* instance;

    GLuint ShaderProgramEqual = 0;
    GLuint ShaderProgramEven  = 0;
    GLuint ShaderProgramOdd   = 0;
    int    currentScheme      = 1;

    GLuint ShaderProgram  = 0;
    GLuint ShaderObject   = 0;

    GLuint samplerDisplacementMap = 0;
    GLuint samplerTexture        = 0;
    GLuint tessLevelInner        = 0;
    GLuint tessLevelOuter        = 0;
    GLint  mvpLocation           = -1;       // uniform MVP (tess shader)

    GLuint simpleShaderProgram  = 0;
    GLuint simpleShaderObject    = 0;
    GLuint simpleSamplerTexture  = 0;
    GLint  simpleMvpLocation     = -1;      // uniform MVP (simple shader)

    GLuint colorShaderProgram    = 0;       // flat color shader
    GLint  colorMvpLocation      = -1;      // uniform MVP
    GLint  colorColorLocation    = -1;      // uniform Color

public:
    enum Shader_Type { SHADER_SIMPLE, SHADER_TESS, SHADER_COLOR };

    static ShaderManager& getInstance(void);
    void startShader(Shader_Type ID = SHADER_SIMPLE);
    void endShader(void);

    void setMVP(const glm::mat4& mvp);      // envia para o shader ativo
    void setModel(const glm::mat4& model);  // envia a matriz Model para o shader ativo
    void setColor(const glm::vec4& color);  // envia para o shader de cor activa

    void setTessellationScheme(int scheme); // seleciona o esquema de partição (0=equal, 1=even, 2=odd)
    int  getTessellationScheme(void);       // retorna o esquema de partição ativo

    void reloadShaders(void);
    static std::string loadFile(const std::string& path);

    GLuint getTessLevelInner(void);
    GLuint getTessLevelOuter(void);
    GLuint getSamplerDisplacementMap(void);
    GLuint getSamplerTexture(void);
    GLuint getSimpleSamplerTexture(void);
};
