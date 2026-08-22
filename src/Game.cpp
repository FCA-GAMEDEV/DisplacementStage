#include "Game.h"
#include <iostream>

//--------------------------------------------------------------
void Game::setup()
{
    this->sceneManager = &SceneManager::getInstance();
    this->timeManager  = &TimeManager::getInstance();
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
