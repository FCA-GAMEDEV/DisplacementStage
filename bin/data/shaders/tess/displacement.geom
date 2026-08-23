#version 420 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform sampler2D samplerDMap;
uniform mat4 MVP;

// ── ILUMINAÇÃO: Inclusão da matriz Model para transformar as normais para o espaço do mundo ────────────────
uniform mat4 Model;

in  vec3 tePosition[3];
in  vec2 teTexCoord[3];
in  vec3 tePatchDistance[3];

out vec2 gTexCoord;

// ── ILUMINAÇÃO: Envio da normal calculada para o Fragment Shader ────────────────
out vec3 gNormal;

void main()
{
    float scale = 10.0;

    // Calcular posições dos vértices deformados em Espaço de Modelo (antes de aplicar MVP)
    vec2 tc0    = teTexCoord[0];
    vec4 h0     = texture(samplerDMap, tc0);
    vec3 p0     = gl_in[0].gl_Position.xyz + vec3(h0.r, h0.b, h0.g) * scale;

    vec2 tc1    = teTexCoord[1];
    vec4 h1     = texture(samplerDMap, tc1);
    vec3 p1     = gl_in[1].gl_Position.xyz + vec3(h1.r, h1.b, h1.g) * scale;

    vec2 tc2    = teTexCoord[2];
    vec4 h2     = texture(samplerDMap, tc2);
    vec3 p2     = gl_in[2].gl_Position.xyz + vec3(h2.r, h2.b, h2.g) * scale;

    // ── ILUMINAÇÃO: Cálculo analítico da normal da face usando o produto vetorial das arestas deformadas ──
    vec3 edge1 = p1 - p0;
    vec3 edge2 = p2 - p0;
    vec3 modelNormal = normalize(cross(edge1, edge2));
    
    // Transformar a normal do espaço de modelo para o espaço do mundo (rotação aplicada)
    gNormal = normalize(vec3(Model * vec4(modelNormal, 0.0)));

    // Emitir os vértices transformados por MVP
    gTexCoord   = tc0;
    gl_Position = MVP * vec4(p0, 1.0);
    EmitVertex();

    gTexCoord   = tc1;
    gl_Position = MVP * vec4(p1, 1.0);
    EmitVertex();

    gTexCoord   = tc2;
    gl_Position = MVP * vec4(p2, 1.0);
    EmitVertex();

    EndPrimitive();
}
