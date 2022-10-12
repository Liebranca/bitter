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
  #include "ff/Pal.hpp"

// ---   *   ---   *   ---
// constructor

Pal::Pal(
  std::string fpath,
  char mode=Bin::READ

):Bin() {

  this->open(fpath,mode);

  if(mode&Bin::READ) {
    this->read_header(&m_hed);
    this->decode();

  };

};

// ---   *   ---   *   ---
// dump to disk

void Pal::write(void) {
  this->write_header(&m_hed);
  this->encode();

};

// ---   *   ---   *   ---
// pass raw file through zlib

void Pal::encode(void) {

  // initialize zlib
  Zwrap z(Zwrap::DEFLATE|Zwrap::OUTPUT_BIN);

  // calculate size
  uint64_t size_i=m_hed.elem_sz*m_hed.elem_cnt;

  // get mem
  uint8_t* buff=(uint8_t*) m_values.data();

  // set targets
  z.set_src(buff,size_i);
  z.set_dst(this);

  // decompress
  z.flate();

};

// ---   *   ---   *   ---
// pass compressed file through zlib

void Pal::decode(void) {

  // initialize zlib
  Zwrap z(Zwrap::INFLATE|Zwrap::INPUT_BIN);

  // calculate size after decompression
  uint64_t size_i=m_hed.elem_sz*m_hed.elem_cnt;

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

  // pass buffer into vector
  this->from_buff(buff_p);

};

// ---   *   ---   *   ---
// uses buffer to populate values

void Pal::from_buff(uint8_t* buff) {

  // get mem
  m_values.reserve(m_hed.elem_cnt);

  // byte mask to elem size
  uint64_t mask=(1<<m_hed.elem_sz)-1;
  mask=(!mask) ? -1 : mask;

  // assemble bytes from buff into vector
  for(uint64_t i=0;i<m_hed.elem_cnt;i++) {
    m_values.push_back(*(

      (uint64_t*)
      (buff+i*m_hed.elem_sz)

    )&mask);

  };

};

// ---   *   ---   *   ---
