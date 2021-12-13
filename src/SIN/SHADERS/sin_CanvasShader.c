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
in  vec4  Position;\
uniform vec4 Transform;\
\
layout(std140, binding=0) uniform Palette {\
  vec4 color[16];\
\
} _Palette;\
\
layout (std430, binding=1) buffer CharData {\
  flat uint i[2048];\
\
} _CharData;\
\
out vec2  texCoords;\
flat out uint  ch;\
flat out uint  show_ctl;\
out vec4 col;\
\
void main() {\
\
\
  float px=float(uint(Position.x)&1);\
  float py=float(uint(Position.y)&1);\
\
  gl_Position = vec4(\
    (px*Transform[0])+Position.z,\
\
    (py*Transform[1])+(Position.w-Transform[1]),\
    0, 1.0\
  );\
\
  texCoords=vec2(px>0,py>0);\
  uint c_id=uint(Position.x)>>1;\
  ch=_CharData.i[c_id];\
  col=_Palette.color[((ch&0x0000FF00)>>8)&0xF];\
  show_ctl=(ch&0xFF0000)>>16;\
  ch=ch&0xFF;\
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
flat in uint ch;\
flat in uint show_ctl;\
in vec4 col;\
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
  uint chidex = ch&0xFF;\
\
  i-=z*32;\
  bool r = bool(lycon[chidex][z]&(1<<i));\
  r=r&&(chidex>=show_ctl);\
\
  gl_FragColor = vec4(col.rgb,col.w*float(r));\
}\
"
};

//   ---     ---     ---     ---     ---

const SHDP SIN_CanvasShader =
{

    SIN_CanvasShader_source_v,                      // Vertex sources
    SIN_CanvasShader_source_f,                      // Fragment sources

    { "Position"                            },      // Attributes

    { "Transform"                           },      // Uniforms

    { "Palette"                             },      // UBOs

    {                                       },      // Samplers

    2,                                              // Number of vertex shader blocks
    3,                                              // Number of fragment shader blocks
    1,                                              // Number of attributes
    1,                                              // Number of uniforms
    1,                                              // Number of UBOs
    1,                                              // Number of samplers

    0                                               // Flags

};

//   ---     ---     ---     ---     ---
