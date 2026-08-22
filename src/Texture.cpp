// stb_image: incluir a implementação UMA VEZ em todo o projeto
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"
#include <iostream>

Texture::Texture(const std::string& imageName)
{
    // stb_image carrega a imagem e faz flip vertical (OpenGL origin = bottom-left)
    stbi_set_flip_vertically_on_load(true);

    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(imageName.c_str(), &w, &h, &channels, 4); // força RGBA

    if (!data) {
        std::cerr << "Texture: falha ao carregar '" << imageName << "': "
                  << stbi_failure_reason() << "\n";
        return;
    }

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    this->width  = static_cast<unsigned short>(w);
    this->height = static_cast<unsigned short>(h);

    stbi_image_free(data);
}

Texture::~Texture(void)
{
    if (this->id) {
        glDeleteTextures(1, &this->id);
        this->id = 0;
    }
}

unsigned int Texture::getID(void)         { return this->id; }
unsigned short Texture::getWidth(void)    { return this->width; }
unsigned short Texture::getHeight(void)   { return this->height; }
