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
	this->decorator = NULL;
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
	if (this->bShowDecorator && this->decorator)
		this->decorator->draw(this->shaderManager, this->displacementMap);
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

	if (key == GLFW_KEY_PAGE_UP)
		this->terrain->increaseCameraPosition(0,1,0);

	if (key == GLFW_KEY_PAGE_DOWN)
		this->terrain->decreaseCameraPosition(0,1,0);

	if (key == GLFW_KEY_UP)
		this->terrain->decreaseCameraPosition(0,0,1);

	if (key == GLFW_KEY_DOWN)
		this->terrain->increaseCameraPosition(0,0,1);

	if (key == GLFW_KEY_RIGHT)
		this->terrain->increaseCameraPosition(1,0,0);

	if (key == GLFW_KEY_LEFT)
		this->terrain->decreaseCameraPosition(1,0,0);
}

void Scene::keyReleased(int key)
{
	if (key == 'W' || key == 'w') 
		this->terrain->setWireframe(!this->terrain->getWireframe());

	if (key == 'R' || key == 'r') 
		this->terrain->setRotate(!this->terrain->getRotate());

	if (key == 'P' || key == 'p') 
		this->bPause = !this->bPause;

	if (key == 'D' || key == 'd')
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

		// Rotacionar câmera orbital
		// Sensibilidade: 0.005 radianos por pixel
		this->terrain->orbitCamera(-dx * 0.005f, -dy * 0.005f);
	}
}

void Scene::mousePressed(int x, int y, int button)
{
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
		// Zoom da câmera orbital: Sensibilidade de 2 unidades por tick de scroll
		this->terrain->zoomCamera(-yoffset * 2.0f);
	}
}