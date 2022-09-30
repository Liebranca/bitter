// ---   *   ---   *   ---
// BIN
// Talks to your disk
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

#include <cstring>
#include <filesystem>

#include "kvrnel/Evil.hpp"
#include "kvrnel/Bin.hpp"

// ---   *   ---   *   ---
// reads mode flags

void Bin::set_mode(char mode) {

  m_mode^=m_mode;

  if(mode&READ) {
    m_mode|=std::ios::in;

  } else if(mode&WRITE) {
    m_mode|=std::ios::out;

  };

// ---   *   ---   *   ---

  if(mode&TRUNC) {
    m_mode|=std::ios::trunc;

  } else if(mode&APPEND) {
    m_mode|=std::ios::app;

  };

// ---   *   ---   *   ---
// we always open in binary mode
// read the name of the effn class

  m_mode|=std::ios::binary;

};

// ---   *   ---   *   ---
// opens a file

int Bin::open(std::string fpath,char mode) {

  int out=AR_DONE;

  this->set_mode(mode);
  m_fh.open(fpath,m_mode);

  // errchk
  if(!m_fh.is_open()) {

    Evil::terminator(
      Error::OPEN_FAIL,
      fpath.c_str()

    );

    out=AR_ERROR;

  // update filepath only on success
  } else {
    m_fpath=fpath;

  };

  return out;

};

// ---   *   ---   *   ---
// true if signatures match

bool Bin::match_sig(void) {

  bool out=true;
  size_t sig_len=m_fsig.length();

  this->rewind();

  // skip check if no signature
  if(sig_len) {

    // read in first few bytes
    std::string sig(sig_len,'\0');
    m_fh.read(&sig[0],SLIMIT);

    // compare and throw
    if(m_fsig!=sig) {

      Evil::terminator(
        Error::BAD_SIG,
        m_fpath

      );

      out=false;

    };

  };

  return out;

};

// ---   *   ---   *   ---

void Bin::set_ptr(

  long to,

  std::ios_base::seekdir from,
  bool ignore_header

) {

  size_t skip=m_fsig.length()+m_header_sz;
  skip*=ignore_header;

  // update ptr
  switch(from) {

  case BEG:
    m_ptr=skip+to;
    break;

  case CUR:
    m_ptr+=to;
    break;

  case END:
    m_ptr=(m_size+skip)-to;
    break;

  };

};

// ---   *   ---   *   ---
// gets filesize minus (sig+header)

void Bin::set_fsize(void) {

  m_size=std::filesystem::file_size(m_fpath);
  m_size-=(m_header_sz+m_fsig.length());

};

// ---   *   ---   *   ---
// constructor

Bin::Bin(std::string fpath,char mode) {

  this->open(fpath,mode);
  this->match_sig();
  this->set_fsize();

};

// ---   *   ---   *   ---

Bin::~Bin(void) {

  if(m_fh.is_open()) {
    m_fh.close();

  };

};

// ---   *   ---   *   ---
// open, read, close

std::unique_ptr<char[]> Bin::orc(
  std::string fpath

) {

  Bin b(fpath,READ);

  std::unique_ptr<char[]> buff(
    new char[b.m_size]

  );

  b.m_fh.read(buff.get(),b.m_size);

  return buff;

};

// ---   *   ---   *   ---
// read from cursor up to size

std::unique_ptr<char[]> Bin::read(
  size_t sz

) {

  sz+=m_size*(sz==0);

  std::unique_ptr<char[]> buff(
    new char[sz]

  );

  this->m_fh.read(buff.get(),sz);
  m_ptr+=sz;

  return buff;

};

// ---   *   ---   *   ---
// write size bytes from buff

void Bin::write(
  char* buff,size_t sz

) {

  sz+=m_size*(sz==0);
  this->m_fh.write(buff,sz);

  m_ptr+=sz;

};

// ---   *   ---   *   ---
// moves cursor

void Bin::seek(

  long to,
  std::ios_base::seekdir from

) {

  this->set_ptr(to,from,1);

  // ^seek to
  if(m_mode&std::ios::in) {
    m_fh.seekg(m_ptr);

  } else {
    m_fh.seekp(m_ptr);

  };

};

// ---   *   ---   *   ---
// seek to beg

inline void Bin::rewind(void) {
  this->set_ptr(0,BEG,0);

};

// ---   *   ---   *   ---
// read from A, write to B

bool Bin::transfer(Bin* other,size_t sz) {

  sz+=m_size*(sz==0);

  std::unique_ptr<char[]> buff=
    this->read(sz);

  other->write(buff.get(),sz);

  buff.reset();

  return true;

};

// ---   *   ---   *   ---
