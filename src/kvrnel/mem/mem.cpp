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

  #include "mem.hpp"

// ---   *   ---   *   ---
// force sz == pow 2

inline static long align_pow2(long x) {

  long y=log2(--x);y++;
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

template<class H,typename T>
inline Mem<H,T>* Mem<H,T>::nit(

  long  sz,
  ID*   id

) {

  Mem<H,T>* out;
  sz=align_pow2(sz);

  long fsize=sz+sizeof(Mem<H,T>);

  // get block
  void* buff=malloc(fsize);

  // errchk done in poly
  if(buff!=NULL) {

    memset(buff,0,fsize);
    out=(Mem<H,T>*) buff;

    out->m_id=*id;
    out->m_fsize=fsize;

    out->m_beg=((char*) out)+sizeof(Mem<H,T>);
    out->m_bsize=sz;

  };

  return out;

};

// ---   *   ---   *   ---
// ^free

template<class H,typename T>
void Mem<H,T>::del(Mem<H,T>* m) {
  free(m);

};

// ---   *   ---   *   ---
// zero flood buffer

template<class H,typename T>
inline void Mem<H,T>::cl(void) {
  memset(m_beg,0,m_bsize);

};

// ---   *   ---   *   ---
// subscript

template<class H,typename T>
inline T& Mem<H,T>::operator[](long idex) {

  long mask=-1*(idex<0);
  long base=m_bsize&mask;

  long ptr=((base+idex)&mask) | idex;

  ptr*=sizeof(T);
  ptr&=m_bsize-1;

  return (T&) *(((char*) m_beg) + ptr);

};

// ---   *   ---   *   ---
// debug print

template<class H,typename T>
void Mem<H,T>::prich(int errout) {

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

  for(long i=0;i<m_bsize;i+=16) {

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
