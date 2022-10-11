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

  this->set_fsize();
  this->match_sig();

  return out;

};

// ---   *   ---   *   ---
// true if signatures match

bool Bin::match_sig(void) {

  bool out=true;

  uint64_t sig_len=m_fsig().length();

  this->rewind();

  // skip check if no signature
  if(sig_len) {

// ---   *   ---   *   ---
// compare signatures if reading
// or file is not new

    if(

       m_mode&std::ios::in
    || !(m_mode&std::ios::trunc)

    ) {

      // read in first few bytes
      std::string sig(sig_len,'\0');
      m_fh.read(&sig[0],sig_len);

      // compare and throw
      if(m_fsig()!=sig) {

        Evil::terminator(
          Error::BAD_SIG,
          m_fpath

        );

        out=false;

      };

// ---   *   ---   *   ---
// paste signature if writting

    } else if(m_mode&std::ios::out) {

      uint64_t x=m_header_sz();
      std::string s(x,'\0');

      memcpy(&s[0],&m_fsig()[0],sig_len);
      m_fh.write(&s[0],x);

    };

// ---   *   ---   *   ---

  };

  return out;

};

// ---   *   ---   *   ---

void Bin::set_ptr(

  long to,

  std::ios_base::seekdir from,
  bool ignore_header

) {

  uint64_t skip=m_header_sz();

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
  m_size-=m_header_sz();

};

// ---   *   ---   *   ---
// constructor

Bin::Bin(std::string fpath,char mode) {
  this->open(fpath,mode);

};

// ---   *   ---   *   ---

Bin::~Bin(void) {

  if(m_fh.is_open()) {
    m_fh.close();

  };

};

// ---   *   ---   *   ---
// open, read, close

std::unique_ptr<uint8_t> Bin::orc(
  std::string fpath

) {

  Bin b(fpath,READ);

  std::unique_ptr<uint8_t> buff(
    new uint8_t[b.m_size]

  );

  b.m_fh.read((char*) buff.get(),b.m_size);

  return buff;

};

// ---   *   ---   *   ---
// read from cursor up to size

std::unique_ptr<uint8_t> Bin::read(
  uint64_t sz

) {

  sz+=m_size*(sz==0);

  std::unique_ptr<uint8_t> buff(
    new uint8_t[sz]

  );

  m_fh.read((char*) buff.get(),sz);
  m_ptr+=sz;

  return buff;

};

// ---   *   ---   *   ---
// ^buff provided by user

void Bin::read(void* dst,uint64_t sz) {

  sz+=m_size*(sz==0);

  m_fh.read((char*) dst,sz);
  m_ptr+=sz;

};

// ---   *   ---   *   ---
// gets header section

void Bin::read_header(void* buff) {

  // save current position
  uint64_t ptr=m_ptr;

  // go to real beg of file
  this->rewind();
  m_fh.read((char*) buff,m_header_sz());

  // return to old position
  this->seek(ptr,Bin::BEG,1);

};

// ---   *   ---   *   ---
// write size bytes from buff

void Bin::write(
  void* buff,uint64_t sz

) {

  sz+=m_size*(sz==0);
  m_fh.write((char*) buff,sz);

  m_ptr+=sz;

};

// ---   *   ---   *   ---
// overwrites header section

void Bin::write_header(void* buff) {

  // save current position
  uint64_t ptr=m_ptr;

  // copy signature
  memcpy(buff,&m_fsig()[0],m_fsig().length());

  // go to real beg of file
  this->rewind();
  m_fh.write((char*) buff,m_header_sz());

  // return to old position
  this->seek(ptr,Bin::BEG,1);

};

// ---   *   ---   *   ---
// moves cursor

void Bin::seek(

  long to,
  std::ios_base::seekdir from,

  bool ignore_header

) {

  this->set_ptr(to,from,ignore_header);

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
  this->seek(0,BEG,0);

};

// ---   *   ---   *   ---
// read from A, write to B

bool Bin::transfer(Bin* other,uint64_t sz) {

  sz+=m_size*(sz==0);

  std::unique_ptr<uint8_t> buff=
    this->read(sz);

  other->write((char*) buff.get(),sz);

  buff.reset();

  return true;

};

// ---   *   ---   *   ---
