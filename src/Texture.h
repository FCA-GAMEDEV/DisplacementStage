#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
public:
    Texture(const std::string& imageName);
    virtual ~Texture(void);
    unsigned int   getID(void);
    unsigned short getWidth(void);
    unsigned short getHeight(void);

protected:
    unsigned int id = 0;

private:
    unsigned short width  = 0;
    unsigned short height = 0;
};
