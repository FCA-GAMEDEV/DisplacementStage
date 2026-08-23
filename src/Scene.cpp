#include "Scene.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <string>
#include <sstream>
#include <ctime>

DisplacementStage * Scene::displacementStage = NULL;
ShaderManager * Scene::shaderManager = NULL;
Terrain * Scene::terrain = NULL;
Texture * Scene::texture = NULL;
DisplacementMap * Scene::displacementMap = NULL;
Interoperability * Scene::interoperability = NULL;

Scene::Scene(DisplacementStage * displacementStage)
{
	if(!Scene::displacementStage) Scene::displacementStage = displacementStage;
	if(!Scene::shaderManager)     Scene::shaderManager = &ShaderManager::getInstance();
	if(!Scene::terrain)           Scene::terrain = new Terrain;
	if(!Scene::texture)           Scene::texture = new Texture("data/textures/texture.png");
	if(!Scene::displacementMap)   Scene::displacementMap = new DisplacementMap("data/textures/generalMap.png");
	if(!Scene::interoperability)  Scene::interoperability = new Interoperability;

	this->terrain->setWireframe(false);
	this->terrain->setRotate(false);
	this->terrain->setAngle(0);
	this->terrain->setTessellationFactor(64,64);

	this->bPause = false;
	this->decorator = new Interface;
	this->bShowDecorator = false;
}

Scene::~Scene(void)
{
	if(this->decorator)
	{
		delete this->decorator;
		this->decorator = NULL;
	}
}

void Scene::draw(void)
{
	if (this->decorator)
		this->decorator->draw(this->shaderManager, this->displacementMap, false, this->terrain->getWireframe(), this->terrain->getRotate(), this->terrain->getCullFace(), ShaderManager::getInstance().getTessellationScheme(), this->bShowDecorator);
}

void Scene::clear(void)
{
	if(Scene::interoperability)
	{
		delete Scene::interoperability;
		Scene::interoperability = NULL;
	}

	if(Scene::displacementMap)
	{
		delete Scene::displacementMap;
		Scene::displacementMap = NULL;
	}

	if(Scene::texture) 
	{
		delete Scene::texture;
		Scene::texture = NULL;
	}

	if(Scene::terrain)
	{
		delete Scene::terrain;
		Scene::terrain = NULL;
	}
}

void Scene::keyPressed(int key)
{
	if (key == GLFW_KEY_F1)
		this->terrain->decreaseTessellationFactor(1,0);

	if (key == GLFW_KEY_F2)
		this->terrain->increaseTessellationFactor(1,0);

	if (key == GLFW_KEY_F3)
		this->terrain->decreaseTessellationFactor(0,1);

	if (key == GLFW_KEY_F4)
		this->terrain->increaseTessellationFactor(0,1);

	if (key == GLFW_KEY_F5)
		this->terrain->decreaseTessellationFactor(1,1);

	if (key == GLFW_KEY_F6)
		this->terrain->increaseTessellationFactor(1,1);
}

void Scene::updateCamera(void)
{
	if (this->decorator)
	{
		if (this->decorator->checkAndResetWireframeToggle())
			this->terrain->setWireframe(!this->terrain->getWireframe());

		if (this->decorator->checkAndResetRotationToggle())
			this->terrain->setRotate(!this->terrain->getRotate());

		if (this->decorator->checkAndResetCullFaceToggle())
			this->terrain->setCullFace(!this->terrain->getCullFace());

		if (this->decorator->checkAndResetEvenSpacingToggle())
		{
			// EVEN é o mínimo: se já está ativo, não faz nada (nunca fica sem seleção)
			int current = ShaderManager::getInstance().getTessellationScheme();
			if (current != 1)
				ShaderManager::getInstance().setTessellationScheme(1);
		}

		if (this->decorator->checkAndResetOddSpacingToggle())
		{
			// Se ODD já está ativo, volta para EVEN (nunca para Equal/0)
			int current = ShaderManager::getInstance().getTessellationScheme();
			ShaderManager::getInstance().setTessellationScheme(current == 2 ? 1 : 2);
		}
	}

	GLFWwindow* window = glfwGetCurrentContext();
	if (window)
	{
		float speed = 0.5f; // velocidade de movimento por frame
		float dx = 0.f, dy = 0.f, dz = 0.f;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dz += speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dz -= speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dx -= speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dx += speed;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) dy += speed;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) dy -= speed;

		if (dx != 0.f || dy != 0.f || dz != 0.f)
		{
			this->camera.move(dx, dy, dz);
		}
	}
}

void Scene::keyReleased(int key)
{
	if (key == GLFW_KEY_F7)
		ShaderManager::getInstance().reloadShaders();

	if (key == GLFW_KEY_F11) 
		this->terrain->setWireframe(!this->terrain->getWireframe());

	if (key == 'R' || key == 'r') 
		this->terrain->setRotate(!this->terrain->getRotate());

	if (key == 'P' || key == 'p') 
		this->bPause = !this->bPause;

	if (key == GLFW_KEY_F9)
		this->bShowDecorator = !this->bShowDecorator;

	if (key == 'S' || key == 's')
	{
		unsigned char * _p = new unsigned char[64*64*4];
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, displacementMap->getID());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, _p);		
		
		std::time_t t = std::time(nullptr);
		std::ostringstream oss;
		oss << "data/dmap/dmap_" << t << ".png";
		std::string path = oss.str();

		// stb_image_write salva de cabeça para baixo por padrão em relação ao OpenGL, então podemos dar flip
		stbi_flip_vertically_on_write(true);
		stbi_write_png(path.c_str(), 64, 64, 4, _p, 64 * 4);

		delete [] _p;
		cout << "Imagem \"" << path.c_str() << "\" criada com sucesso!" << endl;
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

static int lastMouseX = 0;
static int lastMouseY = 0;
static bool isOrbiting = false;

void Scene::mouseMoved(int x, int y)
{
}

void Scene::mouseDragged(int x, int y, int button)
{
	if (isOrbiting)
	{
		float dx = (float)(x - lastMouseX);
		float dy = (float)(y - lastMouseY);
		lastMouseX = x;
		lastMouseY = y;

		// Rotacionar visão da câmera (olhar ao redor)
		// Sensibilidade: 0.003 radianos por pixel
		this->camera.orbit(dx * 0.003f, -dy * 0.003f);
	}
}

void Scene::mousePressed(int x, int y, int button)
{
	if (this->decorator)
	{
		this->decorator->mousePressed(x, y, button);
	}
	
	// Se for clique esquerdo na área dos checkboxes (x: 15..160, y: 334..459), não inicia órbita da câmera
	if (button == 0 && x >= 15 && x <= 160 && y >= 334 && y <= 459)
	{
		return;
	}

	lastMouseX = x;
	lastMouseY = y;
	isOrbiting = true;
}

void Scene::mouseReleased(int x, int y, int button)
{
	isOrbiting = false;
}

void Scene::mouseScrolled(double xoffset, double yoffset)
{
	GLFWwindow* window = glfwGetCurrentContext();
	bool isCtrlPressed = false;
	if (window)
	{
		isCtrlPressed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
		                 glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
	}

	if (this->decorator && !isCtrlPressed)
	{
		this->decorator->mouseScrolled(xoffset, yoffset);
	}
	else
	{
		// Zoom da câmera: Sensibilidade de 2 unidades por tick de scroll
		this->camera.zoom(-yoffset * 2.0f);
	}
}