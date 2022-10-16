// ---   *   ---   *   ---
// PAL
// For your buffer paintbrush
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "ff/Zwrap.hpp"
  #include "ff/PAL.hpp"

// ---   *   ---   *   ---
// constructor

PAL::PAL(
  std::string fpath,
  char mode,

  uint64_t sz

):Bin() {

  this->defopen<PAL::Header>(fpath,mode);

  if(sz) {
    m_hed.elem_cnt=sz;

  };

  if(mode&Bin::READ) {
    this->read();

  } else {
    m_tab=Symtab(m_hed.elem_cnt);

  };

};

// ---   *   ---   *   ---
// dump to disk

void PAL::write(void) {

  m_tab.sort();

  std::vector<uint64_t> buff=
    m_tab.sorted_buff();

  m_hed.elem_cnt=buff.size();

  this->write_header(&m_hed);

  Bin::write(
    buff.data(),
    m_hed.elem_cnt*sizeof(buff[0])

  );

};

// ---   *   ---   *   ---
// pass compressed file through zlib

void PAL::read(void) {

  uint64_t sz=
    sizeof(uint64_t)*m_hed.elem_cnt;

  m_values.reserve(m_hed.elem_cnt);

  this->seek(0,Bin::BEG);
  Bin::read(m_values.data(),sz);

};

// ---   *   ---   *   ---
