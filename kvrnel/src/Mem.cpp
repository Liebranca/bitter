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

  #include <cstdlib>
  #include <cstddef>
  #include <cstring>
  #include <cstdio>

  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---

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
// alloc

template<typename T,uint64_t SZ>
Mem<T,SZ>::Mem(

  ID*     id,
  uint64_t  header_sz

) {

  uint64_t buff_sz=near_pow2(SZ);
  uint64_t fsize=buff_sz+header_sz;

  m_id=*id;
  m_fsize=fsize;

  m_base=(void*) this;
  m_bsize=buff_sz;

  this->cl();

};

// ---   *   ---   *   ---
// zero flood buffer

template<typename T,uint64_t SZ>
inline void Mem<T,SZ>::cl(void) {
  memset(&m_beg[0],0,m_bsize);

};

// ---   *   ---   *   ---
// buffer write from string

template<typename T,uint64_t SZ>
uint64_t Mem<T,SZ>::write(

  std::string s,
  uint64_t ptr

) {

  uint64_t mask=m_bsize-1;

  ptr*=sizeof(T);
  ptr&=mask;

  memcpy(

    ((char*) &m_beg[0])+ptr,

    s.c_str(),
    s.length()&mask

  );

  return 0;

};

// ---   *   ---   *   ---
// subscript

template<typename T,uint64_t SZ>
inline T& Mem<T,SZ>::operator[](long idex) {

  uint64_t mask=-1*(idex<0);
  uint64_t base=m_bsize&mask;

  uint64_t ptr=((base+idex)&mask) | idex;

  ptr*=sizeof(T);
  ptr&=m_bsize-1;

  return (T&) *(((char*) &m_beg[0]) + ptr);

};

// ---   *   ---   *   ---
// debug print

template<typename T,uint64_t SZ>
void Mem<T,SZ>::prich(int errout) {

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

    "ID",m_id.as_str(),

    "FULL_SIZE",m_fsize,
    "BUFF_SIZE",m_bsize,

    "BASE_ADDR",m_base,
    "BUFF_ADDR",&m_beg[0]

  );

// ---   *   ---   *   ---
// print out the buffer

  char line[64];
  char* base=(char*) &m_beg[0];

  for(uint64_t i=0;i<m_bsize;i+=16) {

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

// ---   *   ---   *   ---

  };

};

// ---   *   ---   *   ---
