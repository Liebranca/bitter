#ifndef __LYM_VEC4_H__
#define __LYM_VEC4_H__

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct {

  union {
    struct {
      float x;
      float y;
      float z;
      float w;

    };float mem[4];
  };

} vec4;

typedef struct {

  union {
    struct {
      int x;
      int y;
      int z;
      int w;

    };int mem[4];
  };

} ivec4;

//   ---     ---     ---     ---     ---

// basic move/scaling
void addv(vec4* a, vec4* b);
void subv(vec4* a, vec4* b);
void mulv(vec4* a, float b);
void divv(vec4* a, float b);

// distance stuff
int lenv(vec4* a);
vec4 gtvecto(vec4* a,vec4* b);
int distv(vec4* a,vec4* b);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYM_VEC4_H__