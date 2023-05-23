#ifndef __24_DAF_H__
#define __24_DAF_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// info

class DAF: public Bin {

public:

  VERSION   "v2.01.0";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// errmes

  struct Error {

    CX Evil::Errcode FULL={

      .type=AR_FATAL,

      .code=__COUNTER__,
      .mess="DAF archive is full"

    };



    CX Evil::Errcode NSF={

      .type=AR_FATAL,

      .code=__COUNTER__,
      .mess="No such fname in archive"

    };

    CX Evil::Errcode OOB={

      .type=AR_FATAL,

      .code=__COUNTER__,

      .mess=

        "DAF insert, remove or replace "
        "out of bounds"

    };

    CX Evil::Errcode EMPTY={

      .type=AR_FATAL,

      .code=__COUNTER__,
      .mess="Pop from empty DAF"

    };

  };

// ---   *   ---   *   ---
// helpers

private:

  cx16 BLK_CNT=DAFSIZE/64;

  // single entry
  struct Block {

    uint64_t off;
    uint64_t sz;

  };

  // ^whole header
  struct Header {

    char       sig      [8];

    uint64_t   used=0;
    uint64_t   blk_mask [BLK_CNT];

    DAF::Block blk      [DAFSIZE];
    DAF::Block tab;

  };

// ---   *   ---   *   ---
// attrs

  Header m_hed;
  Itab   m_tab;

  strvec m_fnames;
  strvec m_cl_on_close;

  bool   m_updated=false;

// ---   *   ---   *   ---
// virtual const

  vicstr m_fsig(void) {
    return "%DAF\0\0\0\0";

  };

  vic64 m_header_sz(void) {
    return sizeof(Header);

  };

  VIC void* get_header(void) {
    return &m_hed;

  };

// ---   *   ---   *   ---
// guts

  // give next avail slot
  uint64_t* get_avail(void);

  // ^get idex of slot mask
  inline uint64_t avail_idex(
    uint64_t* avail

  ) {

    return uint64_t(
      avail-&m_hed.blk_mask[0]

    ) >>3;

  };

  // get idex of first available slot
  uint64_t get_next(void);

  // header sanity checks
  void blktrav(void);
  void blkshf(uint64_t end);
  void blkshb(uint64_t start);

  // positions file cursor on first avail block
  DAF::Block& jump_to_avail(void);

  // positions file cursor on specific block
  DAF::Block& jump_to_idex(uint64_t idex);

  // go to last entry
  DAF::Block& jump_to_end(void);

  // got to beg of ftab
  void jump_to_ftab(void);

  // saves portion of file to dump
  void dump_tail(DAF::Block& blk);

  // ^recovers
  void slap_tail(DAF::Block& blk);

  // passes file through zlib
  void zip(void);
  void unzip(void);

  // save file table for later fetch
  void write_ftab(void);

  // rebuilds file table
  void read_ftab(void);

  // ^push new entry to ftab
  void push_ftab(
    std::string key,
    uint64_t    idex

  );

  // ^specific position
  void insert_ftab(
    std::string key,
    uint64_t    idex

  );

  // ^remove last
  void pop_ftab(void);

  // ^remove specific
  void remove_ftab(uint64_t idex);

  // gives blk idex of key
  uint64_t ftab(std::string key);

  // clears tmp files
  void rmdump(void);

// ---   *   ---   *   ---
// dryguts

  inline void blk_from_file(
    Bin&        src,
    DAF::Block& blk

  ) {

    src.f_transfer(*this);
    blk.sz=src.get_fullsize();

  };

  inline void blk_from_buff(

    void*       src,
    uint64_t    sz,

    DAF::Block& blk

  ) {

    this->write(src,sz);
    blk.sz=sz;

  };

  inline void push_epilogue(void) {
    m_hed.used++;
    this->blktrav();

  };

  DAF::Block& insert_prelude(uint64_t idex);
  void insert_epilogue(DAF::Block& blk);

  DAF::Block& replace_prelude(uint64_t idex);

// ---   *   ---   *   ---
// iface

public:

  // ctrash
  DAF(void) {};
  ~DAF(void);

  // constructor
  DAF(std::string fpath,char mode=Bin::READ);

  // save changes; automatic on destroy
  void close(void);

// ---   *   ---   *   ---
// put new block at end of file

  // src is file
  inline void push(
    std::string key,
    Bin&        src

  ) {

    m_updated=true;

    auto& blk=this->jump_to_avail();
    this->blk_from_file(src,blk);
    this->push_ftab(key,m_hed.used);
    this->push_epilogue();

  };

  // src is buff
  inline void push(

    std::string key,

    void*       src,
    uint64_t    sz

  ) {

    m_updated=true;

    auto& blk=this->jump_to_avail();
    this->blk_from_buff(src,sz,blk);
    this->push_ftab(key,m_hed.used);
    this->push_epilogue();

  };

// ---   *   ---   *   ---
// put new block at position

  // src is file
  inline void insert(

    std::string key,

    Bin&        src,
    uint64_t    idex

  ) {

    m_updated=true;

    auto& blk=this->insert_prelude(idex);
    this->blk_from_file(src,blk);
    this->insert_ftab(key,idex);
    this->insert_epilogue(blk);

  };

  // src is buff
  inline void insert(

    std::string key,

    void*       src,
    uint64_t    sz,

    uint64_t    idex

  ) {

    m_updated=true;

    auto& blk=this->insert_prelude(idex);
    this->blk_from_buff(src,sz,blk);
    this->insert_ftab(key,idex);
    this->insert_epilogue(blk);

  };

// ---   *   ---   *   ---
// change value of block

  // src is file
  void replace(
    uint64_t key,
    Bin&     src

  );

  inline void replace(
    std::string key,
    Bin&        src

  ) {

    this->replace(this->ftab(key),src);

  };

  // src is buff
  void replace(

    uint64_t    idex,

    void*       src,
    uint64_t    sz

  );

  inline void replace(

    std::string key,

    void*       src,
    uint64_t    sz

  ) {

    this->replace(this->ftab(key),src,sz);

  };

// ---   *   ---   *   ---

  // remove last block
  void pop(void);

  // remove specific block
  void remove(uint64_t i);

  // ^lookup idex from key
  inline void remove(std::string key) {
    this->remove(this->ftab(key));

  };

  // create bin from entry
  void extract(

    std::string fname,

    std::string path  = "",
    bool        clear = false

  );

  // ^whole archive
  void unpack(
    std::string path  = "",
    bool        clear = false

  );

// ---   *   ---   *   ---
// conditional push/replace wrapper

  inline void cpush(
    std::string key,
    Bin&        src

  ) {

    auto lkp=m_tab.has(key);
    if(lkp.key_match) {
      this->replace(key,src);

    } else {
      this->push(key,src);

    };

  };

  // src is buff
  inline void cpush(

    std::string key,

    void*       src,
    uint64_t    sz

  ) {

    auto lkp=m_tab.has(key);
    if(lkp.key_match) {
      this->replace(key,src,sz);

    } else {
      this->push(key,src,sz);

    };

  };

// ---   *   ---   *   ---

  // debug out
  void prich(void);

  // getters
  inline strvec& get_fnames(void) {
    return m_fnames;

  };

};

// ---   *   ---   *   ---

#endif // __24_DAF_H__
