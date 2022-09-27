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

static Mem* Mem::nit(size_t sz,ID* id) {

  Mem*  out=NULL;
  void* buff=calloc(sz);

  if(buff!=NULL) {

    out=(Mem*) buff;

    out->id=*id;
    out->fsize=size;

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
  memset(m->beg,0,m->bsize);

};

// ---   *   ---   *   ---
// subscript

void* Mem::operator[](int64_t idex) {

  size_t ptr=0;

// ---   *   ---   *   ---
// positive indexing

  if(idex >= 0) {

    if(idex < m->bsize) {
      ptr=idex;

    } else {
      ptr=m->bsize-1;

    };

  }

// ---   *   ---   *   ---
// negative indexing

  else {

    if(-idex <= m->bsize) {
      ptr=m->bsize+idex;

    } else {
      ptr=0;

    };

  };

// ---   *   ---   *   ---
// give buff+ptr

  return ((char*) m->beg) + ptr;

};

// ---   *   ---   *   ---

