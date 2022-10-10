#ifndef __24_BIN_H__
#define __24_BIN_H__

// ---   *   ---   *   ---
// deps

  #include <iostream>
  #include <fstream>
  #include <string>
  #include <memory>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Evil.hpp"

// ---   *   ---   *   ---
// info

class Bin {

public:

  VERSION   "2.00.1";
  AUTHOR    "IBN-3DILA";

  cx8       READ   = 0x01;
  cx8       WRITE  = 0x02;
  cx8       TRUNC  = 0x04;
  cx8       APPEND = 0x08;

  cx8       SLIMIT = 0x80;

// ---   *   ---   *   ---

  CX std::ios_base::seekdir BEG=
    std::ios_base::beg;

  CX std::ios_base::seekdir CUR=
    std::ios_base::cur;

  CX std::ios_base::seekdir END=
    std::ios_base::end;

// ---   *   ---   *   ---

private:

  cxstr     m_fsig="";
  cx64      m_header_sz=0;

  uint64_t  m_size;
  uint64_t  m_ptr;

  std::string m_fpath;

  std::ios_base::openmode m_mode;
  std::fstream m_fh;

// ---   *   ---   *   ---

  // reads mode flags
  void set_mode(char mode);

  // manipulates cursor
  void set_ptr(

    long to,

    std::ios_base::seekdir from=CUR,
    bool ignore_header=1

  );

  // stat current fpath
  void set_fsize(void);

// ---   *   ---   *   ---

public:

  // create object (implicit open)
  Bin(std::string fpath,char mode=READ);

  // destroy object (implicit close)
  ~Bin(void);

  // open file
  int open(
    std::string fpath,
    char mode=READ

  );

  // current file has correct signature
  bool match_sig(void);

  // open, read && close
  static std::unique_ptr<uint8_t> orc(
    std::string fpath

  );

  // read from cursor up to size
  std::unique_ptr<uint8_t> read(
    uint64_t sz=0

  );

  // ^same, you provide the buff
  void read(void* dst,uint64_t sz);

  // write from cursor
  void write(void* buff,uint64_t sz);

  // move bytes from file A to B
  bool transfer(Bin* other,uint64_t sz=0);

  // move cursor
  void seek(

    long to,
    std::ios_base::seekdir from=CUR

  );

  // seek to beggining
  inline void rewind(void);

// ---   *   ---   *   ---
// getters

  inline uint64_t tell(void) {
    return m_ptr;

  };

  inline uint64_t get_size(void) {
    return m_size;

  };

// ---   *   ---   *   ---

};

#endif // __24_BIN_H__
