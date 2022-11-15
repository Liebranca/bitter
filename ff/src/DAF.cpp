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

  #include "kvrnel/Bytes.hpp"
  #include "ff/DAF.hpp"

// ---   *   ---   *   ---
// constructor

DAF::DAF(
  std::string fpath,
  char mode

):Bin() {

  this->defopen<DAF::Header>(fpath,mode);

};

// ---   *   ---   *   ---
// ^destroy

DAF::~DAF(void) {
  this->close();

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

// ---   *   ---   *   ---
// get block by idex

DAF::Block* DAF::get_blk(uint64_t idex) {

  DAF::Block* blk=&m_hed.blk[idex];

  if(idex) {
    DAF::Block* prev=&m_hed.blk[idex-1];
    blk->off=prev->off+prev->sz;

  } else {
    blk->off=0;

  };

  return blk;

};

// ---   *   ---   *   ---
// get idex of first available slot

uint64_t DAF::get_next(void) {

  uint64_t  i     = 0;
  uint64_t* avail = this->get_avail();

  // throw full
  if(avail==NULL) {
    Evil::terminator(
      Fatal::ARCHIVE_FULL,
      m_fpath

    );

  };

  // get next free slot
  if(*avail) {
    i=nbsf(*avail);

  };

  // occupy
  *avail|=1<<i;

  // get real idex of block
  i+=this->avail_idex(avail)*64;

  return i;

};

// ---   *   ---   *   ---
// insert new block

void DAF::push(Bin& src) {

  auto idex = this->get_next();
  auto blk  = this->get_blk(idex);

  blk->sz   = src.get_fullsize();

  this->seek(blk->off,Bin::BEG);
  src.f_transfer(this);

};

// ---   *   ---   *   ---
// close file

void DAF::close(void) {
  this->write_header(&m_hed);
  Bin::close();

};

// ---   *   ---   *   ---
