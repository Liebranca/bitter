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

  #include <cmath>
  #include <cstdlib>
  #include <cstddef>
  #include <cstring>
  #include <cstdio>

  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---
// force sz == pow 2

inline static size_t align_pow2(size_t x) {

  size_t y=log2(--x);y++;
  y=(y<4) ? 4 : y;

  return pow(2,y);

};

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

template<typename T>
Mem<T>* Mem<T>::nit(

  size_t  buff_sz,
  size_t  header_sz,

  ID*     id

) {

  buff_sz=align_pow2(buff_sz);
  size_t fsize=buff_sz+header_sz;

  Mem<T>* out=(Mem<T>*) malloc(fsize);

  // success
  if(out!=NULL) {

    memset(out,0,fsize);

    out->m_id=*id;
    out->m_fsize=fsize;

    char* raw=(char*) out;

    out->m_base=(void*) raw;

    out->m_beg=(T*) (raw+header_sz);
    out->m_bsize=buff_sz;

  // or die
  } else {

    Evil::terminator(
      Fatal::OOM,
      id->as_str()

    );

  };

  return out;

};

// ---   *   ---   *   ---
// ^dealloc

template<typename T>
void Mem<T>::del(void) {
  free(m_base);

};

// ---   *   ---   *   ---
// zero flood buffer

template<typename T>
inline void Mem<T>::cl(void) {
  memset(m_beg,0,m_bsize);

};

// ---   *   ---   *   ---
// buffer write from string

template<typename T>
size_t Mem<T>::write(

  std::string s,
  size_t ptr

) {

  size_t mask=m_bsize-1;

  ptr*=sizeof(T);
  ptr&=mask;

  memcpy(

    ((char*) m_beg)+ptr,

    s.c_str(),
    s.length()&mask

  );

  return 0;

};

// ---   *   ---   *   ---
// subscript

template<typename T>
inline T& Mem<T>::operator[](long idex) {

  size_t mask=-1*(idex<0);
  size_t base=m_bsize&mask;

  size_t ptr=((base+idex)&mask) | idex;

  ptr*=sizeof(T);
  ptr&=m_bsize-1;

  return (T&) *(((char*) m_beg) + ptr);

};

// ---   *   ---   *   ---
// debug print

template<typename T>
void Mem<T>::prich(int errout) {

  const char* fmat=

    "%-24s %s\n"
    "\n"
    "%-24s %016X\n"
    "%-24s %016X\n"
    "\n"
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

    "BUFF_ADDR",m_beg

  );

// ---   *   ---   *   ---
// print out the buffer

  char line[64];
  char* base=(char*) m_beg;

  for(size_t i=0;i<m_bsize;i+=16) {

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

      *a&0xFFFF,*a>>16,*b&0xFFFF,*b>>16,
      *c&0xFFFF,*c>>16,*d&0xFFFF,*d>>16,

      stirr_p(a)

    );

// ---   *   ---   *   ---

  };

};

// ---   *   ---   *   ---
