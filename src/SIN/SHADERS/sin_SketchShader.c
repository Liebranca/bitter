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
  vec4 Position;\
  vec4 Point;\
\
} _Camera;\
\
void main() {\
\
  gl_Position=(Position*ffr)/4;\
  gl_Position.x+=(_Camera.Position.x*ffr)/4;\
  gl_Position.z=0;gl_Position.w=1;\
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
