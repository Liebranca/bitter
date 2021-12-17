#include "KVRNEL/zjc_CommonTypes.h"
#include <stdio.h>

//   ---     ---     ---     ---     ---

typedef struct {

  void* vp;

  char** cpp;
  char* cp;
  size_t szt;
  
  uchar uc;
  ushort us;
  uint ui;
  ulong ul;
  
  char c;
  short s;
  int i;
  long l;
  
  float* fp;
  float f;

} PACK;

//   ---     ---     ---     ---     ---

EXPORT void TYPE_TEST(int x,int y) {
  printf("%u\n",x+y);

};

EXPORT void TYPE_TEST2(char x,char** y) {
  printf("Hello!\n");

};

//   ---     ---     ---     ---     ---
