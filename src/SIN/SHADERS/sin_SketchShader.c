#include "sin_SketchShader.h"
#include "sin_globals.h"

//   ---     ---     ---     ---     ---

cchar* source_v[2] =
{

SIN_GL_VER,

"\
const float ffr=1.0/32.0;\
in ivec4 Position;\
out vec4 col;\
\
void main() {\
\
\
  float px=float(Position.x)*ffr;\
  float py=float(Position.y)*ffr;\
\
  gl_Position = vec4(px/4,py/4,0,1);\
  col=vec4(abs(px)/4,abs(py)/4,0,1);\
\
};\
"

};

//   ---     ---     ---     ---     ---

cchar* source_f[2] =
{

SIN_GL_VER,

"\
in vec4 col;\
void main() {\
  gl_FragColor=col;\
}\
"
};

//   ---     ---     ---     ---     ---

const SHDP SIN_SketchShader =
{

  // sources
  source_v,
  source_f,

  // Attributes
  { "Position"},

  // Uniforms
  { "Transform"},

  // UBOs
  { "Palette"},

  // Samplers
  {},

  // Number of vertex shader blocks
  2,

  // Number of fragment shader blocks
  2,

  // Number of attributes
  1,

  // Number of uniforms
  1,

  // Number of UBOs
  1,

  // Number of samplers
  1,

  // Flags
  0

};

//   ---     ---     ---     ---     ---
