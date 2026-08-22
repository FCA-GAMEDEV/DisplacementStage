#include "ShaderManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <cstdio>

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager::ShaderManager(void)
{
    this->createShader();
    this->createSimpleShader();
    this->compileColorShader();
}

ShaderManager::~ShaderManager(void)
{
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

// ═══════════════════════════════════════════════════════════════
// SHADER PRINCIPAL (Tessellation + Displacement)
// ═══════════════════════════════════════════════════════════════

void ShaderManager::createShader(void)
{
    this->createVertexShader();
    this->createTessControlShader();
    this->createTessEvaluationShader();
    this->createGeometryShader();
    this->createFragmentShader();
    this->compileShader();
}

// ── Vertex Shader ──────────────────────────────────────────────
// Recebe posição e texcoord via VAO. Passa adiante para o TCS.
void ShaderManager::createVertexShader(void)
{
    this->pVertexShader =
        "#version 420 core\n"
        "\n"
        "layout(location = 0) in vec4 Position;\n"
        "layout(location = 1) in vec2 TexCoord;\n"
        "\n"
        "out vec3 vPosition;\n"
        "out vec2 vTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vTexCoord = TexCoord;\n"
        "    vPosition = Position.xyz;\n"
        "}\n";
}

// ── Tessellation Control Shader ────────────────────────────────
void ShaderManager::createTessControlShader(void)
{
    this->pTessControlShader =
        "#version 420 core\n"
        "\n"
        "layout(vertices = 3) out;\n"
        "\n"
        "in  vec3 vPosition[];\n"
        "out vec3 tcPosition[];\n"
        "in  vec2 vTexCoord[];\n"
        "out vec2 tcTexCoord[];\n"
        "\n"
        "uniform float TessLevelInner;\n"
        "uniform float TessLevelOuter;\n"
        "\n"
        "#define ID gl_InvocationID\n"
        "\n"
        "void main()\n"
        "{\n"
        "    tcPosition[ID] = vPosition[ID];\n"
        "    tcTexCoord[ID] = vTexCoord[ID];\n"
        "    if (ID == 0) {\n"
        "        gl_TessLevelInner[0] = TessLevelInner;\n"
        "        gl_TessLevelOuter[0] = TessLevelOuter;\n"
        "        gl_TessLevelOuter[1] = TessLevelOuter;\n"
        "        gl_TessLevelOuter[2] = TessLevelOuter;\n"
        "    }\n"
        "}\n";
}

// ── Tessellation Evaluation Shader ─────────────────────────────
void ShaderManager::createTessEvaluationShader(void)
{
    this->pTessEvaluationShader =
        "#version 420 core\n"
        "\n"
        "layout(triangles, equal_spacing, cw) in;\n"
        "\n"
        "in  vec3 tcPosition[];\n"
        "out vec3 tePosition;\n"
        "out vec3 tePatchDistance;\n"
        "in  vec2 tcTexCoord[];\n"
        "out vec2 teTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 p0 = gl_TessCoord.x * tcPosition[0];\n"
        "    vec3 p1 = gl_TessCoord.y * tcPosition[1];\n"
        "    vec3 p2 = gl_TessCoord.z * tcPosition[2];\n"
        "    vec2 t0 = gl_TessCoord.x * tcTexCoord[0];\n"
        "    vec2 t1 = gl_TessCoord.y * tcTexCoord[1];\n"
        "    vec2 t2 = gl_TessCoord.z * tcTexCoord[2];\n"
        "    tePatchDistance = gl_TessCoord;\n"
        "    tePosition      = p0 + p1 + p2;\n"
        "    teTexCoord      = t0 + t1 + t2;\n"
        "    gl_Position     = vec4(tePosition, 1.0);\n"
        "}\n";
}

// ── Geometry Shader ────────────────────────────────────────────
// Aplica o displacement: lê o mapa de deslocamento e desloca cada
// vértice ao longo dos eixos. Usa uniform MVP em vez de gl_ProjectionMatrix.
void ShaderManager::createGeometryShader(void)
{
    this->pGeometryShader =
        "#version 420 core\n"
        "\n"
        "layout(triangles) in;\n"
        "layout(triangle_strip, max_vertices = 3) out;\n"
        "\n"
        "uniform sampler2D samplerDMap;\n"
        "uniform mat4 MVP;\n"
        "\n"
        "in  vec3 tePosition[3];\n"
        "in  vec2 teTexCoord[3];\n"
        "in  vec3 tePatchDistance[3];\n"
        "out vec2 gTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    float scale = 10.0;\n"
        "\n"
        "    gTexCoord   = teTexCoord[0];\n"
        "    vec4 h0     = texture(samplerDMap, gTexCoord);\n"
        "    gl_Position = MVP * vec4(\n"
        "        gl_in[0].gl_Position.x + h0.r * scale,\n"
        "        gl_in[0].gl_Position.y + h0.b * scale,\n"
        "        gl_in[0].gl_Position.z + h0.g * scale,\n"
        "        gl_in[0].gl_Position.w);\n"
        "    EmitVertex();\n"
        "\n"
        "    gTexCoord   = teTexCoord[1];\n"
        "    vec4 h1     = texture(samplerDMap, gTexCoord);\n"
        "    gl_Position = MVP * vec4(\n"
        "        gl_in[1].gl_Position.x + h1.r * scale,\n"
        "        gl_in[1].gl_Position.y + h1.b * scale,\n"
        "        gl_in[1].gl_Position.z + h1.g * scale,\n"
        "        gl_in[1].gl_Position.w);\n"
        "    EmitVertex();\n"
        "\n"
        "    gTexCoord   = teTexCoord[2];\n"
        "    vec4 h2     = texture(samplerDMap, gTexCoord);\n"
        "    gl_Position = MVP * vec4(\n"
        "        gl_in[2].gl_Position.x + h2.r * scale,\n"
        "        gl_in[2].gl_Position.y + h2.b * scale,\n"
        "        gl_in[2].gl_Position.z + h2.g * scale,\n"
        "        gl_in[2].gl_Position.w);\n"
        "    EmitVertex();\n"
        "\n"
        "    EndPrimitive();\n"
        "}\n";
}

// ── Fragment Shader ────────────────────────────────────────────
void ShaderManager::createFragmentShader(void)
{
    this->pFragmentShader =
        "#version 420 core\n"
        "\n"
        "in  vec2 gTexCoord;\n"
        "out vec4 FragColor;\n"
        "\n"
        "uniform sampler2D samplerTex;\n"
        "uniform sampler2D samplerDMap;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(samplerTex, gTexCoord);\n"
        "}\n";
}

// ═══════════════════════════════════════════════════════════════
// SHADER SIMPLES (quad 2D para mini-mapa)
// ═══════════════════════════════════════════════════════════════

void ShaderManager::createSimpleShader(void)
{
    this->createSimpleVertexShader();
    this->createSimpleFragmentShader();
    this->compileSimpleShader();
}

void ShaderManager::createSimpleVertexShader(void)
{
    this->pSimpleVertexShader =
        "#version 420 core\n"
        "\n"
        "layout(location = 0) in vec2 Position;\n"
        "layout(location = 1) in vec2 TexCoord;\n"
        "\n"
        "out vec2 TexCoord0;\n"
        "\n"
        "uniform mat4 MVP;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = MVP * vec4(Position, 0.0, 1.0);\n"
        "    TexCoord0   = TexCoord;\n"
        "}\n";
}

void ShaderManager::createSimpleFragmentShader(void)
{
    this->pSimpleFragmentShader =
        "#version 420 core\n"
        "\n"
        "in  vec2 TexCoord0;\n"
        "out vec4 FragColor;\n"
        "\n"
        "uniform sampler2D samplerTex;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(samplerTex, TexCoord0);\n"
        "}\n";
}

// ═══════════════════════════════════════════════════════════════
// COMPILAÇÃO
// ═══════════════════════════════════════════════════════════════

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
        return;
    }
    glAttachShader(prog, obj);
    this->ShaderObject = obj; // guarda referência (compatibilidade)
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
        return;
    }
    glAttachShader(prog, obj);
    this->simpleShaderObject = obj;
}

void ShaderManager::compileShader(void)
{
    this->ShaderProgram = glCreateProgram();
    if (!this->ShaderProgram) { fprintf(stderr, "Erro ao criar programa de shader\n"); return; }

    addShader(ShaderProgram, pVertexShader,          GL_VERTEX_SHADER);
    addShader(ShaderProgram, pTessControlShader,     GL_TESS_CONTROL_SHADER);
    addShader(ShaderProgram, pTessEvaluationShader,  GL_TESS_EVALUATION_SHADER);
    addShader(ShaderProgram, pGeometryShader,        GL_GEOMETRY_SHADER);
    addShader(ShaderProgram, pFragmentShader,        GL_FRAGMENT_SHADER);

    glLinkProgram(ShaderProgram);
    GLint ok; char log[1024];
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) { glGetProgramInfoLog(ShaderProgram, sizeof(log), nullptr, log);
               fprintf(stderr, "Erro no link do shader: %s\n", log); return; }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &ok);
    if (!ok) { glGetProgramInfoLog(ShaderProgram, sizeof(log), nullptr, log);
               fprintf(stderr, "Shader inválido: %s\n", log); return; }

    tessLevelInner        = glGetUniformLocation(ShaderProgram, "TessLevelInner");
    tessLevelOuter        = glGetUniformLocation(ShaderProgram, "TessLevelOuter");
    samplerDisplacemetMap = glGetUniformLocation(ShaderProgram, "samplerDMap");
    samplerTexture        = glGetUniformLocation(ShaderProgram, "samplerTex");
    mvpLocation           = glGetUniformLocation(ShaderProgram, "MVP");
}

void ShaderManager::compileSimpleShader(void)
{
    this->simnpleShaderProgram = glCreateProgram();
    if (!simnpleShaderProgram) { fprintf(stderr, "Erro ao criar programa simples\n"); return; }

    addSimpleShader(simnpleShaderProgram, pSimpleVertexShader,   GL_VERTEX_SHADER);
    addSimpleShader(simnpleShaderProgram, pSimpleFragmentShader, GL_FRAGMENT_SHADER);

    glLinkProgram(simnpleShaderProgram);
    GLint ok; char log[1024];
    glGetProgramiv(simnpleShaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) { glGetProgramInfoLog(simnpleShaderProgram, sizeof(log), nullptr, log);
               fprintf(stderr, "Erro no link do shader simples: %s\n", log); return; }

    simpleSamplerTexture = glGetUniformLocation(simnpleShaderProgram, "samplerTex");
    simpleMvpLocation    = glGetUniformLocation(simnpleShaderProgram, "MVP");
}

void ShaderManager::compileColorShader(void)
{
    const char* colorVS = 
        "#version 420 core\n"
        "layout(location = 0) in vec2 Position;\n"
        "uniform mat4 MVP;\n"
        "void main() {\n"
        "    gl_Position = MVP * vec4(Position, 0.0, 1.0);\n"
        "}\n";

    const char* colorFS = 
        "#version 420 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 Color;\n"
        "void main() {\n"
        "    FragColor = Color;\n"
        "}\n";

    this->colorShaderProgram = glCreateProgram();
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &colorVS, nullptr);
    glCompileShader(vs);
    glAttachShader(colorShaderProgram, vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &colorFS, nullptr);
    glCompileShader(fs);
    glAttachShader(colorShaderProgram, fs);

    glLinkProgram(colorShaderProgram);
    
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
        case SHADER_SIMPLE: glUseProgram(simnpleShaderProgram); break;
        case SHADER_TESS:   glUseProgram(ShaderProgram);        break;
        case SHADER_COLOR:  glUseProgram(colorShaderProgram);   break;
    }
}

void ShaderManager::endShader(void) { glUseProgram(0); }

void ShaderManager::setMVP(const glm::mat4& mvp)
{
    // Identifica qual programa está ativo e envia para o uniform correto
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    if (current == (GLint)ShaderProgram && mvpLocation >= 0)
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    else if (current == (GLint)simnpleShaderProgram && simpleMvpLocation >= 0)
        glUniformMatrix4fv(simpleMvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    else if (current == (GLint)colorShaderProgram && colorMvpLocation >= 0)
        glUniformMatrix4fv(colorMvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
}

void ShaderManager::setColor(const glm::vec4& color)
{
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    if (current == (GLint)colorShaderProgram && colorColorLocation >= 0)
        glUniform4fv(colorColorLocation, 1, glm::value_ptr(color));
}

GLuint ShaderManager::getTessLevelInner(void)          { return tessLevelInner; }
GLuint ShaderManager::getTessLevelOuter(void)          { return tessLevelOuter; }
GLuint ShaderManager::getSamplerDisplacementMap(void)  { return samplerDisplacemetMap; }
GLuint ShaderManager::getSamplerTexture(void)          { return samplerTexture; }
GLuint ShaderManager::getSimpleSamplerTexture(void)    { return simpleSamplerTexture; }
