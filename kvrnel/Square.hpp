#ifndef __24_SQUARE_H__
#define __24_SQUARE_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bytes.hpp"

  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---
// get index of point in rect from 2D coords

inline uint64_t sq_idex(

  uint16_t x,
  uint16_t y,

  uint64_t side

) {return x+(y*side);};

// ---   *   ---   *   ---
// ^get 2D coords from index

inline uint32_t rsq_idex(
  uint64_t i,
  uint64_t side

) {

  uint16_t n=bsf(side);

  uint16_t y=i>>n;
  uint16_t x=i-(y<<n);

  return x | (uint32_t(y)<<16);

};

// ---   *   ---   *   ---

static Mem<void*> sq_rows(
  void*    buff,

  uint64_t sz,
  uint64_t step

) {

  Mem<void*> out(sz);

  void**   out_p = &out[0];
  uint8_t* row   = (uint8_t*) buff;

  for(uint64_t i=0;i<sz;i++) {
    out_p[i]=row;
    row+=sz*step;

  };

  return Mem<void*>(out);

};

// ---   *   ---   *   ---

template <typename T>
struct AR1D {

  uint64_t cnt;
  std::vector <T> buff;

  T& operator[](uint64_t i) {
    return buff[i];

  };

  inline uint64_t size(void) {
    return buff.size();

  };

  inline T* data(void) {
    return buff.data();

  };

  inline void resize(uint64_t sz) {
    buff.resize(sz);

  };

  inline void clear(void) {
    buff.clear();

  };

  inline void push_back(T ref) {
    buff.push_back(ref);

  };

};

template <typename T>
struct AR2D {

  uint64_t cnt;
  std::vector<AR1D<T>> buff;

  std::vector<T>& operator[](uint64_t i) {
    return buff[i];

  };

  inline uint64_t size(void) {
    return buff.size();

  };

  inline std::vector<T>* data(void) {
    return buff.data();

  };

  inline void resize(uint64_t sz) {
    buff.resize(sz);

  };

  inline void clear(void) {
    buff.clear();

  };

  inline void push_back(std::vector<T>& ref) {
    buff.push_back(ref);

  };

};

// ---   *   ---   *   ---

#endif // __24_SQUARE_H__
