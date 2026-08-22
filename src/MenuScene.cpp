#include "MenuScene.h"
#include <GLFW/glfw3.h>

MenuScene::MenuScene(DisplacementStage * displacementStage)
	: Scene(displacementStage)
{
	this->image = new Texture("data/menu.png");
	this->initQuad();
}

MenuScene::~MenuScene(void)
{
	if (this->image)
	{
		delete this->image;
		this->image = nullptr;
	}

	if (menuVAO) { glDeleteVertexArrays(1, &menuVAO); menuVAO = 0; }
	if (menuVBO) { glDeleteBuffers(1, &menuVBO);      menuVBO = 0; }
}

void MenuScene::initQuad(void)
{
	// Full screen quad in NDC
	float verts[] = {
		-1.f,  1.f,   0.f, 1.f, // Top-left
		 1.f,  1.f,   1.f, 1.f, // Top-right
		-1.f, -1.f,   0.f, 0.f, // Bottom-left
		 1.f, -1.f,   1.f, 0.f, // Bottom-right
	};

	glGenVertexArrays(1, &menuVAO);
	glGenBuffers(1, &menuVBO);

	glBindVertexArray(menuVAO);
	glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	// Pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// UV
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

void MenuScene::update(void)
{
}

void MenuScene::draw(void)
{
	glDisable(GL_DEPTH_TEST);

	shaderManager->startShader(ShaderManager::SHADER_SIMPLE);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(shaderManager->getSimpleSamplerTexture(), 0);
	glBindTexture(GL_TEXTURE_2D, this->image->getID());

	shaderManager->setMVP(glm::mat4(1.0f));

	glBindVertexArray(menuVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	shaderManager->endShader();
}

void MenuScene::keyPressed(int key)
{
}

void MenuScene::keyReleased(int key)
{
	if (key == '1' || key == GLFW_KEY_1)      SceneManager::getInstance().changeScene(1);
	else if (key == '2' || key == GLFW_KEY_2) SceneManager::getInstance().changeScene(2);
	else if (key == '3' || key == GLFW_KEY_3) SceneManager::getInstance().changeScene(3);
	else if (key == '4' || key == GLFW_KEY_4) SceneManager::getInstance().changeScene(4);
	else if (key == '5' || key == GLFW_KEY_5) SceneManager::getInstance().changeScene(5);
}
