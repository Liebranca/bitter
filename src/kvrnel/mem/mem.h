#ifndef __KVR_MEM_H__
#define __KVR_MEM_H__

//#include "../evil.h"
#include "../types/id.h"

// ---   *   ---   *   ---
// info

class Mem {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// generic holding dynamic memory

private:

  ID      id;     // universal block header

  size_t  fsize;  // total space used
  size_t  bsize;  // usable, non-header space

  void*   beg;    // start of non-header memory
  void*   pad;    // 16 aligns the struct

// ---   *   ---   *   ---

public:

  // alloc
  static Mem* nit(size_t sz,ID* id);

  // ^free
  static void del(void* p);

  // get ptr to block @offset
  void* operator[](int64_t idex);

  // flood a block with zeroes
  void cl(void);

  // debug print
  void prich(int errout);

  // get type-casted ptr to block @offset
  template<class T>
  T buff(int64_t offset=0) {
    return (T) *(char*) ((*this)[offset]);

  };

// ---   *   ---   *   ---
// create a new block

  template<class T>
  static T poly(size_t sz,ID* id) {

    size_t req=(sz)+sizeof(T);

//    static char szstr[32];
//    int retx=0;
//
//    __writoa(req,szstr,10);

    Mem* m=nit(req,id);

//    if(m==NULL) {
//      ERRCATCH(AR_FATAL,retx,0x00,szstr);
//
//    };

    m->beg=((char*) m)+sizeof(T);
    m->bsize=m->fsize-sizeof(T);

    return (T) *m;

  };

};

// ---   *   ---   *   ---
// check struct is tight

//CASSERT(
//
//  sizeof(Mem)
//
//  == (sizeof(ID))
//  +  (sizeof(size_t)*2)
//  +  (sizeof(void*)),
//
//  "Bad Mem size"
//
//);

// ---   *   ---   *   ---

#endif // __KVR_MEM_H__
