#ifndef __24_VEC_H__
#define __24_VEC_H__

// ---   *   ---   *   ---
// deps

  #include "Style.hpp"

// ---   *   ---   *   ---

template <typename T,size_t S>
class Vec {

private:
  T m_values[S];

public:

  // constructor
  Vec(T x=0,T y=0,T z=0,T w=0);

  // subscript
  inline T& operator[](size_t idex);

  // by-name
  inline T& x(void);
  inline T& y(void);
  inline T& z(void);
  inline T& w(void);

  // debug print
  void prich(void);

};

// ---   *   ---   *   ---

#endif // __24_VEC_H__
