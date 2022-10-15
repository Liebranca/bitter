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

  m_tab=Symtab(m_hed.elem_cnt);
  m_values.reserve(m_hed.elem_cnt);

};

// ---   *   ---   *   ---
// dump to disk

void PAL::write(void) {
  this->write_header(&m_hed);
  this->encode();

};

// ---   *   ---   *   ---
// pass raw file through zlib

void PAL::encode(void) {

  // calculate size
  uint64_t size_i=
    sizeof(uint64_t)*m_hed.elem_cnt;

  // get mem
  std::unique_ptr<uint8_t> buff(
    new uint8_t[size_i]

  );

  uint8_t* buff_p=buff.get();

  // ^deref pointers unto buff
  for(TAB::Symbol* sym : m_values) {

    *((uint64_t*) buff_p)=sym->value;
    buff_p+=sizeof(TAB::Symbol);

  };

  // initialize zlib
  Zwrap z(Zwrap::DEFLATE|Zwrap::OUTPUT_BIN);

  // set targets
  z.set_src(buff.get(),size_i);
  z.set_dst(this);

  // decompress
  z.flate();

};

// ---   *   ---   *   ---
// pass compressed file through zlib

void PAL::decode(void) {

  // initialize zlib
  Zwrap z(Zwrap::INFLATE|Zwrap::INPUT_BIN);

  // calculate size after decompression
  uint64_t size_i=
    sizeof(TAB::Symbol)*m_hed.elem_cnt;

  // get mem
  std::unique_ptr<uint8_t> buff(
    new uint8_t[size_i]

  );

  uint8_t* buff_p=buff.get();

  // set targets
  z.set_src(this);
  z.set_dst(buff_p,size_i);

  // decompress
  z.flate();

  // pass buffer to tab
  this->from_buff(buff_p);

};

// ---   *   ---   *   ---
// uses buffer to populate values

void PAL::from_buff(uint8_t* buff) {

  // assemble bytes from buff into vector
  for(uint64_t i=0;i<m_hed.elem_cnt;i++) {

    TAB::Symbol sym=*((TAB::Symbol*) buff);

    m_tab.push(sym.value,sym);
    m_values.push_back(&sym);

  };

};

// ---   *   ---   *   ---

void PAL::cpush(uint64_t key) {

  TAB::Symbol* sym=m_tab.cpush(key);

  if(sym->freq==1) {
    m_values.push_back(sym);

  };

};

// ---   *   ---   *   ---
