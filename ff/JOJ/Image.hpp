// ---   *   ---   *   ---
// paste-in buffer
// part of JOJ class

#ifndef __24_JOJ_IMAGE_H__
#define __24_JOJ_IMAGE_H__

// ---   *   ---   *   ---
// rgba ops boilerplate

template <typename T>
void img_cpy_yauv2rgba_pixel(

  T*     dst,
  float* src,

  float  mult

);

template <typename T>
void img_cpy_rgba2yauv_pixel(

  float* dst,
  T*     src,

  float  mult

);

template <typename T>
void img_cpy_yauv2rgba(

  T*       dst,
  float*   src,

  uint16_t sz,
  float    mult

);

template <typename T>
void img_cpy_rgba2yauv(

  float*   dst,
  T*       src,

  uint16_t sz,
  float    mult

);

// ---   *   ---   *   ---

#endif // __24_JOJ_IMAGE_H__
