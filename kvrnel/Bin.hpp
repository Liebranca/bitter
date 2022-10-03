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

  size_t    m_size;
  size_t    m_ptr;

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
  static std::unique_ptr<char[]> orc(
    std::string fpath

  );

  // read from cursor up to size
  std::unique_ptr<char[]> read(
    size_t sz=0

  );

  // write from cursor
  void write(char* buff,size_t sz);

  // move bytes from file A to B
  bool transfer(Bin* other,size_t sz=0);

  // move cursor
  void seek(

    long to,
    std::ios_base::seekdir from=CUR

  );

  // seek to beggining
  inline void rewind(void);

// ---   *   ---   *   ---
// getters

  inline size_t tell(void) {
    return m_ptr;

  };

// ---   *   ---   *   ---

};

#endif // __24_BIN_H__
