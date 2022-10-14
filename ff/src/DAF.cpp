// ---   *   ---   *   ---
// DAF
// Archiver of darkness
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "ff/DAF.hpp"

// ---   *   ---   *   ---
// constructor

DAF::DAF(
  std::string fpath,
  char mode=Bin::READ

):Bin() {

  this->open(fpath,mode);

  if(mode&Bin::READ) {
    this->read_header(&m_hed);

  };

  m_fpath=fpath;

};

// ---   *   ---   *   ---
// give next avail slot

uint64_t* DAF::get_avail(void) {

  uint64_t* avail;
  int i=0;

  while(

     m_hed.blk_mask[i]==-1
  && i<4

  ) {i++;};

  avail=(i<4)
    ? &m_hed.blk_mask[i]
    : NULL
    ;

  return avail;

};

inline uint64_t DAF::avail_idex(
  uint64_t* avail

) {
  return uint64_t(
    avail-&m_hed.blk_mask[0]

  ) >>3;

};

// ---   *   ---   *   ---
// insert new block

void DAF::push(Bin& src) {

  uint64_t* avail=this->get_avail();

  // throw full
  if(avail==NULL) {
    Evil::terminator(
      Fatal::ARCHIVE_FULL,
      m_fpath

    );

  };

// ---   *   ---   *   ---

  uint64_t i=0;

  // get next free slot
  if(*avail) {
    i=nbsf(*avail);

  };

  // occupy
  *avail|=1<<i;

  // get real idex of block
  uint64_t blk_idex=i+(
    this->avail_idex(avail)*64

  );

// ---   *   ---   *   ---

  DAF::Block* blk=&m_hed.blk[blk_idex];

  if(blk_idex) {
    DAF::Block* prev=&m_hed.blk[blk_idex-1];
    blk->off=prev->off+prev->sz;

  } else {
    blk->off=0;

  };

  blk->sz=src.get_fullsize();

  this->seek(blk->off,Bin::BEG);
  src->f_transfer(this);

};

// ---   *   ---   *   ---
