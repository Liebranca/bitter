#ifndef __24_MEM_H__
#define __24_MEM_H__

// ---   *   ---   *   ---
// deps

  #include <memory>
  #include <string>

  #include "ID.hpp"

// ---   *   ---   *   ---
// info

template<typename T,uint64_t SZ>
class Mem {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// generic bytearray

private:

  ID       m_id;      // universal block header

  uint64_t m_fsize;   // total space used
  uint64_t m_bsize;   // usable, non-header space

  void*    m_base;    // evil circular reference

  T        m_beg[SZ]; // start of usable memory

// ---   *   ---   *   ---

public:

  // compiler trash
  Mem<T,SZ>() {};

  // alloc
  Mem<T,SZ>(

    ID*    id,
    uint64_t header_sz=0x00

  );

  // flood a block with zeroes
  inline void cl(void);

  // buffer write from string
  uint64_t write(std::string s,uint64_t ptr=0);

  // subscript
  inline T& operator[](long idex);

  // debug print
  void prich(int errout=0);

};

// ---   *   ---   *   ---

#endif // __24_MEM_H__
