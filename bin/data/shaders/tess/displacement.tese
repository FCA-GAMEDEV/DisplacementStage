#version 420 core

layout(triangles, equal_spacing, cw) in;

in  vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;
in  vec2 tcTexCoord[];
out vec2 teTexCoord;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    vec2 t0 = gl_TessCoord.x * tcTexCoord[0];
    vec2 t1 = gl_TessCoord.y * tcTexCoord[1];
    vec2 t2 = gl_TessCoord.z * tcTexCoord[2];
    tePatchDistance = gl_TessCoord;
    tePosition      = p0 + p1 + p2;
    teTexCoord      = t0 + t1 + t2;
    gl_Position     = vec4(tePosition, 1.0);
}
