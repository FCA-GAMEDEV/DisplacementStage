#include "Interface.h"
#include <cstring>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

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
    if (circleVAO) { glDeleteVertexArrays(1, &circleVAO); circleVAO = 0; }
    if (circleVBO) { glDeleteBuffers(1, &circleVBO);      circleVBO = 0; }
    if (rectVAO) { glDeleteVertexArrays(1, &rectVAO); rectVAO = 0; }
    if (rectVBO) { glDeleteBuffers(1, &rectVBO);      rectVBO = 0; }
}

void Interface::initQuad(void)
{
    const float x0 = -1.f,                  y0 =  1.f;
    const float x1 = -1.f + 2.f*320/1024.f, y1 = 1.f - 2.f*320/768.f;

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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindVertexArray(0);

    // Inicializar VAO/VBO do Círculo Unitário (34 vértices)
    float circleVerts[34 * 2];
    circleVerts[0] = 0.0f;
    circleVerts[1] = 0.0f;
    for (int i = 1; i <= 32; ++i)
    {
        float theta = 2.0f * 3.14159265f * float(i - 1) / 32.0f;
        circleVerts[i * 2]     = cosf(theta);
        circleVerts[i * 2 + 1] = sinf(theta);
    }
    circleVerts[33 * 2]     = 1.0f;
    circleVerts[33 * 2 + 1] = 0.0f;

    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVerts), circleVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Inicializar VAO/VBO do Retângulo Unitário
    float rectVerts[] = {
        0.f, 0.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
    };
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVerts), rectVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
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

void Interface::drawRect(ShaderManager* shaderManager, float x, float y, float w, float h, const glm::vec4& color)
{
    shaderManager->startShader(ShaderManager::SHADER_COLOR);
    
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(x, y, 0.f));
    model = glm::scale(model, glm::vec3(w, h, 1.f));
    shaderManager->setMVP(model);
    shaderManager->setColor(color);
    
    glBindVertexArray(rectVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    
    shaderManager->endShader();
}

void Interface::draw(ShaderManager* shaderManager, DisplacementMap* displacementMap, bool drawCircle, bool isWireframe, bool isRotate, bool isCullFace, int tessScheme, bool drawMinimap)
{
    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    // 1. Cor da borda com base no canal RGB selecionado
    glm::vec4 channelColor(0.f);
    if (color == RED)        channelColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (color == GREEN) channelColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    else                     channelColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    // Coordenadas em NDC do mini-mapa
    const float x0 = -1.f;
    const float y0 =  1.f;
    const float x1 = -1.f + 2.f*320/1024.f; // -0.375
    const float y1 = 1.f - 2.f*320/768.f;  // 0.166667

    float outline_x = 1.f * (2.f / 1024.f);
    float outline_y = 1.f * (2.f / 768.f);

    // 2. Desenhar mini-mapa e bordas se drawMinimap for true
    if (drawMinimap)
    {
        // Desenhar borda do mini-mapa se estiver na cena de pintura (drawCircle = true)
        if (drawCircle)
        {
            float bw_x = 4.f * (2.f / 1024.f);
            float bw_y = 4.f * (2.f / 768.f);
            drawRect(shaderManager, x0, y0 - bw_y, x1 - x0, bw_y, channelColor); // Top
            drawRect(shaderManager, x0, y1, x1 - x0, bw_y, channelColor);       // Bottom
            drawRect(shaderManager, x0, y1, bw_x, y0 - y1, channelColor);       // Left
            drawRect(shaderManager, x1 - bw_x, y1, bw_x, y0 - y1, channelColor); // Right
        }

        // Usar shader simples para desenhar o mini-mapa
        shaderManager->startShader(ShaderManager::SHADER_SIMPLE);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(shaderManager->getSimpleSamplerTexture(), 0);
        glBindTexture(GL_TEXTURE_2D, displacementMap->getID());
        shaderManager->setMVP(glm::mat4(1.f));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
        shaderManager->endShader();
    }

    // 3. Desenhar barra de raio apenas na cena de pintura (drawCircle = true e drawMinimap = true)
    if (drawCircle && drawMinimap)
    {
        // Posição: 1px de distância do mini-mapa (que termina em 320px). Starts at y = 321px, height = 8px.
        // Comprimento da barra: exatamente 320px (o mesmo do mini-mapa), iniciando na borda esquerda (x = 0 px).
        float bar_x0 = -1.f;
        float bar_y0 = 1.f - 329.f * (2.f / 768.f);
        float bar_w = 320.f * (2.f / 1024.f);
        float bar_h = 8.f * (2.f / 768.f);

        // Contorno da barra (cinza claro) - sem overflow nas laterais (x) para casar certinho
        drawRect(shaderManager, bar_x0, bar_y0 - outline_y, bar_w, bar_h + 2.f * outline_y, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        // Fundo da barra (cinza escuro)
        drawRect(shaderManager, bar_x0, bar_y0, bar_w, bar_h, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        // Preenchimento da barra baseado no raio (clamp entre 2 e 60)
        float t = (float)(radius - 2) / (60.f - 2.f);
        if (t < 0.f) t = 0.f;
        if (t > 1.f) t = 1.f;
        drawRect(shaderManager, bar_x0, bar_y0, bar_w * t, bar_h, channelColor);
    }

    // 4. Desenhar os checkboxes (Sempre desenhados em todas as cenas!)
    // Checkbox 1: WIREFRAME (y: [349, 363] px, x: [20, 34] px)
    {
        float box_x = 20.f * (2.f / 1024.f) - 1.f;
        float box_y = 1.f - 363.f * (2.f / 768.f);
        float box_w = 14.f * (2.f / 1024.f);
        float box_h = 14.f * (2.f / 768.f);

        // Desenhar contorno da caixa (cinza claro)
        drawRect(shaderManager, box_x, box_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Bottom
        drawRect(shaderManager, box_x, box_y + box_h - outline_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Top
        drawRect(shaderManager, box_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Left
        drawRect(shaderManager, box_x + box_w - outline_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Right

        // Se o wireframe estiver marcado, desenhar preenchimento interno (laranja)
        if (isWireframe)
        {
            float inner_x = box_x + 3.f * outline_x;
            float inner_y = box_y + 3.f * outline_y;
            float inner_w = box_w - 6.f * outline_x;
            float inner_h = box_h - 6.f * outline_y;
            drawRect(shaderManager, inner_x, inner_y, inner_w, inner_h, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        }

        // Desenhar texto "WIREFRAME" ao lado
        float text_x = 44.f * (2.f / 1024.f) - 1.f;
        float text_y = 1.f - 361.f * (2.f / 768.f);
        float char_w = 8.f * (2.f / 1024.f);
        float char_h = 12.f * (2.f / 768.f);
        float spacing = 11.f * (2.f / 1024.f);
        drawStrokeText(shaderManager, "WIREFRAME", text_x, text_y, char_w, char_h, spacing, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    // Checkbox 2: ROTATION (10px abaixo de Wireframe, y: [373, 387] px, x: [20, 34] px)
    {
        float box_x = 20.f * (2.f / 1024.f) - 1.f;
        float box_y = 1.f - 387.f * (2.f / 768.f);
        float box_w = 14.f * (2.f / 1024.f);
        float box_h = 14.f * (2.f / 768.f);

        // Desenhar contorno da caixa (cinza claro)
        drawRect(shaderManager, box_x, box_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Bottom
        drawRect(shaderManager, box_x, box_y + box_h - outline_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Top
        drawRect(shaderManager, box_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Left
        drawRect(shaderManager, box_x + box_w - outline_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Right

        // Se rotation estiver marcado, desenhar preenchimento interno (verde claro)
        if (isRotate)
        {
            float inner_x = box_x + 3.f * outline_x;
            float inner_y = box_y + 3.f * outline_y;
            float inner_w = box_w - 6.f * outline_x;
            float inner_h = box_h - 6.f * outline_y;
            drawRect(shaderManager, inner_x, inner_y, inner_w, inner_h, glm::vec4(0.0f, 0.8f, 0.1f, 1.0f));
        }

        // Desenhar texto "ROTATION" ao lado
        float text_x = 44.f * (2.f / 1024.f) - 1.f;
        float text_y = 1.f - 385.f * (2.f / 768.f);
        float char_w = 8.f * (2.f / 1024.f);
        float char_h = 12.f * (2.f / 768.f);
        float spacing = 11.f * (2.f / 1024.f);
        drawStrokeText(shaderManager, "ROTATION", text_x, text_y, char_w, char_h, spacing, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    // Checkbox 3: CULL FACE (10px abaixo de Rotation, y: [397, 411] px, x: [20, 34] px)
    {
        float box_x = 20.f * (2.f / 1024.f) - 1.f;
        float box_y = 1.f - 411.f * (2.f / 768.f);
        float box_w = 14.f * (2.f / 1024.f);
        float box_h = 14.f * (2.f / 768.f);

        // Desenhar contorno da caixa (cinza claro)
        drawRect(shaderManager, box_x, box_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Bottom
        drawRect(shaderManager, box_x, box_y + box_h - outline_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Top
        drawRect(shaderManager, box_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Left
        drawRect(shaderManager, box_x + box_w - outline_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Right

        // Se cull face estiver marcado, desenhar preenchimento interno (azul claro/cyan)
        if (isCullFace)
        {
            float inner_x = box_x + 3.f * outline_x;
            float inner_y = box_y + 3.f * outline_y;
            float inner_w = box_w - 6.f * outline_x;
            float inner_h = box_h - 6.f * outline_y;
            drawRect(shaderManager, inner_x, inner_y, inner_w, inner_h, glm::vec4(0.0f, 0.7f, 0.9f, 1.0f));
        }

        // Desenhar texto "CULL FACE" ao lado
        float text_x = 44.f * (2.f / 1024.f) - 1.f;
        float text_y = 1.f - 409.f * (2.f / 768.f);
        float char_w = 8.f * (2.f / 1024.f);
        float char_h = 12.f * (2.f / 768.f);
        float spacing = 11.f * (2.f / 1024.f);
        drawStrokeText(shaderManager, "CULL FACE", text_x, text_y, char_w, char_h, spacing, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    // Checkbox 4: EVEN SPACING (10px abaixo de Cull Face, y: [421, 435] px, x: [20, 34] px)
    {
        float box_x = 20.f * (2.f / 1024.f) - 1.f;
        float box_y = 1.f - 435.f * (2.f / 768.f);
        float box_w = 14.f * (2.f / 1024.f);
        float box_h = 14.f * (2.f / 768.f);

        // Desenhar contorno da caixa (cinza claro)
        drawRect(shaderManager, box_x, box_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Bottom
        drawRect(shaderManager, box_x, box_y + box_h - outline_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Top
        drawRect(shaderManager, box_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Left
        drawRect(shaderManager, box_x + box_w - outline_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Right

        // Se o esquema ativado for EVEN (1), desenhar preenchimento interno (amarelo/gold)
        if (tessScheme == 1)
        {
            float inner_x = box_x + 3.f * outline_x;
            float inner_y = box_y + 3.f * outline_y;
            float inner_w = box_w - 6.f * outline_x;
            float inner_h = box_h - 6.f * outline_y;
            drawRect(shaderManager, inner_x, inner_y, inner_w, inner_h, glm::vec4(1.0f, 0.9f, 0.0f, 1.0f));
        }

        // Desenhar texto "EVEN SPACING" ao lado
        float text_x = 44.f * (2.f / 1024.f) - 1.f;
        float text_y = 1.f - 433.f * (2.f / 768.f);
        float char_w = 8.f * (2.f / 1024.f);
        float char_h = 12.f * (2.f / 768.f);
        float spacing = 11.f * (2.f / 1024.f);
        drawStrokeText(shaderManager, "EVEN SPACING", text_x, text_y, char_w, char_h, spacing, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    // Checkbox 5: ODD SPACING (10px abaixo de Even Spacing, y: [445, 459] px, x: [20, 34] px)
    {
        float box_x = 20.f * (2.f / 1024.f) - 1.f;
        float box_y = 1.f - 459.f * (2.f / 768.f);
        float box_w = 14.f * (2.f / 1024.f);
        float box_h = 14.f * (2.f / 768.f);

        // Desenhar contorno da caixa (cinza claro)
        drawRect(shaderManager, box_x, box_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Bottom
        drawRect(shaderManager, box_x, box_y + box_h - outline_y, box_w, outline_y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Top
        drawRect(shaderManager, box_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Left
        drawRect(shaderManager, box_x + box_w - outline_x, box_y, outline_x, box_h, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Right

        // Se o esquema ativado for ODD (2), desenhar preenchimento interno (roxo/magenta)
        if (tessScheme == 2)
        {
            float inner_x = box_x + 3.f * outline_x;
            float inner_y = box_y + 3.f * outline_y;
            float inner_w = box_w - 6.f * outline_x;
            float inner_h = box_h - 6.f * outline_y;
            drawRect(shaderManager, inner_x, inner_y, inner_w, inner_h, glm::vec4(0.8f, 0.0f, 0.8f, 1.0f));
        }

        // Desenhar texto "ODD SPACING" ao lado
        float text_x = 44.f * (2.f / 1024.f) - 1.f;
        float text_y = 1.f - 457.f * (2.f / 768.f);
        float char_w = 8.f * (2.f / 1024.f);
        float char_h = 12.f * (2.f / 768.f);
        float spacing = 11.f * (2.f / 1024.f);
        drawStrokeText(shaderManager, "ODD SPACING", text_x, text_y, char_w, char_h, spacing, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    // 5. Desenhar círculo ao redor do mouse se solicitado
    if (drawCircle)
    {
        float cx = mouse[0] * 5.0f;
        float cy = mouse[1] * 5.0f;
        float r  = radius * 5.0f;

        float translate_x = (cx / 1024.0f) * 2.0f - 1.0f;
        float translate_y = 1.0f - (cy / 768.0f) * 2.0f;
        float scale_x = (r / 1024.0f) * 2.0f;
        float scale_y = (r / 768.0f) * 2.0f;

        // Ativar blending para transparência no preenchimento
        GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shaderManager->startShader(ShaderManager::SHADER_COLOR);

        // Desenhar preenchimento semi-transparente do círculo
        glm::vec4 fillCol = glm::vec4(channelColor.r, channelColor.g, channelColor.b, 0.25f);
        shaderManager->setColor(fillCol);
        
        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(translate_x, translate_y, 0.f));
        model = glm::scale(model, glm::vec3(scale_x, scale_y, 1.f));
        shaderManager->setMVP(model);

        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 34);

        // Restaurar modo de blend para desenho do contorno
        if (!blendWasEnabled) glDisable(GL_BLEND);

        // Desenhar contorno branco do círculo
        shaderManager->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glDrawArrays(GL_LINE_LOOP, 1, 32);

        glBindVertexArray(0);
        shaderManager->endShader();
        
        if (blendWasEnabled) glEnable(GL_BLEND);
    }

    if (depthWasEnabled) glEnable(GL_DEPTH_TEST);
}

void Interface::drawStrokeText(ShaderManager* shaderManager, const std::string& text, float x, float y, float char_w, float char_h, float spacing, const glm::vec4& color)
{
    shaderManager->startShader(ShaderManager::SHADER_COLOR);
    shaderManager->setColor(color);

    std::vector<float> lineVerts;
    float currentX = x;

    for (char c : text)
    {
        auto addLine = [&](float x1, float y1, float x2, float y2) {
            float ndc_x1 = currentX + x1 * char_w;
            float ndc_y1 = y + y1 * char_h;
            float ndc_x2 = currentX + x2 * char_w;
            float ndc_y2 = y + y2 * char_h;
            
            // Linha original
            lineVerts.push_back(ndc_x1);
            lineVerts.push_back(ndc_y1);
            lineVerts.push_back(ndc_x2);
            lineVerts.push_back(ndc_y2);

            // Deslocamento de 1px horizontal para engrossar (bold)
            float offsetX = 1.0f * (2.0f / 1024.0f);
            lineVerts.push_back(ndc_x1 + offsetX);
            lineVerts.push_back(ndc_y1);
            lineVerts.push_back(ndc_x2 + offsetX);
            lineVerts.push_back(ndc_y2);

            // Deslocamento de 1px vertical para engrossar (bold)
            float offsetY = 1.0f * (2.0f / 768.0f);
            lineVerts.push_back(ndc_x1);
            lineVerts.push_back(ndc_y1 + offsetY);
            lineVerts.push_back(ndc_x2);
            lineVerts.push_back(ndc_y2 + offsetY);
        };

        switch (c)
        {
            case 'W':
                addLine(0.f, 1.f, 0.25f, 0.f);
                addLine(0.25f, 0.f, 0.5f, 0.7f);
                addLine(0.5f, 0.7f, 0.75f, 0.f);
                addLine(0.75f, 0.f, 1.f, 1.f);
                break;
            case 'I':
                addLine(0.5f, 0.f, 0.5f, 1.f);
                addLine(0.2f, 0.f, 0.8f, 0.f);
                addLine(0.2f, 1.f, 0.8f, 1.f);
                break;
            case 'R':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.8f, 1.f);
                addLine(0.8f, 1.f, 0.8f, 0.5f);
                addLine(0.8f, 0.5f, 0.f, 0.5f);
                addLine(0.f, 0.5f, 0.8f, 0.f);
                break;
            case 'E':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.8f, 1.f);
                addLine(0.f, 0.5f, 0.6f, 0.5f);
                addLine(0.f, 0.f, 0.8f, 0.f);
                break;
            case 'F':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.8f, 1.f);
                addLine(0.f, 0.5f, 0.6f, 0.5f);
                break;
            case 'A':
                addLine(0.f, 0.f, 0.5f, 1.f);
                addLine(0.5f, 1.f, 1.f, 0.f);
                addLine(0.25f, 0.5f, 0.75f, 0.5f);
                break;
            case 'M':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.5f, 0.3f);
                addLine(0.5f, 0.3f, 1.f, 1.f);
                addLine(1.f, 1.f, 1.f, 0.f);
                break;
            case 'O':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.8f, 1.f);
                addLine(0.8f, 1.f, 0.8f, 0.f);
                addLine(0.8f, 0.f, 0.f, 0.f);
                break;
            case 'T':
                addLine(0.f, 1.f, 0.8f, 1.f);
                addLine(0.4f, 1.f, 0.4f, 0.f);
                break;
            case 'N':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.8f, 0.f);
                addLine(0.8f, 0.f, 0.8f, 1.f);
                break;
            case 'C':
                addLine(0.8f, 1.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.f, 0.f);
                addLine(0.f, 0.f, 0.8f, 0.f);
                break;
            case 'U':
                addLine(0.f, 1.f, 0.f, 0.f);
                addLine(0.f, 0.f, 0.8f, 0.f);
                addLine(0.8f, 0.f, 0.8f, 1.f);
                break;
            case 'L':
                addLine(0.f, 1.f, 0.f, 0.f);
                addLine(0.f, 0.f, 0.8f, 0.f);
                break;
            case 'V':
                addLine(0.f, 1.f, 0.4f, 0.f);
                addLine(0.4f, 0.f, 0.8f, 1.f);
                break;
            case 'S':
                addLine(0.8f, 1.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.f, 0.5f);
                addLine(0.f, 0.5f, 0.8f, 0.5f);
                addLine(0.8f, 0.5f, 0.8f, 0.f);
                addLine(0.8f, 0.f, 0.f, 0.f);
                break;
            case 'P':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.8f, 1.f);
                addLine(0.8f, 1.f, 0.8f, 0.5f);
                addLine(0.8f, 0.5f, 0.f, 0.5f);
                break;
            case 'G':
                addLine(0.8f, 1.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.f, 0.f);
                addLine(0.f, 0.f, 0.8f, 0.f);
                addLine(0.8f, 0.f, 0.8f, 0.5f);
                addLine(0.8f, 0.5f, 0.4f, 0.5f);
                break;
            case 'D':
                addLine(0.f, 0.f, 0.f, 1.f);
                addLine(0.f, 1.f, 0.6f, 1.f);
                addLine(0.6f, 1.f, 0.8f, 0.8f);
                addLine(0.8f, 0.8f, 0.8f, 0.2f);
                addLine(0.8f, 0.2f, 0.6f, 0.f);
                addLine(0.6f, 0.f, 0.f, 0.f);
                break;
            default:
                break;
        }
        currentX += spacing;
    }

    if (!lineVerts.empty())
    {
        GLuint tempVAO, tempVBO;
        glGenVertexArrays(1, &tempVAO);
        glGenBuffers(1, &tempVBO);
        glBindVertexArray(tempVAO);
        glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
        glBufferData(GL_ARRAY_BUFFER, lineVerts.size() * sizeof(float), lineVerts.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        
        shaderManager->setMVP(glm::mat4(1.f));
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, (GLsizei)(lineVerts.size() / 2));
        glLineWidth(1.0f);
        
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &tempVAO);
        glDeleteBuffers(1, &tempVBO);
    }

    shaderManager->endShader();
}

bool Interface::checkAndResetWireframeToggle(void)
{
    if (bToggleWireframe) {
        bToggleWireframe = false;
        return true;
    }
    return false;
}

bool Interface::checkAndResetRotationToggle(void)
{
    if (bToggleRotation) {
        bToggleRotation = false;
        return true;
    }
    return false;
}

bool Interface::checkAndResetCullFaceToggle(void)
{
    if (bToggleCullFace) {
        bToggleCullFace = false;
        return true;
    }
    return false;
}

bool Interface::checkAndResetEvenSpacingToggle(void)
{
    if (bToggleEvenSpacing) {
        bToggleEvenSpacing = false;
        return true;
    }
    return false;
}

bool Interface::checkAndResetOddSpacingToggle(void)
{
    if (bToggleOddSpacing) {
        bToggleOddSpacing = false;
        return true;
    }
    return false;
}

int*   Interface::getMouse(void)  { return mouse; }
int    Interface::getRadius(void) { return radius; }
float* Interface::getNormal(void) { return normal; }
bool   Interface::getUpdate(void) { return bUpdate; }

void Interface::keyPressed(int key)
{
    if (key == '-' && radius > 2)  radius--;
    if (key == '=' && radius < 60) radius++;
}

void Interface::keyReleased(int key)
{
    if (key == '8') color = RED;
    if (key == '9') color = GREEN;
    if (key == '0') color = BLUE;
    if (key == GLFW_KEY_TAB) {
        if      (color == RED)   color = GREEN;
        else if (color == GREEN) color = BLUE;
        else                     color = RED;
    }
}

void Interface::mouseMoved   (int x, int y)            { mouse[0]=x/5; mouse[1]=y/5; }
void Interface::mouseDragged (int x, int y, int btn)   { if (button == 1) return; mouse[0]=x/5; mouse[1]=y/5; button=btn; }

void Interface::mousePressed (int x, int y, int btn)   
{ 


    if (btn == 0) // Clique esquerdo do mouse
    {
        // Detectar se o clique ocorreu dentro da checkbox de Wireframe (x: [15, 160], y: [334, 358])
        if (x >= 15 && x <= 160 && y >= 334 && y <= 358)
        {
            bToggleWireframe = true;
            button = 1; // Desativa a pintura neste clique
            return;
        }
        // Detectar se o clique ocorreu dentro da checkbox de Rotation (x: [15, 160], y: [358, 382])
        if (x >= 15 && x <= 160 && y >= 358 && y <= 382)
        {
            bToggleRotation = true;
            button = 1; // Desativa a pintura neste clique
            return;
        }
        // Detectar se o clique ocorreu dentro da checkbox de Cull Face (x: [15, 160], y: [382, 406])
        if (x >= 15 && x <= 160 && y >= 382 && y <= 406)
        {
            bToggleCullFace = true;
            button = 1; // Desativa a pintura neste clique
            return;
        }
        // Detectar se o clique ocorreu dentro da checkbox de Even Spacing (x: [15, 160], y: [406, 430])
        if (x >= 15 && x <= 160 && y >= 406 && y <= 430)
        {
            bToggleEvenSpacing = true;
            button = 1; // Desativa a pintura neste clique
            return;
        }
        // Detectar se o clique ocorreu dentro da checkbox de Odd Spacing (x: [15, 160], y: [430, 459])
        if (x >= 15 && x <= 160 && y >= 430 && y <= 459)
        {
            bToggleOddSpacing = true;
            button = 1; // Desativa a pintura neste clique
            return;
        }
    }
    mouse[0]=x/5; mouse[1]=y/5; button=btn; 
}

void Interface::mouseReleased(int x, int y, int /*btn*/){ mouse[0]=x/5; mouse[1]=y/5; button=1; }

void Interface::mouseScrolled(double /*xoffset*/, double yoffset)
{
    if (yoffset > 0.0 && radius < 60) {
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