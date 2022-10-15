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

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

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
// internals

  uint64_t* get_avail(void);

  inline uint64_t avail_idex(
    uint64_t* avail

  );

// ---   *   ---   *   ---

public:

  // compiler trash
  DAF(void) {};
  ~DAF(void);

  // constructor
  DAF(std::string fpath,char mode=Bin::READ);

  // add new element
  void push(Bin& src);

  void close(void);

};

// ---   *   ---   *   ---

#endif // __24_DAF_H__
