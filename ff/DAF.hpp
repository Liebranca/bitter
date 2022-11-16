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

  VERSION   "v2.00.4";
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
      .mess="DAF insert out of bounds"

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

  // positions file cursor on first avail block
  DAF::Block& jump_to_end(void);

  // positions file cursor on specific block
  DAF::Block& jump_to_idex(uint64_t idex);

  // saves portion of file to dump
  void dump_tail(DAF::Block& blk);

  // ^recovers
  void slap_tail(DAF::Block& blk);

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
  inline void push(Bin& src) {

    auto& blk = this->jump_to_end();
    blk.sz    = src.get_fullsize();

    src.f_transfer(*this);

    m_hed.used++;
    this->blktrav();

  };

  // src is buff
  inline void push(
    void*    src,
    uint64_t sz

  ) {

    auto& blk = this->jump_to_end();
    blk.sz    = sz;

    this->write(src,sz);

    m_hed.used++;
    this->blktrav();

  };

// ---   *   ---   *   ---
// put new block at position

  void insert(
    Bin&     src,
    uint64_t idex

  );

};

// ---   *   ---   *   ---

#endif // __24_DAF_H__
