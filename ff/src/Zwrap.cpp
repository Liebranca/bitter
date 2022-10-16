// ---   *   ---   *   ---
// ZWRAP
// Wraps around zlib
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

  #include <cstring>
  #include <cstdio>
  #include <cassert>

// ---   *   ---   *   ---

std::string Zwrap::get_status(void) {

  switch (m_status) {

  case -1:
    return "Z_ERRNO";
  case -2:
    return "Z_STREAM_ERROR";
  case -3:
    return "Z_DATA_ERROR";
  case -4:
    return "Z_MEM_ERROR";
  case -5:
    return "Z_BUF_ERROR";
  case -6:
    return "Z_VERSION_ERROR";

  case 1:
    return "Z_STREAM_END";
  case 2:
    return "Z_NEED_DICT";

  default:
    return "Z_OK";

  };

};

// ---   *   ---   *   ---
// legacy errchk macro

#define CALL_ZLIB(x) {\
  m_status=x;\
  if (m_status < 0) {\
    Evil::terminator(\
      Fatal::ZLIB,\
      this->get_status()\
    );\
  }\
}

// ---   *   ---   *   ---
// constructor

Zwrap::Zwrap(int mode) {

  m_mode          = mode;
  m_buff          = NULL;
  m_readsize      = DAFPAGE;
  m_remain        = 0x00;

  m_strm          = {0};
  m_strm.zalloc   = Z_NULL;
  m_strm.zfree    = Z_NULL;
  m_strm.opaque   = Z_NULL;
  m_strm.next_in  = Z_NULL;
  m_strm.avail_in = 0;

  if(m_mode&Zwrap::INFLATE) {

    CALL_ZLIB(inflateInit2(
      &m_strm,

      WINDOW_BITS
    | ENABLE_ZLIB_GZIP

    ));

  } else {

    CALL_ZLIB(deflateInit2(

      &m_strm,

      Z_BEST_COMPRESSION,
      Z_DEFLATED,

      WINDOW_BITS
    | GZIP_ENCODING,

      8,
      Z_DEFAULT_STRATEGY

    ));

  };

};

// ---   *   ---   *   ---
// destructor

Zwrap::~Zwrap(void) {

  if(m_mode&Zwrap::INFLATE) {
    inflateEnd(&m_strm);

  } else {
    deflateEnd(&m_strm);

  };

};

// ---   *   ---   *   ---
// buff read

void Zwrap::next_chunk(void) {

  // cap chunk size
  this->get_readsize();

  // get chunk
  uint8_t* src;

  // from file
  if(m_mode&Zwrap::INPUT_BIN) {
    src=m_buff.get();
    m_src.bin->read(src,m_readsize);

  // from mem
  } else {
    src=m_src.bytes;
    m_src.bytes+=m_readsize;

  };

  // ^point to it
  m_strm.avail_in = m_readsize;
  m_strm.next_in  = src;

  m_remain-=m_readsize;

  // ^spit it out
  this->dump();

};

// ---   *   ---   *   ---
// ^invokes zlib on current chunk

uint64_t Zwrap::process(
  uint8_t* dst,
  uint64_t avail

) {

  uint64_t have    = 0;

  m_strm.avail_out = avail;
  m_strm.next_out  = dst;

  if(!avail) {goto TAIL;};

// ---   *   ---   *   ---

  if(m_mode&Zwrap::INFLATE) {
    CALL_ZLIB(inflate(
      &m_strm,m_flush

    ));

  } else {
    CALL_ZLIB(deflate(
      &m_strm,m_flush

    ));

  };

// ---   *   ---   *   ---

TAIL:

  have=avail-m_strm.avail_out;
  return have;

};

// ---   *   ---   *   ---
// inflate buff2buff

void Zwrap::dump(void) {

  if(!(m_mode&Zwrap::INFLATE)) {
    m_flush=(m_remain)
      ? Z_NO_FLUSH
      : Z_FINISH
      ;

  } else {m_flush=Z_NO_FLUSH;};

// ---   *   ---   *   ---

  while(

     m_strm.avail_in
  || !m_strm.avail_out

  ) {

    uint64_t have=0x00;

// ---   *   ---   *   ---
// from file

    if(m_mode&Zwrap::OUTPUT_BIN) {

      uint8_t* dst   = m_buff.get()+DAFPAGE;
      uint64_t avail = DAFPAGE;

      have=this->process(dst,avail);

      if(have) {
        m_dst.bin->write(dst,have);

      };

// ---   *   ---   *   ---
// from mem

    } else {

      have=this->process(m_dst.bytes,m_dst.size);

      m_dst.bytes+=have;
      m_dst.size-=have;

    };

// ---   *   ---   *   ---
// early exit

    if(!have || !m_remain) {break;};


  };

};

// ---   *   ---   *   ---
// configure target to mem or disk

void Zwrap::Target::set(
  Bin*      src,

  uint64_t  size,
  uint64_t  offset

) {

  this->size   = (!size)
    ? src->get_size()-offset
    : size
    ;

  this->offset = offset;
  this->bin    = src;

};

void Zwrap::Target::set(
  uint8_t*  src,

  uint64_t  size,
  uint64_t  offset

) {

  this->size   = size;
  this->offset = offset;

  this->bytes  = src+offset;

};

// ---   *   ---   *   ---
// ^single entry

void Zwrap::set_src(
  void*    src,

  uint64_t size,
  uint64_t offset

) {

  if(m_mode&Zwrap::INPUT_BIN) {
    m_src.set((Bin*) src,size,offset);

  } else {
    m_src.set((uint8_t*) src,size,offset);

  };

};

// ---   *   ---   *   ---
// ^clutter-copy

void Zwrap::set_dst(
  void*    dst,

  uint64_t size,
  uint64_t offset

) {

  if(m_mode&Zwrap::OUTPUT_BIN) {
    m_dst.set((Bin*) dst,size,offset);

  } else {
    m_dst.set((uint8_t*) dst,size,offset);

  };

};

// ---   *   ---   *   ---
// grows a compressed buff or bin
// shrinks a raw buffer or bin

int Zwrap::flate(void) {

  // get read/write buffer for bins
  if(

  (  (m_mode&Zwrap::INPUT_BIN)
  || (m_mode&Zwrap::OUTPUT_BIN)

  ) && m_buff==NULL

  ) {

    m_buff=std::unique_ptr<uint8_t>(
      new uint8_t[DAFPAGE*2]

    );

  };

// ---   *   ---   *   ---
// consume input

  m_remain=m_src.size;

  while(m_remain) {
    this->next_chunk();

  };

  return AR_DONE;

};

// ---   *   ---   *   ---
