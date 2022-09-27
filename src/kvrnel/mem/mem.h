#ifndef __KVR_MEM_H__
#define __KVR_MEM_H__

#include "../evil.h"
#include "../types/id.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---   *   ---   *   ---
// info

namespace Mem {

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

};

// ---   *   ---   *   ---
// generic holding dynamic memory

class Mem {

private:

  ID      id;     // universal block header

  size_t  fsize;  // total space used
  size_t  bsize;  // usable, non-header space

  void*   beg;    // start of non-header memory

// ---   *   ---   *   ---

public:

  // create a new block
  static Mem* nit(size_t sz,ID* id);

  // ^free
  void del(void* p);

  // get ptr to block @offset
  void* operator[](int64_t idex);

  // flood a block with zeroes
  void cl(void);

  // get type-casted ptr to block @offset
  template<T>
  T buff(offset=0) {return (T*) this[offset];};

// ---   *   ---   *   ---

  template<T>
  static T* morph(size_t sz,ID* id) {

    size_t req=(sz)+sizeof(T);

    static char szstr[32];
    int retx=0;

    __writoa(req,szstr,10);

    Mem* m=Mem::nit(req,id);

    if(m==NULL) {
      ERRCATCH(AR_FATAL,retx,0x00,szstr);

    };

    m->beg=((char*) m)+sizeof(T);
    m->bsize=m->fsize-sizeof(T);

    return (T*) m;

  };

};

// ---   *   ---   *   ---
// check struct is tight

CASSERT(

  sizeof(Mem)

  == (sizeof(ID))
  +  (sizeof(size_t)*2)
  +  (sizeof(void*)),

  "Bad Mem size"

);

// ---   *   ---   *   ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_MEM_H__
