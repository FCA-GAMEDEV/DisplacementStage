#version 420 core

in  vec2 gTexCoord;

// ── ILUMINAÇÃO: Normal da face recebida do Geometry Shader ────────────────────
in  vec3 gNormal;

out vec4 FragColor;

uniform sampler2D samplerTex;
uniform sampler2D samplerDMap;

void main()
{
    // Obter cor base da textura
    vec4 baseColor = texture(samplerTex, gTexCoord);

    // ── ILUMINAÇÃO: Definição da luz direcional inclinada (de cima para baixo, frente para trás, esquerda para direita)
    // O vetor abaixo aponta da superfície na direção da fonte de luz (inverso de (1.0, -1.0, -1.0))
    vec3 lightDir = normalize(vec3(-1.0, 1.0, 1.0));

    // Componente difuso (Lambert)
    vec3 normal = normalize(gNormal);
    float diff = max(dot(normal, lightDir), 0.0);

    // Componentes de luz ambiente e difusa balanceados para manter legibilidade sem superexposição
    float ambientCoeff = 0.35;
    float diffuseCoeff = 0.65;
    float lightIntensity = ambientCoeff + diffuseCoeff * diff;

    // Aplicar intensidade de iluminação à cor base do fragmento
    FragColor = vec4(baseColor.rgb * lightIntensity, baseColor.a);
}
