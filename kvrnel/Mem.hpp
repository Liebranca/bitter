#ifndef __24_MEM_H__
#define __24_MEM_H__

// ---   *   ---   *   ---
// deps

  #include <memory>
  #include <string>

  #include "ID.hpp"

// ---   *   ---   *   ---
// info

template<typename T>
class Mem {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// generic holding dynamic memory

private:

  ID     m_id;     // universal block header

  size_t m_fsize;  // total space used
  size_t m_bsize;  // usable, non-header space

  void*  m_base;   // evil circular reference
  T*     m_beg;    // start of usable memory

// ---   *   ---   *   ---

public:

  // compiler trash
  Mem<T>() {};

  // alloc
  static Mem<T>* nit(

    size_t buff_sz,
    size_t header_sz,

    ID* id

  );

  // dealloc
  inline void del(void);

  // flood a block with zeroes
  inline void cl(void);

  // buffer write from string
  size_t write(std::string s,size_t ptr=0);

  // subscript
  inline T& operator[](long idex);

  // debug print
  void prich(int errout=0);

};

// ---   *   ---   *   ---

#endif // __24_MEM_H__
