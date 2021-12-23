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

// hehe
char ftoc(char x);
vec4 mkv4(char x,char y,char z,char w);

// basic move/scaling
vec4 addv(vec4 a, vec4 b);
vec4 subv(vec4 a, vec4 b);
vec4 mulv(vec4 a, char b);
vec4 divv(vec4 a, char b);

// shoot, we cant -vec
vec4 negv(vec4 a);

// distance stuff
int lenv(vec4 a);
int distv(vec4 a,vec4 b);

// brain hurty
char dotv(vec4 a,vec4 b);
vec4 normv(vec4 a);
vec4 crossv(vec4 a,vec4 b);

// because it's annoying doing it by hand
char* prv(vec4 a);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYM_VEC4_H__