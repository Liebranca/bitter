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

  #include "ff/Zwrap.h"

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
    if(KVR_DEBUG) {\
      fprintf(stderr,"FAILED %s\n\b",#x);\
    };\
    Evil::terminator(\
      Fatal::ZLIB,\
      this->get_status()\
    );\
  }\
}

// ---   *   ---   *   ---

Zwrap::Zwrap(bool mode) {

  m_mode=mode;

  m_strm.zalloc   = Z_NULL;
  m_strm.zfree    = Z_NULL;
  m_strm.opaque   = Z_NULL;
  m_strm.next_in  = Z_NULL;
  m_strm.avail_in = 0;

  // get mem
  m_buff=std::unique_ptr<uint8_t>(
    new uint8_t[DAFPAGE*2]

  );

// ---   *   ---   *   ---

  if(m_mode==Zwrap::INFLATE) {

    CALL_ZLIB(inflateInit2(
      strm,windowBits|ENABLE_ZLIB_GZIP

    ));

// ---   *   ---   *   ---

  } else {

    CALL_ZLIB(deflateInit2(

      strm,Z_BEST_COMPRESSION,Z_DEFLATED,
      windowBits|GZIP_ENCODING,8,
      Z_DEFAULT_STRATEGY)

    );

  };

// ---   *   ---   *   ---

};

Zwrap::~Zwrap(void) {

  if(m_mode&Zwrap::INFLATE) {
    inflateEnd(&m_strm);

  } else {
    deflateEnd(&m_strm);

  };

};

// ---   *   ---   *   ---

inline void Zwrap::get_readsize(void) {

  m_readsize=(m_readsize > m_remain)
    ? m_remain
    : m_readsize
    ;

};

// ---   *   ---   *   ---
// buff read

uint8_t* next_chunk(uint8_t* src) {

  // cap chunk size
  get_readsize();

  // read next chunk from file
  m_strm.avail_in = m_readsize;
  m_strm.next_in  = src;

  m_remain-=m_readsize;

  return src;

};

// ---   *   ---   *   ---
// inflate buff2buff

void dump_i(uint8_t* dst,uint8_t* src) {

  while(

     m_strm.avail_in
  || !m_strm.avail_out

  ) {

    uint64_t have;

    m_strm.avail_out = m_readsize;
    m_strm.next_out  = src;

    CALL_ZLIB(inflate(
      &m_strm,Z_NO_FLUSH

    ));

    have=m_readsize-m_strm.avail_out;
    std::memcpy(dst,src,have);

  };

};

// ---   *   ---   *   ---
// configure target to mem or disk

inline void Zwrap::Target::set(
  Bin*      src,

  uint64_t  size,
  uint64_t  offset

) {

  this->size   = size;
  this->offset = offset;

  this->bin    = src;

  src->seek(offset,Bin::BEG);

};

inline void Zwrap::Target::set(
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

template <typename T>
inline void Zwrap::set_src<T>(
  T*       src,

  uint64_t size,
  uint64_t offset

) {m_src->set(src,size,offset);};

template <typename T>
inline void Zwrap::set_dst<T>(
  T*       dst,

  uint64_t size,
  uint64_t offset

) {m_dst->set(dst,size,offset);};

// ---   *   ---   *   ---
// decodes a deflated buff or bin

template <typename dst_t,typename src_t>
int Zwrap::inflate<dst_t,src_t>(void) {

  m_remain=m_src.size;

  while(m_remain) {

    this->dump_i(
      (dst_t*) m_dst,
      this->next_chunk((src_t*) m_src)

    );

  };

  return AR_DONE;

};

// ---   *   ---   *   ---

int Zwrap::deflate(void) {

  // fetch locs
  uint8_t* in  = m_buff.get();
  uint8_t* out = m_buff.get()+DAFPAGE;

  // rewind and skip
  ((Bin*) dst)->seek(m_offs_d,Bin::BEG);
  rewind(dst->file);

  uint64_t flush;

  m_remain=m_size_i;

// ---   *   ---   *   ---

  while(m_remain) {

    // cap chunk size
    uint64_t readsize = get_readsize(remain);

    m_strm.next_in    = in;
    m_strm.avail_in   = readsize;

    remain-=readsize;
    flush=(remain) ? Z_NO_FLUSH : Z_FINISH;

// ---   *   ---   *   ---

    while(
       m_strm.avail_in
    || !m_strm.avail_out

    ) {

      uint64_t have;

      m_strm.avail_out = DAFPAGE;
      m_strm.next_out  = out;

      CALL_ZLIB(deflate(&m_strm,flush));

      have=DAFPAGE-m_strm.avail_out;
      m_size_d+=have;

      fseek(dst->file,0,SEEK_CUR);
      BINWRITE(dst,wb,uchar,have,out);
      fseek(dst->file,0,SEEK_CUR);

    };

// ---   *   ---   *   ---

    in+=readsize;

  };

  return AR_DONE;

};

// ---   *   ---   *   ---
