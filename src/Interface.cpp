#include "Interface.h"
#include <cstring>

Interface::Interface(void)
    : radius(5)
    , color(BLUE)
    , button(1)
    , bUpdate(false)
{
    mouse[0]  = mouse[1]  = 0;
    normal[0] = normal[1] = normal[2] = 0.f;
    initQuad();
}

Interface::~Interface(void)
{
    if (quadVAO) { glDeleteVertexArrays(1, &quadVAO); quadVAO = 0; }
    if (quadVBO) { glDeleteBuffers(1, &quadVBO);      quadVBO = 0; }
}

// Cria um quad 2D (320x320 px) para exibir o mini-mapa de deslocamento
// Posicionado no canto superior esquerdo em coordenadas NDC [-1,1]
void Interface::initQuad(void)
{
    // Coordenadas em NDC para um quad no canto superior esquerdo (320x320 de 1024x768)
    // x: -1 até -1 + 2*(320/1024) = -1 + 0.625 = -0.375
    // y:  1 até  1 - 2*(320/768)  =  1 - 0.833 =  0.167
    const float x0 = -1.f,               y0 =  1.f;
    const float x1 = -1.f + 2.f*320/1024.f, y1 = 1.f - 2.f*320/768.f;

    // pos(xy) + texcoord(uv)
    float verts[] = {
        x0, y0,   0.f, 1.f,
        x1, y0,   1.f, 1.f,
        x0, y1,   0.f, 0.f,
        x1, y1,   1.f, 0.f,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    // layout(location=0) = position xy
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    // layout(location=1) = texcoord uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindVertexArray(0);
}

void Interface::update(void)
{
    bUpdate = false;
    if (button != 1) {
        paint(mouse[0], mouse[1], button);
        bUpdate = true;
    }
}

void Interface::draw(ShaderManager* shaderManager, DisplacementMap* displacementMap, bool drawCircle)
{
    // Salvar estado de depth test (vamos desligar para 2D)
    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    // Usar shader simples para desenhar o mini-mapa
    shaderManager->startShader(ShaderManager::SHADER_SIMPLE);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shaderManager->getSimpleSamplerTexture(), 0);
    glBindTexture(GL_TEXTURE_2D, displacementMap->getID());

    // Enviar matriz identidade como MVP (já em NDC)
    shaderManager->setMVP(glm::mat4(1.f));

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    shaderManager->endShader();

    // desenhar círculo ao redor do mouse se solicitado
    if (drawCircle)
    {
        float cx = mouse[0] * 5.0f;
        float cy = mouse[1] * 5.0f;
        float r  = radius * 5.0f;

        const int numSegments = 32;
        float circleVerts[numSegments * 2];
        for (int i = 0; i < numSegments; ++i)
        {
            float theta = 2.0f * 3.14159265f * float(i) / float(numSegments);
            float px = cx + cosf(theta) * r;
            float py = cy + sinf(theta) * r;

            circleVerts[i * 2]     = (px / 1024.0f) * 2.0f - 1.0f;
            circleVerts[i * 2 + 1] = 1.0f - (py / 768.0f) * 2.0f;
        }

        GLuint circleVAO, circleVBO;
        glGenVertexArrays(1, &circleVAO);
        glGenBuffers(1, &circleVBO);

        glBindVertexArray(circleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleVerts), circleVerts, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        shaderManager->startShader(ShaderManager::SHADER_COLOR);
        shaderManager->setMVP(glm::mat4(1.f));
        shaderManager->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glDrawArrays(GL_LINE_LOOP, 0, numSegments);

        glBindVertexArray(0);
        shaderManager->endShader();

        glDeleteVertexArrays(1, &circleVAO);
        glDeleteBuffers(1, &circleVBO);
    }

    if (depthWasEnabled) glEnable(GL_DEPTH_TEST);
}

int*   Interface::getMouse(void)  { return mouse; }
int    Interface::getRadius(void) { return radius; }
float* Interface::getNormal(void) { return normal; }
bool   Interface::getUpdate(void) { return bUpdate; }

void Interface::keyPressed(int key)
{
    if (key == '-' && radius > 2) radius--;
    if (key == '=')               radius++;
}

void Interface::keyReleased(int key)
{
    if (key == '8') color = RED;
    if (key == '9') color = GREEN;
    if (key == '0') color = BLUE;
}

void Interface::mouseMoved   (int x, int y)            { mouse[0]=x/5; mouse[1]=y/5; }
void Interface::mouseDragged (int x, int y, int btn)   { mouse[0]=x/5; mouse[1]=y/5; button=btn; }
void Interface::mousePressed (int x, int y, int btn)   { mouse[0]=x/5; mouse[1]=y/5; button=btn; }
void Interface::mouseReleased(int x, int y, int /*btn*/){ mouse[0]=x/5; mouse[1]=y/5; button=1; }

void Interface::mouseScrolled(double /*xoffset*/, double yoffset)
{
    if (yoffset > 0.0) {
        radius++;
    } else if (yoffset < 0.0 && radius > 2) {
        radius--;
    }
}

void Interface::paint(int x, int y, int btn)
{
    normal[0] = normal[1] = normal[2] = 0.f;
    float delta = (btn == 0) ? 1/255.f : -1/255.f;
    if (btn == 0 || btn == 2) {
        if      (color == RED)   normal[0] = delta;
        else if (color == GREEN) normal[1] = delta;
        else if (color == BLUE)  normal[2] = delta;
    }
}