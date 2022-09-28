#ifndef __LYM_MAT4_H__
#define __LYM_MAT4_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vec4.h"

//   ---     ---     ---     ---     ---

typedef struct {

  vec4 a;
  vec4 b;
  vec4 c;
  vec4 d;

} mat4;

//   ---     ---     ---     ---     ---

mat4 lookat(vec4 pos,vec4 at,vec4 up);
mat4 normm(mat4* m);
void prm(mat4* m);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYM_MAT4_H__