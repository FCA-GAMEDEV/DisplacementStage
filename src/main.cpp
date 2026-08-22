#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include "Game.h"

// ── Callbacks GLFW ──────────────────────────────────────────
static void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
        game->keyPressed(key);
    else if (action == GLFW_RELEASE)
        game->keyReleased(key);
}

static void cursorPosCallback(GLFWwindow* window, double x, double y)
{
    auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    // Botão mantido pressionado = drag
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS)
        game->mouseDragged((int)x, (int)y, 0);
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        game->mouseDragged((int)x, (int)y, 2);
    else
        game->mouseMoved((int)x, (int)y);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/)
{
    auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // Mapeia botões GLFW (0=esq,1=meio,2=dir) para OF (0=esq,1=meio,2=dir)
    int btn = (button == GLFW_MOUSE_BUTTON_LEFT)   ? 0 :
              (button == GLFW_MOUSE_BUTTON_MIDDLE)  ? 1 : 2;

    if (action == GLFW_PRESS)
        game->mousePressed((int)x, (int)y, btn);
    else if (action == GLFW_RELEASE)
        game->mouseReleased((int)x, (int)y, btn);
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    game->mouseScrolled(xoffset, yoffset);
}

// ── main ─────────────────────────────────────────────────────
int main()
{
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Erro: falha ao inicializar GLFW\n";
        return -1;
    }

    // Solicitar OpenGL 4.2 Core Profile (necessário para tessellation shaders)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Displacement Stage", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erro: falha ao criar janela GLFW (OpenGL 4.2 core)\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync ligado — evita 100% CPU e travamentos

    // Inicializar GLEW (deve ser após o contexto GL estar ativo)
    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << "Erro GLEW: " << glewGetErrorString(glewErr) << "\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GPU:    " << glGetString(GL_RENDERER) << "\n";

    // Instanciar e configurar o jogo
    Game game;
    glfwSetWindowUserPointer(window, &game);

    // Registrar callbacks
    glfwSetKeyCallback        (window, keyCallback);
    glfwSetCursorPosCallback  (window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback     (window, scrollCallback);

    game.setup();

    // ── Game loop ──────────────────────────────────────────
    double lastTime = glfwGetTime();
    double fpsTimer = lastTime;
    int    frames   = 0;

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        frames++;

        // Atualizar título com FPS a cada 0.5s
        if (now - fpsTimer >= 0.5) {
            double fps = frames / (now - fpsTimer);
            char title[64];
            std::snprintf(title, sizeof(title), "Displacement Stage — FPS: %.0f", fps);
            glfwSetWindowTitle(window, title);
            fpsTimer = now;
            frames   = 0;
        }

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game.update();
        game.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        lastTime = now;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
