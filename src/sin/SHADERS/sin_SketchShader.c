#include "sin_SketchShader.h"
#include "sin_globals.h"

//   ---     ---     ---     ---     ---

cchar* source_v[2] = {

SIN_GL_VER,

"\
const float ffr=1.0/32.0;\
in vec4 Position;\
in vec4 ShData;\
\
out vec4 col;\
\
layout(std140, binding=0) uniform Palette {\
  vec4 color[16];\
\
} _Palette;\
\
layout(std140, binding=1) uniform Camera {\
  ivec4 View;\
\
} _Camera;\
\
float sin_icast(int x,int mask) {\
  x=(x&(0xFF<<(mask*8))) >>(mask*8);\
  return float(-x *(-1*int(x<0x80)) )*ffr;\
\
};\
\
vec4 sin_ivcast(int x) {\
\
  vec4 v=vec4(0,0,0,1);\
  for(int i=0;i<4;i++) {\
    v[i]=sin_icast(x,i);\
\
  };\
\
  return v;\
\
};\
\
void main() {\
\
  mat4 view=mat4(\
    sin_ivcast(_Camera.View.x),\
    sin_ivcast(_Camera.View.y),\
    sin_ivcast(_Camera.View.z),\
    sin_ivcast(_Camera.View.w)\
\
  );\
\
  gl_Position=(Position*ffr)/4;\
  gl_Position.z=0;gl_Position.w=1;\
  gl_Position*=view;\
  col=_Palette.color[int(ShData.x)&0xF];\
\
};\
"

};

//   ---     ---     ---     ---     ---

cchar* source_f[2] = {

SIN_GL_VER,

"\
in vec4 col;\
void main() {\
  gl_FragColor=col;\
}\
"
};

//   ---     ---     ---     ---     ---

const SHDP SIN_SketchShader = {

  // sources
  source_v,
  source_f,

  // Attributes
  { "Position", "ShData" },

  // Uniforms
  { "" },

  // UBOs
  { "Palette", "Camera" },

  // Samplers
  {},

  // Number of vertex shader blocks
  2,

  // Number of fragment shader blocks
  2,

  // Number of attributes
  2,

  // Number of uniforms
  0,

  // Number of UBOs
  2,

  // Number of samplers
  0,

  // Flags
  0

};

//   ---     ---     ---     ---     ---
