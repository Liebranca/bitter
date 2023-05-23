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
  #include "kvrnel/Arstd.hpp"

  #include "ff/Zwrap.hpp"
  #include "ff/DAF.hpp"

// ---   *   ---   *   ---
// constructor

DAF::DAF(
  std::string fpath,
  char        mode

):Bin() {

  m_tab.nit(8);

  if(mode != Bin::NEW) {
    m_fpath=fpath;
    this->unzip();

  };

  this->defopen<DAF::Header>(fpath,mode);
  if(mode != Bin::NEW) {
    this->read_ftab();

  };

  this->seek(0,Bin::BEG);

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

      auto& prev = m_hed.blk[idex-1];
      blk.off    = prev.off+prev.sz;

    } else {
      blk.off=0;

    };

  };

};

// ---   *   ---   *   ---
// shifts block descriptors one step forward

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
// ^inverse

void DAF::blkshb(uint64_t start) {

  for(

    uint64_t idex=start;

    idex<m_hed.used-1;
    idex++

  ) {

    if((idex+1) >= DAFSIZE) {
      continue;

    };

    auto& blk  = m_hed.blk[idex];
    auto& next = m_hed.blk[idex+1];

    blk.off    = next.off;
    blk.sz     = next.sz;

  };

};

// ---   *   ---   *   ---
// get idex of first available slot

uint64_t DAF::get_next(void) {

  uint64_t  i     = 0;
  uint64_t* avail = this->get_avail();

  // throw full
  if(avail==NULL) {
    err(DAF::Error::FULL,m_fpath);

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

DAF::Block& DAF::jump_to_avail(void) {

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
// go to last entry

DAF::Block& DAF::jump_to_end(void) {

  auto& blk=m_hed.blk[m_hed.used-1];
  this->seek(blk.off,Bin::BEG);

  return blk;

};

// ---   *   ---   *   ---
// got to beg of ftab

void DAF::jump_to_ftab(void) {
  auto& blk=m_hed.blk[m_hed.used-1];
  this->seek(blk.off+blk.sz,Bin::BEG);

};

// ---   *   ---   *   ---
// saves portion of file to dump

void DAF::dump_tail(DAF::Block& blk) {

  Bin cpy(m_fpath,Bin::READ);
  Bin tmp(m_fpath+"_tail",Bin::NEW);

  this->stat();

  uint64_t sz=this->get_size()-blk.off;;

  if(sz) {
    cpy.seek(this->m_header_sz()+blk.off);
    cpy.transfer(tmp,sz);

  };

};

// ---   *   ---   *   ---
// ^put dumped tail back in file

void DAF::slap_tail(DAF::Block& blk) {

  Bin tmp(m_fpath+"_tail",Bin::READ);

  this->stat();
  this->trunc_to(blk.off);

  tmp.f_transfer(*this);
  tmp.unlink();

};

// ---   *   ---   *   ---
// drybeg of func

DAF::Block& DAF::insert_prelude(
  uint64_t idex

) {

  if(idex>=m_hed.used) {
    err(DAF::Error::OOB,m_fpath);

  };

  this->blkshf(idex);

  auto& blk=this->jump_to_idex(idex);
  this->dump_tail(blk);

  this->seek(blk.off,Bin::BEG);

  return blk;

};

// ---   *   ---   *   ---
// dryend of func

void DAF::insert_epilogue(
  DAF::Block& blk

) {

  this->slap_tail(blk);
  this->get_next();
  this->push_epilogue();

};

// ---   *   ---   *   ---
// remove last block

void DAF::pop(void) {

  if(!m_hed.used) {
    err(DAF::Error::EMPTY,m_fpath);

  };

  m_updated=true;

  auto& blk = this->jump_to_end();
  blk.sz    = 0;

  this->stat();
  this->trunc_to(blk.off);
  this->pop_ftab();

  m_hed.used--;

};

// ---   *   ---   *   ---

DAF::Block& DAF::replace_prelude(
  uint64_t idex

) {

  auto& next=this->jump_to_idex(idex+1);
  this->dump_tail(next);

  auto& blk=this->jump_to_idex(idex);

  return blk;

};

// ---   *   ---   *   ---
// remove specific block

void DAF::remove(uint64_t idex) {

  if(idex >= m_hed.used) {
    err(DAF::Error::OOB,m_fpath);

  };

  m_updated=true;

  // element in the middle
  if(idex!=m_hed.used-1) {

    auto& blk=this->replace_prelude(idex);
    blk.sz=0;

    this->slap_tail(blk);
    this->blktrav();
    this->blkshb(idex);
    this->remove_ftab(idex);

    m_hed.used--;

  // remove last element
  } else {
    this->pop();

  };

};

// ---   *   ---   *   ---
// change value of block

void DAF::replace(
  uint64_t idex,
  Bin&     src

) {

  if(idex>=m_hed.used) {
    err(DAF::Error::OOB,m_fpath);

  };

  m_updated=true;

  // element in the middle
  if(idex!=m_hed.used-1) {

    auto& blk=this->replace_prelude(idex);

    this->blk_from_file(src,blk);
    this->slap_tail(blk);
    this->blktrav();

  // last element
  } else {
    auto& blk=this->jump_to_end();
    this->blk_from_file(src,blk);
    this->blktrav();

  };

};

// ---   *   ---   *   ---
// ^from buff

void DAF::replace(

  uint64_t idex,

  void*    src,
  uint64_t sz

) {

  if(idex>=m_hed.used) {
    err(DAF::Error::OOB,m_fpath);

  };

  m_updated=true;

  // element in the middle
  if(idex!=m_hed.used-1) {

    auto& blk=this->replace_prelude(idex);

    this->blk_from_buff(src,sz,blk);
    this->slap_tail(blk);
    this->blktrav();

  // last element
  } else {
    auto& blk=this->jump_to_end();
    this->blk_from_buff(src,sz,blk);
    this->blktrav();

  };

};

// ---   *   ---   *   ---
// passes file through zlib

void DAF::zip(void) {

  Zwrap z(

    Zwrap::DEFLATE

  | Zwrap::INPUT_BIN
  | Zwrap::OUTPUT_BIN

  );

  Bin in(m_fpath,Bin::READ);
  Bin out(m_fpath+'z',Bin::NEW);

  z.set_src(&in);
  z.set_dst(&out);

  z.flate();

};

// ---   *   ---   *   ---
// ^undo

void DAF::unzip(void) {

  Zwrap z(

    Zwrap::INFLATE

  | Zwrap::INPUT_BIN
  | Zwrap::OUTPUT_BIN

  );

  Bin in(m_fpath+'z',Bin::READ);
  Bin out(m_fpath,Bin::NEW);

  z.set_src(&in);
  z.set_dst(&out);

  z.flate();

};

// ---   *   ---   *   ---
// save file table for later fetch

void DAF::write_ftab(void) {

  this->jump_to_ftab();

  for(auto& key : m_fnames) {
    this->write(key);

  };

};

// ---   *   ---   *   ---
// rebuilds file table

void DAF::read_ftab(void) {

  this->jump_to_ftab();

  uint64_t i=0;
  uint64_t j=0;

  auto     m=this->read();

  while(i < m.used()) {

    std::string key;
    while(m[i]) {key+=(char) m[i++];};

    i++;

    if(! key.length()) {break;};
    this->push_ftab(key,j++);

  };

};

// ---   *   ---   *   ---
// push new entry to ftab

void DAF::push_ftab(
  std::string key,
  uint64_t    idex

) {

  m_fnames.push_back(key);
  m_tab.push(key,idex);

};

// ---   *   ---   *   ---
// ^specific position

void DAF::insert_ftab(
  std::string key,
  uint64_t    idex

) {

  m_fnames.insert(m_fnames.begin()+idex,key);
  m_tab.push(key,idex);

};

// ---   *   ---   *   ---
// ^remove last

void DAF::pop_ftab(void) {

  auto& s=m_fnames.back();

  m_tab.pop(s);
  m_fnames.pop_back();

};

// ---   *   ---   *   ---
// ^remove specific

void DAF::remove_ftab(uint64_t idex) {

  auto& s=m_fnames[idex];

  m_tab.pop(s);
  m_fnames.erase(m_fnames.begin()+idex);

};

// ---   *   ---   *   ---
// gives blk idex of key

uint64_t DAF::ftab(std::string key) {

  auto lkp=m_tab.has(key);

  if(! lkp.key_match) {
    err(DAF::Error::NSF,key);

  };

  return m_tab.get(lkp);

};

// ---   *   ---   *   ---
// create bin from entry

void DAF::extract(

  std::string fname,
  std::string path,

  bool        clear

) {

  if(! path.length()) {
    path=Bin::getcwd();

  };

  path=path+'/'+fname;
  Bin out(path,Bin::NEW);

  if(clear) {
    m_cl_on_close.push_back(path);

  };

  auto& blk=this->jump_to_idex(
    this->ftab(fname)

  );

  this->transfer(out,blk.sz);

};

// ---   *   ---   *   ---
// ^whole archive

void DAF::unpack(
  std::string path,
  bool        clear

) {

  for(auto& fname : m_fnames) {
    this->extract(fname,path,clear);

  };

};

// ---   *   ---   *   ---
// close file

void DAF::close(void) {

  if(m_mode_ch & m_updated) {
    this->write_header(&m_hed);
    this->write_ftab();

    ((Bin*) this)->close();
    this->zip();

  };

  this->rmdump();

};

// ---   *   ---   *   ---
// clears the mess

void DAF::rmdump(void) {

  for(auto& path : m_cl_on_close) {
    Bin::unlink(path);

  };

  this->unlink();

};

// ---   *   ---   *   ---
// debug out

void DAF::prich(void) {

  for(auto& fname : m_fnames) {

    auto  idex = this->ftab(fname);
    auto& blk  = m_hed.blk[idex];

    printf("%s\n",fname.c_str());

  };

};

// ---   *   ---   *   ---
