// ---   *   ---   *   ---
// MEM
// to free or not to free
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <stdlib.h>
  #include <stddef.h>
  #include <stdint.h>
  #include <inttypes.h>
  #include <string.h>
  #include <stdio.h>

  #include "mem.h"

// ---   *   ---   *   ---
// alloc

Mem* Mem::nit(size_t sz,ID* id) {

  Mem*  out=NULL;
  void* buff=malloc(sz);

  if(buff!=NULL) {

    memset(buff,0,sz);

    out=(Mem*) buff;

    out->id=*id;
    out->fsize=sz;

  };

  return out;

};

// ---   *   ---   *   ---
// ^dealloc

void Mem::del(void* p) {

  if(p!=NULL) {
    free(p);

  };

};

// ---   *   ---   *   ---
// zero flood buffer

void Mem::cl(void) {
  memset(beg,0,bsize);

};

// ---   *   ---   *   ---
// subscript

void* Mem::operator[](int64_t idex) {

  size_t ptr=0;

// ---   *   ---   *   ---
// positive indexing

  if(idex >= 0) {

    if(idex < bsize) {
      ptr=idex;

    } else {
      ptr=bsize-1;

    };

  }

// ---   *   ---   *   ---
// negative indexing

  else {

    if(-idex <= bsize) {
      ptr=bsize+idex;

    } else {
      ptr=0;

    };

  };

// ---   *   ---   *   ---
// give buff+ptr

  return ((char*) beg) + ptr;

};

// ---   *   ---   *   ---

void Mem::prich(int errout) {

  const char* fmat=

    "%-24s %s\n"
    "\n"
    "%-24s %016X\n"
    "%-24s %016X\n"
    "\n"
    "%-24s %016X\n"
    "\n"

  ;

// ---   *   ---   *   ---
// select file handle and spit

  FILE* f=(errout) ? stderr : stdout;

  fprintf(f,fmat,

    "ID",id.as_str(),
    "FULL_SIZE",fsize,
    "BUFF_SIZE",bsize,
    "BUFF_ADDR",beg

  );

// ---   *   ---   *   ---

  char line[64];
  char* base=(char*) beg;

  for(size_t i=0;i<bsize;i+=16) {

    int* a=(int*) (base+i+0x00);
    int* b=(int*) (base+i+0x04);
    int* c=(int*) (base+i+0x08);
    int* d=(int*) (base+i+0x0C);

    fmat=(i && !(i%64))

      ? "\n  %08X %08X : %08X %08X %u\n"
      : "  %08X %08X : %08X %08X %u\n"
      ;

    fprintf(f,fmat,*a,*b,*c,*d,i);

  };

};

// ---   *   ---   *   ---
