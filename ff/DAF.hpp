#ifndef __24_DAF_H__
#define __24_DAF_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"

// ---   *   ---   *   ---
// info

class DAF: public Bin {

public:

  VERSION   "v2.00.5";
  AUTHOR    "IBN-3DILA";

  struct Error {

    CX Evil::Errcode FULL={

      .type=AR_FATAL,

      .code=__COUNTER__,
      .mess="DAF archive is full"

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

private:

  cx16 BLK_CNT=DAFSIZE/64;

  // single entry
  typedef struct {

    uint64_t off;
    uint64_t sz;

  } Block;

  // ^whole header
  typedef struct {

    char       sig      [8];

    uint64_t   used=0;
    uint64_t   blk_mask [BLK_CNT];
    DAF::Block blk      [DAFSIZE];

  } Header;

// ---   *   ---   *   ---
// attrs

  Header m_hed;

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

  // saves portion of file to dump
  void dump_tail(DAF::Block& blk);

  // ^recovers
  void slap_tail(DAF::Block& blk);

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

  // remove last block
  void pop(void);

  // remove specific block
  void remove(uint64_t idex);

// ---   *   ---   *   ---
// put new block at end of file

  // src is file
  inline void push(Bin& src) {

    auto& blk=this->jump_to_avail();
    this->blk_from_file(src,blk);
    this->push_epilogue();

  };

  // src is buff
  inline void push(
    void*    src,
    uint64_t sz

  ) {

    auto& blk=this->jump_to_avail();
    this->blk_from_buff(src,sz,blk);
    this->push_epilogue();

  };

// ---   *   ---   *   ---
// put new block at position

  // src is file
  inline void insert(
    Bin&     src,
    uint64_t idex

  ) {

    auto& blk=this->insert_prelude(idex);
    this->blk_from_file(src,blk);
    this->insert_epilogue(blk);

  };

  // src is buff
  inline void insert(
    void*    src,
    uint64_t sz,

    uint64_t idex

  ) {

    auto& blk=this->insert_prelude(idex);
    this->blk_from_buff(src,sz,blk);
    this->insert_epilogue(blk);

  };

// ---   *   ---   *   ---
// change value of block

  // src is file
  void replace(
    Bin&     src,
    uint64_t idex

  );

  // src is buff
  void replace(
    void*    src,
    uint64_t sz,

    uint64_t idex

  );


};

// ---   *   ---   *   ---

#endif // __24_DAF_H__
