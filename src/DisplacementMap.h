#pragma once

#include "Texture.h"

class DisplacementMap : public Texture
{
public:
	DisplacementMap(std::string imageName) : Texture(imageName) { }
	~DisplacementMap(void);
};

