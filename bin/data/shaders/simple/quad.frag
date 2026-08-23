#version 420 core

in  vec2 TexCoord0;
out vec4 FragColor;

uniform sampler2D samplerTex;

void main()
{
    FragColor = texture(samplerTex, TexCoord0);
}
