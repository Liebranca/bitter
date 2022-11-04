// ---   *   ---   *   ---
// JOJ IMAGE
// Pixel-copying boilerpaste
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---

template <typename T>
void JOJ::img_cpy_yauv2rgba_pixel(

  T*     dst,
  float* src,

  float  mult

) {

  this->color(src,Frac::DECODE);

  dst[0]=src[0]*mult;
  dst[1]=src[1]*mult;
  dst[2]=src[2]*mult;
  dst[3]=src[3]*mult;

};

// ---   *   ---   *   ---

template <typename T>
void JOJ::img_cpy_rgba2yauv_pixel(

  float* dst,
  T*     src,

  float  mult

) {

  dst[0]=src[0]/mult;
  dst[1]=src[1]/mult;
  dst[2]=src[2]/mult;
  dst[3]=src[3]/mult;

  this->color(dst,Frac::ENCODE);

};

// ---   *   ---   *   ---

template <typename T>
void JOJ::img_cpy_yauv2rgba(

  T*       dst,
  float*   src,

  uint16_t sz,
  float    mult

) {

  uint64_t i=0;

  for(uint16_t y=0;y<sz;y++) {
    for(uint16_t x=0;x<sz;x++) {

      this->img_cpy_yauv2rgba_pixel(
        &dst[i],&src[i],mult

      );

      i+=4;

    };

  };

};

// ---   *   ---   *   ---

template <typename T>
void JOJ::img_cpy_rgba2yauv(

  float*   dst,
  T*       src,

  uint16_t sz,
  float    mult

) {

  uint64_t i=0;

  for(uint16_t y=0;y<sz;y++) {
    for(uint16_t x=0;x<sz;x++) {

      this->img_cpy_rgba2yauv_pixel(
        &dst[i],&src[i],mult

      );

      i+=4;

    };

  };

};

// ---   *   ---   *   ---
// instas

template void JOJ::img_cpy_yauv2rgba_pixel(
  float*,float*,float

);

template void JOJ::img_cpy_yauv2rgba(
  float*,float*,uint16_t,float

);

// ---   *   ---   *   ---

template void JOJ::img_cpy_rgba2yauv_pixel(
  float*,float*,float

);

template void JOJ::img_cpy_rgba2yauv(
  float*,float*,uint16_t,float

);

// ---   *   ---   *   ---

template void JOJ::img_cpy_rgba2yauv_pixel(
  float*,uint8_t*,float

);

template void JOJ::img_cpy_rgba2yauv(
  float*,uint8_t*,uint16_t,float

);

// ---   *   ---   *   ---

template void JOJ::img_cpy_yauv2rgba_pixel(
  uint8_t*,float*,float

);

template void JOJ::img_cpy_yauv2rgba(
  uint8_t*,float*,uint16_t,float

);

// ---   *   ---   *   ---
