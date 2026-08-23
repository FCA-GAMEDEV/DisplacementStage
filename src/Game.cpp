#include "Game.h"
#include <iostream>

//--------------------------------------------------------------
void Game::setup()
{
    this->sceneManager = &SceneManager::getInstance();
    this->timeManager  = &TimeManager::getInstance();

    std::cout << "============================================================\n";
    std::cout << "           DISPLACEMENT STAGE - GUIA DE CONTROLES\n";
    std::cout << "============================================================\n";
    std::cout << "[ 1 a 5 ]     - Trocar de Cena (Cena 1 a 5)\n";
    std::cout << "[ ESPACO ]    - Voltar para o Menu Principal (Cena 0)\n";
    std::cout << "[ ESC ]       - Fechar a aplicacao\n\n";
    std::cout << "CAMERA 3D:\n";
    std::cout << "[ Meio Mouse ]- Orbita (Arcball) - Cenas 1-4: Esq/Dir tambem orbita\n";
    std::cout << "[ W/S/A/D ]   - Mover foco da camera (continuo, relativo a orientacao)\n";
    std::cout << "[ E / Q ]     - Elevar / Abaixar foco da camera (eixo Y)\n";
    std::cout << "[ Scroll ]    - Zoom in/out  |  Cena 5: Ctrl+Scroll\n\n";
    std::cout << "VISUALIZACAO:\n";
    std::cout << "[ F7 ]        - Hot-reload dos shaders GLSL (sem reiniciar)\n";
    std::cout << "[ F9 ]        - Mostrar / Ocultar mini-mapa 2D da textura\n";
    std::cout << "[ F11 ]       - Ativar / Desativar modo Wireframe\n";
    std::cout << "[ R ]         - Ativar / Desativar rotacao automatica do terreno\n";
    std::cout << "[ P ]         - Pausar / Despausar a simulacao fisica do relevo\n";
    std::cout << "[ M ]         - Salvar captura (.png) do mapa de deslocamento atual\n\n";
    std::cout << "CHECKBOXES (visiveis em todas as cenas):\n";
    std::cout << "  WIREFRAME   - Modo wireframe da malha\n";
    std::cout << "  ROTATION    - Rotacao automatica\n";
    std::cout << "  CULL FACE   - Culling de faces traseiras\n";
    std::cout << "  EVEN/ODD    - Esquema de tessellation (radio buttons)\n\n";
    std::cout << "SCULPT/PINCEL (Cena 5):\n";
    std::cout << "[ Scroll ]    - Ajustar raio do pincel\n";
    std::cout << "[ = / - ]     - Ajustar raio do pincel via teclado\n";
    std::cout << "[ Clique Esq ]- Esculpir relevo positivo (elevacao)\n";
    std::cout << "[ Clique Dir ]- Esculpir relevo negativo (cratera)\n";
    std::cout << "[ 8, 9, 0 ]   - Trocar canal de deformacao (R/G/B)\n";
    std::cout << "[ TAB ]       - Ciclar canal de deformacao (R->G->B->R)\n\n";
    std::cout << "TESSELACAO (GPU):\n";
    std::cout << "[ F1 / F2 ]   - Diminuir / Aumentar Tessellation Interno\n";
    std::cout << "[ F3 / F4 ]   - Diminuir / Aumentar Tessellation Externo\n";
    std::cout << "[ F5 / F6 ]   - Diminuir / Aumentar Ambos simultaneamente\n";
    std::cout << "============================================================\n\n";
}

//--------------------------------------------------------------
void Game::update()
{
    this->timeManager->update();
    this->sceneManager->update();
}

//--------------------------------------------------------------
void Game::draw()
{
    this->sceneManager->draw();
}

//--------------------------------------------------------------
void Game::keyPressed(int key)
{
    this->sceneManager->keyPressed(key);
}

//--------------------------------------------------------------
void Game::keyReleased(int key)
{
    this->sceneManager->keyReleased(key);
}

//--------------------------------------------------------------
void Game::mouseMoved(int x, int y)
{
    this->sceneManager->mouseMoved(x, y);
}

//--------------------------------------------------------------
void Game::mouseDragged(int x, int y, int button)
{
    this->sceneManager->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void Game::mousePressed(int x, int y, int button)
{
    this->sceneManager->mousePressed(x, y, button);
}

//--------------------------------------------------------------
void Game::mouseReleased(int x, int y, int button)
{
    this->sceneManager->mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void Game::mouseScrolled(double xoffset, double yoffset)
{
    this->sceneManager->mouseScrolled(xoffset, yoffset);
}
