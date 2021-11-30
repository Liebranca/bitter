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

#include "sin_globals.h"

//   ---     ---     ---     ---     ---

cchar* SIN_CanvasShader_source_v[2] = 
{

SIN_GL_VER,

"\
in  vec3  Position;\
uniform vec4 Transform;\
uniform uvec4 CharData;\
\
out vec2  texCoords;\
out uint  chidex;\
\
void main() {\
\
    gl_Position = vec4((Position.x*Transform[0])+Transform[1],\
                       (Position.y*Transform[2])+(Transform[3]-Transform[2]), 0, 1.0);\
\
    texCoords   = vec2(Position.x>0, Position.y>0);\
    chidex      = CharData[0];\
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
flat in uint chidex;\
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
    bool r = bool(lycon[chidex][z]&(1<<i)); r=r&&chidex>0x1F;\
\
\
    vec4 color = vec4(64.0/255.0,174.0/255.0,64.0/255.0,r);\
    gl_FragColor = color;\
}\
"
};

//   ---     ---     ---     ---     ---

const SHDP SIN_CanvasShader =
{

    SIN_CanvasShader_source_v,                      // Vertex sources
    SIN_CanvasShader_source_f,                      // Fragment sources

    { "Position"                            },      // Attributes

    { "Transform", "CharData"               },      // Uniforms

    {                                       },      // UBOs

    {                                       },      // Samplers

    2,                                              // Number of vertex shader blocks
    3,                                              // Number of fragment shader blocks
    1,                                              // Number of attributes
    1,                                              // Number of uniforms
    0,                                              // Number of UBOs
    0,                                              // Number of samplers

    0                                               // Flags

};

//   ---     ---     ---     ---     ---
