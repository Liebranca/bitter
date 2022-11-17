// ---   *   ---   *   ---
// PNG WRAPPER
// because libpng is so ffin
// inconvenient to work with
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Square.hpp"
  #include "ff/PNG_Wrap.hpp"

// ---   *   ---   *   ---
// read nit

PNG::PNG(std::string fpath) {

  m_fpath    = fpath;
  m_fp       = fopen(m_fpath.c_str(),"rb");

  m_rd       = png_create_read_struct(
    PNG_LIBPNG_VER_STRING,NULL,NULL,NULL

  );

  m_info     = png_create_info_struct(m_rd);

  png_init_io(m_rd,m_fp);
  png_read_info(m_rd,m_info);

  m_sz       = png_get_image_width(m_rd,m_info);
  uint16_t h = png_get_image_height(m_rd,m_info);

  if(

     m_sz!=near_pow2(m_sz)
  || m_sz!=h

  ) {

    err(PNG::Error::SIZE,fpath);

  };

  m_sz_sq    = m_sz*m_sz;
  m_mode     = false;

};

// ---   *   ---   *   ---
// write nit

PNG::PNG(std::string fpath,uint16_t sz) {

  if(sz!=near_pow2(sz)) {
    err(PNG::Error::SIZE,fpath);

  };

  m_fpath    = fpath;
  m_fp       = fopen(m_fpath.c_str(),"wb+");

  m_wt       = png_create_write_struct(
    PNG_LIBPNG_VER_STRING,NULL,NULL,NULL

  );

  m_info     = png_create_info_struct(m_wt);

  png_init_io(m_wt,m_fp);

  m_sz       = sz;
  m_sz_sq    = m_sz*m_sz;
  m_mode     = true;

};

// ---   *   ---   *   ---
// del

PNG::~PNG(void) {

  if(!m_mode) {
    png_destroy_read_struct(&m_rd,&m_info,NULL);

  } else {
    png_destroy_write_struct(&m_wt,&m_info);

  };

  fclose(m_fp);

};

// ---   *   ---   *   ---
// io

Mem<uint8_t> PNG::read(void) {

  // nit copy if file open for writing
  if(m_mode) {
    PNG im(m_fpath);
    return im.read();

  };

// ---   *   ---   *   ---

  Mem<uint8_t> out(m_sz_sq<<2);

  auto rows=sq_rows(
    &out[0],m_sz<<2,1

  );

  png_read_image(m_rd,(uint8_t**) &rows[0]);

  return Mem<uint8_t>(out);

};

// ---   *   ---   *   ---

void PNG::write(Mem<uint8_t>& src) {

  // nit copy if file open for reading
  if(!m_mode) {
    PNG im(m_fpath,m_sz);
    im.write(src);

    return;

  };

  if(src.bytesz()!=m_sz_sq<<2) {
    err(PNG::Error::WRITE,m_fpath);

  };

// ---   *   ---   *   ---

  png_set_IHDR(
    m_wt,
    m_info,

    m_sz,m_sz,

    8,

    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT

  );

  png_write_info(m_wt,m_info);

  auto rows=sq_rows(
    &src[0],m_sz<<2,1

  );

  png_write_image(m_wt,(uint8_t**) &rows[0]);
  png_write_end(m_wt,NULL);

};

// ---   *   ---   *   ---
