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

  #include <filesystem>

  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Bin.hpp"

// ---   *   ---   *   ---
// reads mode flags

void Bin::set_mode(char mode) {

  m_mode_ch=mode;
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

void Bin::open(std::string fpath,char mode) {

  this->set_mode(mode);
  m_fh.open(fpath,m_mode);

  // errchk
  if(

     ! m_fh.is_open()
  || ! m_fh.good()

  ) {

    err(Bin::Error::OPEN,fpath);

  // update filepath only on success
  } else {

    m_fpath=fpath;

    this->stat();
    this->match_sig();

  };

};

// ---   *   ---   *   ---
// ^letgo

void Bin::close(void) {

  if(!m_passed && m_fh.is_open()) {

    if(! m_fh.good()) {
      err(Bin::Error::CLOSE,m_fpath);

    };

    m_fh.close();

  };

};

// ---   *   ---   *   ---
// crash if signatures don't match

void Bin::match_sig(void) {

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
        err(Bin::Error::SIG,m_fpath);

      };

// ---   *   ---   *   ---
// paste signature if writting

    } else if(m_mode&std::ios::out) {

      uint64_t x=m_header_sz();
      std::string s(x,'\0');

      memcpy(&s[0],&m_fsig()[0],sig_len);
      m_fh.write(&s[0],x);

    };

  };

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

void Bin::stat(void) {

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
  this->close();

};

// ---   *   ---   *   ---
// open, read, close

Mem<uint8_t> Bin::orc(
  std::string fpath

) {

  Bin b(fpath,READ);
  Mem<uint8_t> buff(b.m_size);

  b.m_fh.read((char*) &buff[0],b.m_size);
  buff.set_used(b.m_size);

  return Mem<uint8_t>(buff);

};

// ---   *   ---   *   ---
// read from cursor up to size

Mem<uint8_t> Bin::read(
  uint64_t sz

) {

  sz+=m_size*(sz==0);
  this->rcap(sz);

  Mem<uint8_t> buff(sz);

  m_fh.read((char*) &buff[0],sz);
  m_ptr+=sz;

  buff.set_used(sz);

  return Mem<uint8_t>(buff);

};

// ---   *   ---   *   ---
// ^buff provided by user

void Bin::read(void* dst,uint64_t sz) {

  sz+=m_size*(sz==0);
  this->rcap(sz);

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
// read from A, write to B

void Bin::transfer(Bin& other,uint64_t sz) {

  sz+=m_size*(sz==0);
  auto buff=this->read(sz);

  other.write(&buff[0],buff.used());

};

void Bin::f_transfer(Bin& other) {

  this->rewind();

  uint64_t sz   = this->get_fullsize();
  auto     buff = this->read(sz);

  other.write(&buff[0],buff.used());

};

// ---   *   ---   *   ---
// shortens file

void Bin::trunc_to(uint64_t sz) {

  if(sz<m_size) {

    sz+=this->m_header_sz();

    std::filesystem::resize_file(
      m_fpath,sz

    );

  };

};

// ---   *   ---   *   ---
