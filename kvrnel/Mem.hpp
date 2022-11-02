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
// random helper

char* stirr_p(int* src,int cnt=4) {

  static char buff[64]={0};
  int i=0;

  while(i<cnt*sizeof(*src)) {

    char* ptr=(char*) src;

    for(int j=0;j<sizeof(*src);j++) {
      char c=*ptr;
      c=(c<0x20 || c>0x7E) ? 0x2E : c;

      buff[i++]=c;
      ptr++;

    };

    src++;

  };

  return buff;

};

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

  uint64_t get_new_slot(void) {

    uint64_t top=m_slstk.size();

    if(top) {
      m_slot=m_slstk[top-1];
      m_slstk.pop_back();

    } else {
      m_slot=m_inst_next++;

    };

    m_inst_cnt++;

    return m_slot;

  };

// ---   *   ---   *   ---

public:

  // compiler trash
  Mem<T>() {};

  // alloc
  Mem<T>(

    uint64_t buff_sz,
    uint64_t header_sz = 0x00,

    ID*      id        = NULL

  ) {

    // default: generate ID for block
    if(id==NULL) {

      m_id=ID(

        std::string(this->m_sigil()),
        this->get_new_slot()

      );

    // copy user-provided
    } else {
      m_id=*id;

    };

// ---   *   ---   *   ---
// get mem and zero initialize it

    m_buff_sz      = near_pow2(buff_sz);
    m_buff         = std::unique_ptr<T>(
      new T[m_buff_sz]

    );

    m_idex_mask    = m_buff_sz-1;

    this->cl();

  };

// ---   *   ---   *   ---
// free

  ~Mem<T>(void) {

    std::cerr

    << "Dealloc: "

    << m_id.as_str()
    << std::endl

    ;

    m_slstk.push_back(m_slot);
    m_buff.reset();

    m_inst_cnt--;

  };

// ---   *   ---   *   ---
// flood a block with zeroes

  inline void cl(void) {
    memset(m_buff.get(),0,m_buff_sz);

  };

// ---   *   ---   *   ---
// copy from buffer into mem

  uint64_t write(

    void*    src,

    uint64_t len=sizeof(T),
    uint64_t ptr=0

  ) {

    ptr&=m_idex_mask;
    len&=m_idex_mask;

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

    fprintf(f,fmat,

      "ID",m_id.as_str().c_str(),

      "FULL_SIZE",sizeof(*this)+m_buff_sz,
      "BUFF_SIZE",m_buff_sz,

      "BASE_ADDR",this,
      "BUFF_ADDR",m_buff.get()

    );

// ---   *   ---   *   ---
// print out the buffer

    char line[64];
    char* base=(char*) m_buff.get();

    for(uint64_t i=0;i<m_buff_sz;i+=16) {

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

#endif // __24_MEM_H__
