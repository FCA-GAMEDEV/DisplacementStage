#version 420 core

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;

out vec2 TexCoord0;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(Position, 0.0, 1.0);
    TexCoord0   = TexCoord;
}
