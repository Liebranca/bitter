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
// header sanity check

void DAF::blktrav(void) {

  for(

    uint64_t idex=0;

    idex<=m_hed.used;
    idex++

  ) {

    if(idex>=DAFSIZE) {
      break;

    };

    auto& blk=m_hed.blk[idex];

    if(idex) {

      auto& prev=m_hed.blk[idex-1];
      blk.off=prev.off+prev.sz;

    } else {
      blk.off=0;

    };

  };

};

// ---   *   ---   *   ---
// shifts block descriptors one step

void DAF::blkshf(uint64_t end) {

  for(

    uint64_t idex=m_hed.used;

    idex>end;
    idex--

  ) {

    if(idex >= DAFSIZE) {
      continue;

    };

    auto& blk  = m_hed.blk[idex];
    auto& prev = m_hed.blk[idex-1];

    blk.off    = prev.off;
    blk.sz     = prev.sz;

  };

};

// ---   *   ---   *   ---
// get idex of first available slot

uint64_t DAF::get_next(void) {

  uint64_t  i     = 0;
  uint64_t* avail = this->get_avail();

  // throw full
  if(avail==NULL) {
    evil_throw(DAF::Error::FULL,m_fpath);

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
// go to end of file

DAF::Block& DAF::jump_to_end(void) {

  auto  idex = this->get_next();
  auto& blk  = m_hed.blk[idex];

  this->seek(blk.off,Bin::BEG);

  return blk;

};

// ---   *   ---   *   ---
// go to specific entry

DAF::Block& DAF::jump_to_idex(uint64_t idex) {

  auto& blk=m_hed.blk[idex];
  this->seek(blk.off,Bin::BEG);

  return blk;

};

// ---   *   ---   *   ---
// saves portion of file to dump

void DAF::dump_tail(DAF::Block& blk) {

  this->seek(blk.off,Bin::BEG);

  DAF cpy(m_fpath,Bin::READ);
  Bin tmp(m_fpath+"_tail",Bin::NEW);

  this->stat();

  uint64_t sz=this->get_size()-blk.off;

  if(sz) {
    cpy.transfer(tmp,sz);

  };

};

// ---   *   ---   *   ---
// ^put dumped tail back in file

void DAF::slap_tail(DAF::Block& blk) {

  Bin tmp(m_fpath+"_tail",Bin::READ);

  tmp.f_transfer(*this);
  tmp.nuke();

  this->trunc_to(this->tell());

};

// ---   *   ---   *   ---
// put new block at position

void DAF::insert(
  Bin&     src,
  uint64_t idex

) {

  // TODO: pass this through evil
  if(idex>=m_hed.used) {
    evil_throw(DAF::Error::OOB,m_fpath);

  };

// ---   *   ---   *   ---

  this->blkshf(idex);

  auto& blk=this->jump_to_idex(idex);
  this->dump_tail(blk);

  // overwrite
  this->seek(blk.off,Bin::BEG);
  src.f_transfer(*this);

  blk.sz=src.get_fullsize();
  this->slap_tail(blk);

  this->get_next();

  m_hed.used++;
  this->blktrav();

};

// ---   *   ---   *   ---
// close file

void DAF::close(void) {

  if(m_mode_ch&Bin::WRITE) {
    this->write_header(&m_hed);

  };

  Bin::close();

};

// ---   *   ---   *   ---
