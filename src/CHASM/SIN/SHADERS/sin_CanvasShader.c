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
#include "sin_lycon.h"

#include "SIN/sin_globals.h"

//   ---     ---     ---     ---     ---

cchar* SIN_CanvasShader_source_v[2] = 
{

SIN_GL_VER,

"\
in  vec3  Position;\
in  vec2  UV;\
\
out vec2  texCoords;\
\
void main() {\
    gl_Position = vec4(Position.x, Position.y, 0, 1.0);\
    texCoords   = (UV+1)*0.5;\
};\
"

};

//   ---     ---     ---     ---     ---

cchar* SIN_CanvasShader_source_f[3] =
{

SIN_GL_VER,

FONTS_LYCON,

"\
\
in vec2 texCoords;\
uniform sampler2DArray Surface;\
uniform uint drCH;\
\
\
\
void main() {\
\
    uint x = uint(texCoords.x*8);\
    uint y = uint(texCoords.y*8);\
\
    uint i = x+(y*8);\
    uint z = uint(i > 31);\
\
    i-=z*32;\
    bool r = !bool(lycon[drCH][z]&(1<<i));\
\
\
    vec4 color = vec4(r,r,r,!r);\
    gl_FragColor = color;\
}\
"
};

//   ---     ---     ---     ---     ---

const SHDP SIN_CanvasShader =
{

    SIN_CanvasShader_source_v,                      // Vertex sources
    SIN_CanvasShader_source_f,                      // Fragment sources

    { "Position", "UV"                      },      // Attributes

    { "drCH"                                },      // Uniforms

    {                                       },      // UBOs

    { "Surface"                             },      // Samplers

    2,                                              // Number of vertex shader blocks
    3,                                              // Number of fragment shader blocks
    2,                                              // Number of attributes
    1,                                              // Number of uniforms
    0,                                              // Number of UBOs
    1,                                              // Number of samplers

    0                                               // Flags

};

//   ---     ---     ---     ---     ---
