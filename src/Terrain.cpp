#include "Terrain.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Terrain::Terrain(void)
    : tessLevelInner(64)
    , tessLevelOuter(64)
    , x(0.f), y(40.f), z(60.f)
    , tx(0.f), ty(0.f), tz(0.f)
    , bWireframe(false)
    , bRotate(false)
    , angle(0.f)
{
    initVAO();
}

Terrain::~Terrain(void)
{
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo);      vbo = 0; }
}

// Cria o VAO/VBO com o plano base: 2 triângulos (6 vértices)
// Cada vértice: pos(xyz) + texcoord(uv)
void Terrain::initVAO(void)
{
    // Plano de 64x64 unidades centrado na origem, no plano Y=0
    // layout: position(3f) + texcoord(2f)
    float verts[] = {
        // triângulo 1
        -32.f, 0.f, -32.f,   0.01f, 0.99f, // topo-esquerda (V=0.99)
         32.f, 0.f, -32.f,   0.99f, 0.99f, // topo-direita (V=0.99)
        -32.f, 0.f,  32.f,   0.01f, 0.01f, // base-esquerda (V=0.01)
        // triângulo 2
        -32.f, 0.f,  32.f,   0.01f, 0.01f, // base-esquerda (V=0.01)
         32.f, 0.f, -32.f,   0.99f, 0.99f, // topo-direita (V=0.99)
         32.f, 0.f,  32.f,   0.99f, 0.01f, // base-direita (V=0.01)
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // layout(location = 0) in vec4 Position  (usamos xyz, w=1 implícito no GS)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // layout(location = 1) in vec2 TexCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void Terrain::draw(ShaderManager* shaderManager, Texture* texture, DisplacementMap* displacementMap)
{
    glEnable(GL_DEPTH_TEST);

    // ── Calcular MVP com GLM ──────────────────────────────────
    glm::mat4 proj  = glm::perspective(glm::radians(60.f), 1024.f / 768.f, 0.001f, 5000.f);
    glm::mat4 view  = glm::lookAt(
        glm::vec3(x,  y,  z),   // posição câmera
        glm::vec3(tx, ty, tz),  // alvo
        glm::vec3(0.f, 1.f, 0.f)
    );
    glm::mat4 model = glm::rotate(glm::mat4(1.f),
                                   glm::radians(angle),
                                   glm::vec3(0.f, 1.f, 0.f));
    if (bRotate) angle += 0.1f;

    glm::mat4 mvp = proj * view * model;

    // ── Configurar shader ─────────────────────────────────────
    shaderManager->startShader(ShaderManager::SHADER_TESS);
    shaderManager->setMVP(mvp);

    // ── Modo wireframe ────────────────────────────────────────
    if (bWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // ── Tessellation ──────────────────────────────────────────
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glUniform1f(shaderManager->getTessLevelInner(), (float)tessLevelInner);
    glUniform1f(shaderManager->getTessLevelOuter(), (float)tessLevelOuter);

    // ── Texturas ──────────────────────────────────────────────
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shaderManager->getSamplerTexture(), 0);
    glBindTexture(GL_TEXTURE_2D, texture->getID());

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(shaderManager->getSamplerDisplacementMap(), 1);
    glBindTexture(GL_TEXTURE_2D, displacementMap->getID());

    // ── Draw ──────────────────────────────────────────────────
    glBindVertexArray(vao);
    glDrawArrays(GL_PATCHES, 0, 6);
    glBindVertexArray(0);

    // ── Restaurar estado ──────────────────────────────────────
    if (bWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shaderManager->endShader();
    glDisable(GL_DEPTH_TEST);
}

// ── Setters / Getters ──────────────────────────────────────────

void  Terrain::setRotate(bool r)           { bRotate = r; }
bool  Terrain::getRotate(void)             { return bRotate; }
void  Terrain::setWireframe(bool w)        { bWireframe = w; }
bool  Terrain::getWireframe(void)          { return bWireframe; }
void  Terrain::setAngle(float a)           { angle = a; }
float Terrain::getAngle(void)              { return angle; }

void Terrain::setCameraPosition(float cx, float cy, float cz) { x=cx; y=cy; z=cz; }
void Terrain::setCameraTarget  (float cx, float cy, float cz) { tx=cx; ty=cy; tz=cz; }

void Terrain::setTessellationFactor(int inner, int outer)
{
    tessLevelInner = inner;
    tessLevelOuter = outer;
}

void Terrain::increaseTessellationFactor(int di, int dout)
{
    tessLevelInner = std::min(tessLevelInner + di,   64);
    tessLevelOuter = std::min(tessLevelOuter + dout, 64);
}

void Terrain::decreaseTessellationFactor(int di, int dout)
{
    tessLevelInner = std::max(tessLevelInner - di,   1);
    tessLevelOuter = std::max(tessLevelOuter - dout, 1);
}

void Terrain::increaseCameraPosition(float dx, float dy, float dz) { x+=dx; y+=dy; z+=dz; }
void Terrain::decreaseCameraPosition(float dx, float dy, float dz) { x-=dx; y-=dy; z-=dz; }
