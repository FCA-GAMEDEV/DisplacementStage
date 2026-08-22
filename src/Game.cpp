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
    std::cout << "CONTROLES DO TERRENO 3D:\n";
    std::cout << "[ W ]         - Ativar / Desativar modo Wireframe (Grade)\n";
    std::cout << "[ R ]         - Ativar / Desativar rotacao automatica do terreno\n";
    std::cout << "[ P ]         - Pausar / Despausar a simulacao fisica do relevo\n";
    std::cout << "[ S ]         - Salvar captura (.png) do mapa de deslocamento atual\n\n";
    std::cout << "CONTROLES DE CAMERA:\n";
    std::cout << "[ Setas ]     - Mover camera para Frente/Tras/Esquerda/Direita\n";
    std::cout << "[ PgUp/PgDn ] - Mover camera para Cima/Baixo\n\n";
    std::cout << "CONTROLES DE SCULPT/PINCEL (Cena 5):\n";
    std::cout << "[ D ]         - Mostrar / Ocultar pincel e mini-mapa 2D\n";
    std::cout << "[ ScrollMouse]- Ajustar raio do pincel (rodinha do mouse)\n";
    std::cout << "[ = / - ]     - Ajustar raio do pincel via teclado\n";
    std::cout << "[ Clique Esq ]- Esculpir / Deformar relevo positivo\n";
    std::cout << "[ Clique Dir ]- Esculpir / Deformar relevo negativo\n";
    std::cout << "[ 8, 9, 0 ]   - Trocar canal de deformacao (Vermelho/Verde/Azul)\n\n";
    std::cout << "TESSELACAO (Subdivisao de Malha na GPU):\n";
    std::cout << "[ F1 / F2 ]   - Diminuir / Aumentar Tessellation Interno (Inner)\n";
    std::cout << "[ F3 / F4 ]   - Diminuir / Aumentar Tessellation Externo (Outer)\n";
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
