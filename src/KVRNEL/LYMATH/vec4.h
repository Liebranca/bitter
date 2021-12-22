#ifndef __LYM_VEC4_H__
#define __LYM_VEC4_H__

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct {

  union {
    struct {
      char x;
      char y;
      char z;
      char w;

    };char mem[4];
  };

} vec4;

//   ---     ---     ---     ---     ---

// basic move/scaling
void addv(vec4* a, vec4* b);
void subv(vec4* a, vec4* b);
void mulv(vec4* a, char b);
void divv(vec4* a, char b);

// distance stuff
int lenv(vec4* a);
vec4 gtvecto(vec4* a,vec4* b);

int distv(vec4* a,vec4* b);

// brain hurty
vec4 crossv(vec4* a,vec4* b);

// because it's annoying doing it by hand
char* prv(vec4* a);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYM_VEC4_H__