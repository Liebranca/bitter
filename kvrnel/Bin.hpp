#ifndef __24_BIN_H__
#define __24_BIN_H__

// ---   *   ---   *   ---
// deps

  #include <iostream>
  #include <fstream>
  #include <string>
  #include <cstring>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Mem.hpp"
  #include "kvrnel/Evil.hpp"

// ---   *   ---   *   ---
// info

class Bin {

public:

  VERSION   "2.00.2";
  AUTHOR    "IBN-3DILA";

  cx8       READ   = 0x01;
  cx8       WRITE  = 0x02;
  cx8       TRUNC  = 0x04;
  cx8       APPEND = 0x08;

  cx8       NEW    = WRITE|TRUNC;

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

  // override dummy
  uint64_t    m_hed;

// ---   *   ---   *   ---
// attrs

  uint64_t    m_size;
  uint64_t    m_ptr;

  bool        m_passed=false;

protected:
  std::string m_fpath;
  uint8_t     m_mode_ch;

private:
  std::ios_base::openmode m_mode;
  std::fstream m_fh;

// ---   *   ---   *   ---
// virtual constants

  vicstr m_fsig(void) {
    return "";

  };

  vic64 m_header_sz(void) {
    return 0;

  };

  VIC void* get_header(void) {
    return &m_hed;

  };

// ---   *   ---   *   ---

  // reads mode flags
  void set_mode(char mode);

  // manipulates cursor
  void set_ptr(

    long to,

    std::ios_base::seekdir from=CUR,
    bool ignore_header=1

  );

// ---   *   ---   *   ---

public:

  // compiler trash
  Bin(void) {};

  // create object (implicit open)
  Bin(std::string fpath,char mode=READ);

  // destroy object (implicit close)
  ~Bin(void);

  // open file
  int open(
    std::string fpath,
    char mode=READ

  );

  // ^let go of fd
  void close(void);

  // sync header with file
  void stat(void);

// ---   *   ---   *   ---

  template <typename T>
  void defopen(
    std::string fpath,
    char mode

  ) {

    T* h=(T*) this->get_header();
    memset(h,0,sizeof(T));

    if(

       !(mode&Bin::READ)
    && !(mode&Bin::TRUNC)

    ) {

      this->open(fpath,Bin::READ);
      this->read_header(h);
      this->close();

    };

    this->open(fpath,mode);

    if(mode&Bin::READ) {
      this->read_header(h);

    };

  };

// ---   *   ---   *   ---

  // current file has correct signature
  bool match_sig(void);

  // open, read && close
  static Mem<uint8_t> orc(
    std::string fpath

  );

  // read from cursor up to size
  Mem<uint8_t> read(
    uint64_t sz=0

  );

  // ^same, you provide the buff
  void read(void* dst,uint64_t sz=0);

  // write from cursor
  void write(void* buff,uint64_t sz);

  // get filebeg
  void read_header(void* buff);

  // overwrites filebeg
  void write_header(void* buff);

  // move bytes from file A to B
  void transfer(Bin& other,uint64_t sz=0);
  void f_transfer(Bin& other);

  // shortens file
  void trunc_to(uint64_t sz);

  // move cursor
  void seek(

    long to,
    std::ios_base::seekdir from=CUR,
    bool ignore_header=1

  );

  // seek to beggining
  inline void rewind(void);

  // remove file
  void nuke(void);

// ---   *   ---   *   ---
// getters

  inline uint64_t tell(void) {
    return m_ptr;

  };

  inline uint64_t get_size(void) {
    return m_size;

  };

  inline uint64_t get_fullsize(void) {
    return m_size+m_header_sz();

  };

  inline std::string get_fpath(void) {
    return m_fpath;

  };

// ---   *   ---   *   ---

};

#endif // __24_BIN_H__
