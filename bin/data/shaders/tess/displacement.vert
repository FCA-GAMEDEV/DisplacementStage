#version 420 core

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

out vec3 vPosition;
out vec2 vTexCoord;

void main()
{
    vTexCoord = TexCoord;
    vPosition = Position.xyz;
}
