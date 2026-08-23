#include "ShaderManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <cstdio>
#include <fstream>

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager::ShaderManager(void)
{
    this->createShader();
    this->createSimpleShader();
    this->compileColorShader();
}

ShaderManager::~ShaderManager(void)
{
    if (ShaderProgramEqual) { glDeleteProgram(ShaderProgramEqual); ShaderProgramEqual = 0; }
    if (ShaderProgramEven)  { glDeleteProgram(ShaderProgramEven);  ShaderProgramEven = 0; }
    if (ShaderProgramOdd)   { glDeleteProgram(ShaderProgramOdd);   ShaderProgramOdd = 0; }
    ShaderProgram = 0;

    if (simpleShaderProgram) {
        glDeleteProgram(simpleShaderProgram);
        simpleShaderProgram = 0;
    }
    if (colorShaderProgram) {
        glDeleteProgram(colorShaderProgram);
        colorShaderProgram = 0;
    }
    
    if (ShaderManager::instance) {
        delete ShaderManager::instance;
        ShaderManager::instance = nullptr;
    }
}

ShaderManager& ShaderManager::getInstance()
{
    if (!ShaderManager::instance)
        ShaderManager::instance = new ShaderManager;
    return *ShaderManager::instance;
}

std::string ShaderManager::loadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ShaderManager::loadFile: Erro ao abrir arquivo '" << path << "'\n";
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ShaderManager::reloadShaders(void)
{
    std::cout << "[ShaderManager] Recarregando shaders de arquivos externos...\n";

    // Liberar programas anteriores
    if (ShaderProgramEqual) { glDeleteProgram(ShaderProgramEqual); ShaderProgramEqual = 0; }
    if (ShaderProgramEven)  { glDeleteProgram(ShaderProgramEven);  ShaderProgramEven = 0; }
    if (ShaderProgramOdd)   { glDeleteProgram(ShaderProgramOdd);   ShaderProgramOdd = 0; }
    ShaderProgram = 0;

    if (simpleShaderProgram) {
        glDeleteProgram(simpleShaderProgram);
        simpleShaderProgram = 0;
    }
    if (colorShaderProgram) {
        glDeleteProgram(colorShaderProgram);
        colorShaderProgram = 0;
    }

    this->createShader();
    this->createSimpleShader();
    this->compileColorShader();
    
    std::cout << "[ShaderManager] Shaders recarregados com sucesso.\n";
}

// ═══════════════════════════════════════════════════════════════
// SHADER PRINCIPAL (Tessellation + Displacement)
// ═══════════════════════════════════════════════════════════════

void ShaderManager::createShader(void)
{
    std::string vsSrc  = loadFile("data/shaders/tess/displacement.vert");
    std::string tcsSrc = loadFile("data/shaders/tess/displacement.tesc");
    std::string teseSrc = loadFile("data/shaders/tess/displacement.tese");
    std::string gsSrc  = loadFile("data/shaders/tess/displacement.geom");
    std::string fsSrc  = loadFile("data/shaders/tess/displacement.frag");

    // 1. Compilar modo EQUAL (padrão do arquivo)
    this->ShaderProgramEqual = this->compileShader(vsSrc, tcsSrc, teseSrc, gsSrc, fsSrc);

    // 2. Compilar modo EVEN
    std::string teseSrcEven = teseSrc;
    size_t posEqual = teseSrcEven.find("equal_spacing");
    if (posEqual != std::string::npos) {
        teseSrcEven.replace(posEqual, std::string("equal_spacing").length(), "fractional_even_spacing");
    }
    this->ShaderProgramEven = this->compileShader(vsSrc, tcsSrc, teseSrcEven, gsSrc, fsSrc);

    // 3. Compilar modo ODD
    std::string teseSrcOdd = teseSrc;
    posEqual = teseSrcOdd.find("equal_spacing");
    if (posEqual != std::string::npos) {
        teseSrcOdd.replace(posEqual, std::string("equal_spacing").length(), "fractional_odd_spacing");
    }
    this->ShaderProgramOdd = this->compileShader(vsSrc, tcsSrc, teseSrcOdd, gsSrc, fsSrc);

    // Inicializar o active ShaderProgram com o esquema salvo
    this->setTessellationScheme(this->currentScheme);
}

GLuint ShaderManager::compileShader(const string& vs, const string& tcs, const string& tes, const string& gs, const string& fs)
{
    GLuint prog = glCreateProgram();
    if (!prog) { fprintf(stderr, "Erro ao criar programa de shader\n"); return 0; }

    addShader(prog, vs.c_str(),  GL_VERTEX_SHADER);
    addShader(prog, tcs.c_str(), GL_TESS_CONTROL_SHADER);
    addShader(prog, tes.c_str(), GL_TESS_EVALUATION_SHADER);
    addShader(prog, gs.c_str(),  GL_GEOMETRY_SHADER);
    addShader(prog, fs.c_str(),  GL_FRAGMENT_SHADER);

    glLinkProgram(prog);
    GLint ok; char log[1024];
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) { 
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro no link do shader: %s\n", log); 
        return 0; 
    }

    glValidateProgram(prog);
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &ok);
    if (!ok) { 
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        fprintf(stderr, "Shader inválido: %s\n", log); 
        return 0; 
    }

    return prog;
}

void ShaderManager::addShader(GLuint prog, const char* text, GLenum type)
{
    GLuint obj = glCreateShader(type);
    if (!obj) { fprintf(stderr, "Erro ao criar shader tipo %d\n", type); return; }

    const GLchar* p[1] = { text };
    GLint len[1]       = { (GLint)strlen(text) };
    glShaderSource(obj, 1, p, len);
    glCompileShader(obj);

    GLint ok;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(obj, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro compilando shader tipo %d:\n%s\n", type, log);
        glDeleteShader(obj);
        return;
    }
    glAttachShader(prog, obj);
    glDeleteShader(obj); // O programa já possui o shader anexado
}

// ═══════════════════════════════════════════════════════════════
// SHADER SIMPLES (quad 2D para mini-mapa)
// ═══════════════════════════════════════════════════════════════

void ShaderManager::createSimpleShader(void)
{
    std::string vsSrc = loadFile("data/shaders/simple/quad.vert");
    std::string fsSrc = loadFile("data/shaders/simple/quad.frag");

    this->compileSimpleShader(vsSrc, fsSrc);
}

void ShaderManager::compileSimpleShader(const string& vs, const string& fs)
{
    this->simpleShaderProgram = glCreateProgram();
    if (!simpleShaderProgram) { fprintf(stderr, "Erro ao criar programa simples\n"); return; }

    addSimpleShader(simpleShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
    addSimpleShader(simpleShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    glLinkProgram(simpleShaderProgram);
    GLint ok; char log[1024];
    glGetProgramiv(simpleShaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) { 
        glGetProgramInfoLog(simpleShaderProgram, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro no link do shader simples: %s\n", log); 
        return; 
    }

    simpleSamplerTexture = glGetUniformLocation(simpleShaderProgram, "samplerTex");
    simpleMvpLocation    = glGetUniformLocation(simpleShaderProgram, "MVP");
}

void ShaderManager::addSimpleShader(GLuint prog, const char* text, GLenum type)
{
    GLuint obj = glCreateShader(type);
    if (!obj) { fprintf(stderr, "Erro ao criar shader simples tipo %d\n", type); return; }

    const GLchar* p[1] = { text };
    GLint len[1]       = { (GLint)strlen(text) };
    glShaderSource(obj, 1, p, len);
    glCompileShader(obj);

    GLint ok;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(obj, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro compilando shader simples tipo %d:\n%s\n", type, log);
        glDeleteShader(obj);
        return;
    }
    glAttachShader(prog, obj);
    glDeleteShader(obj);
}

// ═══════════════════════════════════════════════════════════════
// SHADER DE COR PLANA
// ═══════════════════════════════════════════════════════════════

void ShaderManager::compileColorShader(void)
{
    std::string vsSrc = loadFile("data/shaders/color/flat.vert");
    std::string fsSrc = loadFile("data/shaders/color/flat.frag");

    this->colorShaderProgram = glCreateProgram();
    if (!this->colorShaderProgram) { fprintf(stderr, "Erro ao criar programa de cor plana\n"); return; }
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vsText = vsSrc.c_str();
    glShaderSource(vs, 1, &vsText, nullptr);
    glCompileShader(vs);
    GLint ok;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(vs, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro compilação Vertex Color Shader: %s\n", log);
    }
    glAttachShader(colorShaderProgram, vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fsText = fsSrc.c_str();
    glShaderSource(fs, 1, &fsText, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(fs, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro compilação Fragment Color Shader: %s\n", log);
    }
    glAttachShader(colorShaderProgram, fs);

    glLinkProgram(colorShaderProgram);
    glGetProgramiv(colorShaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(colorShaderProgram, sizeof(log), nullptr, log);
        fprintf(stderr, "Erro link Color Shader: %s\n", log);
    }
    
    colorMvpLocation = glGetUniformLocation(colorShaderProgram, "MVP");
    colorColorLocation = glGetUniformLocation(colorShaderProgram, "Color");

    glDeleteShader(vs);
    glDeleteShader(fs);
}

// ═══════════════════════════════════════════════════════════════
// API PÚBLICA
// ═══════════════════════════════════════════════════════════════

void ShaderManager::startShader(Shader_Type ID)
{
    switch (ID) {
        case SHADER_SIMPLE: glUseProgram(simpleShaderProgram); break;
        case SHADER_TESS:   glUseProgram(ShaderProgram);        break;
        case SHADER_COLOR:  glUseProgram(colorShaderProgram);   break;
    }
}

void ShaderManager::endShader(void) { glUseProgram(0); }

void ShaderManager::setMVP(const glm::mat4& mvp)
{
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    if (current == (GLint)ShaderProgram && mvpLocation >= 0)
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    else if (current == (GLint)simpleShaderProgram && simpleMvpLocation >= 0)
        glUniformMatrix4fv(simpleMvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    else if (current == (GLint)colorShaderProgram && colorMvpLocation >= 0)
        glUniformMatrix4fv(colorMvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
}

void ShaderManager::setModel(const glm::mat4& model)
{
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    if (current == (GLint)ShaderProgram)
    {
        GLint modelLoc = glGetUniformLocation(ShaderProgram, "Model");
        if (modelLoc >= 0)
        {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }
    }
}

void ShaderManager::setColor(const glm::vec4& color)
{
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    if (current == (GLint)colorShaderProgram && colorColorLocation >= 0)
        glUniform4fv(colorColorLocation, 1, glm::value_ptr(color));
}

void ShaderManager::setTessellationScheme(int scheme)
{
    this->currentScheme = scheme;
    if (scheme == 0)      this->ShaderProgram = this->ShaderProgramEqual;
    else if (scheme == 1) this->ShaderProgram = this->ShaderProgramEven;
    else                  this->ShaderProgram = this->ShaderProgramOdd;

    // Atualizar as localizações dos uniforms para o ShaderProgram ativo
    if (this->ShaderProgram > 0)
    {
        this->tessLevelInner        = glGetUniformLocation(this->ShaderProgram, "TessLevelInner");
        this->tessLevelOuter        = glGetUniformLocation(this->ShaderProgram, "TessLevelOuter");
        this->samplerDisplacementMap = glGetUniformLocation(this->ShaderProgram, "samplerDMap");
        this->samplerTexture        = glGetUniformLocation(this->ShaderProgram, "samplerTex");
        this->mvpLocation           = glGetUniformLocation(this->ShaderProgram, "MVP");
    }
}

int ShaderManager::getTessellationScheme(void)
{
    return this->currentScheme;
}

GLuint ShaderManager::getTessLevelInner(void)          { return tessLevelInner; }
GLuint ShaderManager::getTessLevelOuter(void)          { return tessLevelOuter; }
GLuint ShaderManager::getSamplerDisplacementMap(void)  { return samplerDisplacementMap; }
GLuint ShaderManager::getSamplerTexture(void)          { return samplerTexture; }
GLuint ShaderManager::getSimpleSamplerTexture(void)    { return simpleSamplerTexture; }
