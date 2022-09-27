#ifndef __KVR_MEM_H__
#define __KVR_MEM_H__

  #include <stdlib.h>
  #include <stddef.h>
  #include <stdint.h>
  #include <string.h>

  #include <cmath>

//  #include "../evil.h"
  #include "../types/id.h"

// ---   *   ---   *   ---
// info

template<class H,typename T>
class Mem {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// generic holding dynamic memory

private:

  ID      m_id;     // universal block header

  size_t  m_fsize;  // total space used
  size_t  m_bsize;  // usable, non-header space

  void*   m_beg;    // start of non-header memory
  void*   m_pad;    // 16 aligns the struct

// ---   *   ---   *   ---

public:

  // alloc
  inline static Mem<H,T>* nit(size_t sz,ID* id);

  // ^free
  static void del(Mem<H,T>* m);

  // flood a block with zeroes
  inline void cl(void);

  // subscript
  inline T& operator[](int64_t idex);

  // debug print
  void prich(int errout);

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
