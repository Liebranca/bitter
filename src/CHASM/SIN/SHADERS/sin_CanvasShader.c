/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SIN_CANVASSHADER                      *
*                                           *
*     -default shader for fsrect            *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "sin_CanvasShader.h"
#include "SIN/sin_globals.h"

//   ---     ---     ---     ---     ---

cchar* SIN_CanvasShader_source_v[2] = 
{

SIN_GL_VER,

"\
                                                                                             \
in  vec3  Position;                                                                          \
in  vec2  UV;                                                                                \
                                                                                             \
out vec2  texCoords;                                                                         \
                                                                                             \
void main() {                                                                                \
    gl_Position = vec4(Position.x, Position.y, 0, 1.0);                                      \
    texCoords   = UV;                                                                        \
};                                                                                           \
                                                                                             \
"

};

//   ---     ---     ---     ---     ---

cchar* SIN_CanvasShader_source_f[2] =
{

SIN_GL_VER,

"\
                                                                                             \
in vec2 texCoords;                                                                           \
uniform sampler2DArray Surface;                                                              \
                                                                                             \
void main()                                                                                  \
{                                                                                            \
    vec4 color = texture(Surface, vec3(texCoords, 0));  \
    gl_FragColor = color; \
}                                                                                            \
                                                                                             \
"
};

//   ---     ---     ---     ---     ---

const SHDP SIN_CanvasShader =
{

    SIN_CanvasShader_source_v,                      // Vertex sources
    SIN_CanvasShader_source_f,                      // Fragment sources

    { "Position", "UV"                      },      // Attributes

    {                                       },      // Uniforms

    {                                       },      // UBOs

    { "Surface"                             },      // Samplers

    2,                                              // Number of vertex shader blocks
    2,                                              // Number of fragment shader blocks
    2,                                              // Number of attributes
    0,                                              // Number of uniforms
    0,                                              // Number of UBOs
    1,                                              // Number of samplers

    0                                               // Flags

};

//   ---     ---     ---     ---     ---
