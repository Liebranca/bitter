#ifndef __SH_CHAR_H__
#define __SH_CHAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

//   ---     ---     ---     ---     ---

typedef struct {

  char ch;
  char col;
  char ctl;
  char pad;

} ichar;

//   ---     ---     ---     ---     ---

// set canvas data
void sticsc(ichar* buff,size_t x,size_t y);
void sticcol(char col);
void sticctl(char ctl);
void sticpos(size_t* pos);

// get canvas data
void gticpos(size_t* pos);
size_t gticposr(void);

// get handle to draw buffer
ichar* rfdrbuf(void);

// get length of word
int wlenic(ichar* c);

// write to canvas
void wric(char c);
void bkic(void);

// ensures you don't write over the prompt
void prompt_cap(void);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SH_CHAR_H__