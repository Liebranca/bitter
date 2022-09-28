#ifndef __24_MEM_H__
#define __24_MEM_H__

//  #include "../evil.h"
  #include "../types/id.hpp"

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

  long    m_fsize;  // total space used
  long    m_bsize;  // usable, non-header space

  void*   m_beg;    // start of non-header memory
  void*   m_pad;    // 16 aligns the struct

// ---   *   ---   *   ---

public:

  // alloc
  static Mem<H,T>* nit(long sz,ID* id);

  // ^free
  static void del(Mem<H,T>* m);

  // flood a block with zeroes
  inline void cl(void);

  // subscript
  inline T& operator[](long idex);

  // debug print
  void prich(int errout);

};

// ---   *   ---   *   ---

#endif // __24_MEM_H__
