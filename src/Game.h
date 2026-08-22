#pragma once

#include "SceneManager.h"
#include "TimeManager.h"

// Classe principal do jogo — substituiu herança de ofBaseApp
// Os callbacks são chamados pelo main.cpp via GLFW
class Game {

public:
    void setup();
    void update();
    void draw();

    void keyPressed   (int key);
    void keyReleased  (int key);
    void mouseMoved   (int x, int y);
    void mouseDragged (int x, int y, int button);
    void mousePressed (int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseScrolled(double xoffset, double yoffset);

    SceneManager * sceneManager;
    TimeManager  * timeManager;
};
