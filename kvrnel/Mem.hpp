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

#ifndef __24_MEM_H__
#define __24_MEM_H__

// ---   *   ---   *   ---
// deps

  #include <memory>
  #include <string>
  #include <iostream>

  #include <cstdlib>
  #include <cstddef>
  #include <cstring>
  #include <cstdio>

  #include "kvrnel/ID.hpp"
  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Bytes.hpp"

// ---   *   ---   *   ---
// info

template<typename T>
class Mem {

public:

  VERSION   "v2.00.2";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// generic bytearray

private:

  ID       m_id;
  uint64_t m_slot;

  uint64_t m_buff_sz;
  uint64_t m_idex_mask;

  std::unique_ptr<T> m_buff;

// ---   *   ---   *   ---
// virtual consts

  vicstr m_sigil(void) {
    return "$MEM";

  };

// ---   *   ---   *   ---
// instance counter

  static std::vector<uint64_t> m_slstk;

  static uint64_t m_inst_next;
  static uint64_t m_inst_cnt;

  void new_slot(void) {

    uint64_t top=m_slstk.size();

    if(top) {
      m_slot=m_slstk[top-1];
      m_slstk.pop_back();

    } else {
      m_slot=m_inst_next++;

    };

    m_inst_cnt++;

  };

// ---   *   ---   *   ---

public:

  // compiler trash
  Mem<T>(void) {};

  // alloc
  Mem<T>(

    uint64_t buff_sz,
    uint64_t header_sz = 0x00,

    ID*      id        = NULL

  ) {

    this->new_slot();

    // default: generate ID for block
    if(id==NULL) {

      m_id=ID(

        std::string(this->m_sigil()),
        m_slot

      );

    // copy user-provided
    } else {
      m_id=*id;

    };

// ---   *   ---   *   ---
// get mem and zero initialize it

    m_buff_sz      = near_pow2(buff_sz);
    m_idex_mask    = m_buff_sz-1;

    T* raw         = (T*) malloc(this->bytesz());

    m_buff.reset(raw);
    this->cl();

  };

// ---   *   ---   *   ---
// copy

  Mem<T>(Mem<T>& other) {

    m_id         = other.m_id;

    m_slot       = other.m_slot;

    m_buff_sz    = other.m_buff_sz;
    m_idex_mask  = other.m_idex_mask;

    m_buff.reset(other.m_buff.release());

  };

// ---   *   ---   *   ---
// free

  ~Mem<T>(void) {

    if(m_buff.get()!=NULL) {
      m_slstk.push_back(m_slot);
      m_inst_cnt--;

      free(m_buff.release());

    };

  };

// ---   *   ---   *   ---
// flood a block with zeroes

  inline void cl(void) {
    memset(m_buff.get(),0,this->bytesz());

  };

// ---   *   ---   *   ---
// copy from buffer into mem

  uint64_t write(

    void*    src,

    uint64_t len=sizeof(T),
    uint64_t ptr=0

  ) {

    ptr&=m_idex_mask;

    uint64_t avail = m_buff_sz-ptr;
    bool     cap   = len>avail;

    len=(len*!cap)+(avail*cap);

    memcpy(
      m_buff.get()+ptr,
      src,len*sizeof(T)

    );

    return len;

  };

// ---   *   ---   *   ---

  inline uint64_t bytesz(void) {
    return m_buff_sz*sizeof(T);

  };

// ---   *   ---   *   ---
// subscript

  inline T& operator[](long idex) {

    uint64_t mask=-1*(idex<0);
    uint64_t base=m_buff_sz&mask;

    uint64_t ptr=((base+idex)&mask) | idex;

    ptr&=m_idex_mask;

    return *(m_buff.get()+ptr);

  };

// ---   *   ---   *   ---
// debug print

  void prich(int errout=0) {

    const char* fmat=

      "%-24s %s\n"
      "\n"
      "%-24s %016X\n"
      "%-24s %016X\n"
      "\n"
      "%-24s %016X\n"
      "%-24s %016X\n"
      "\n"

    ;

// ---   *   ---   *   ---
// select file handle and spit

    FILE* f=(errout) ? stderr : stdout;
    uint64_t real_size=this->bytesz();

    fprintf(f,fmat,

      "ID",m_id.as_str().c_str(),

      "FULL_SIZE",sizeof(*this)+real_size,
      "BUFF_SIZE",real_size,

      "BASE_ADDR",this,
      "BUFF_ADDR",m_buff.get()

    );

// ---   *   ---   *   ---
// print out the buffer

    char line[64];
    char* base=(char*) m_buff.get();

    for(

      uint64_t i=0;

      i<real_size;
      i+=0x10

    ) {

      int* a=(int*) (base+i+0x00);
      int* b=(int*) (base+i+0x04);
      int* c=(int*) (base+i+0x08);
      int* d=(int*) (base+i+0x0C);

      fmat="\n  "

        "%04X %04X %04X %04X . "
        "%04X %04X %04X %04X | "
        "%s\n"

      ;

      // conditionally skip newline
      fmat+=!(i && !(i%64));

// ---   *   ---   *   ---
// spit it out

      fprintf(

        f,fmat,

        *a&0xFFFF,(*a>>16)&0xFFFF,
        *b&0xFFFF,(*b>>16)&0xFFFF,

        *c&0xFFFF,(*c>>16)&0xFFFF,
        *d&0xFFFF,(*d>>16)&0xFFFF,

        stirr_p(a)

      );

    };

    printf("\n");

  };

// ---   *   ---   *   ---

};

// ---   *   ---   *   ---
// nit statics

template<typename T>
  std::vector<uint64_t> Mem<T>::m_slstk(0);

template<typename T>
  uint64_t Mem<T>::m_inst_next  = 0;

template<typename T>
  uint64_t Mem<T>::m_inst_cnt   = 0;

// ---   *   ---   *   ---
// instas

//template class Mem<int8_t>;
//template class Mem<int16_t>;
//template class Mem<int32_t>;
//template class Mem<int64_t>;
//
//template class Mem<uint8_t>;
//template class Mem<uint16_t>;
//template class Mem<uint32_t>;
//template class Mem<uint64_t>;

// ---   *   ---   *   ---

#endif // __24_MEM_H__
