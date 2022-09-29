// ---   *   ---   *   ---
// VEC
// Next-gen type-punning
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "Vec.hpp"
  #include <iostream>

// ---   *   ---   *   ---
// constructor

template <typename T,size_t S>
Vec<T,S>::Vec(T x,T y,T z,T w) {

  T values[4]={x,y,z,w};

  for(size_t i=0;i<S;i++) {
    m_values[i]=values[i];

  };

};

// ---   *   ---   *   ---
// subscript

template <typename T,size_t S>
inline T& Vec<T,S>::operator[](size_t idex) {
  return m_values[idex];

};


// ---   *   ---   *   ---
// by-name

template <typename T,size_t S>
inline T& Vec<T,S>::x(void) {
  return m_values[0];

};

template <typename T,size_t S>
inline T& Vec<T,S>::y(void) {
  return m_values[1];

};

template <typename T,size_t S>
inline T& Vec<T,S>::z(void) {
  return m_values[2];

};

template <typename T,size_t S>
inline T& Vec<T,S>::w(void) {
  return m_values[3];

};

// ---   *   ---   *   ---
// debug

template <typename T,size_t S>
void Vec<T,S>::prich(void) {

  std::cout << '[';

  for(size_t i=0;i<S;i++) {

    char e=(i==S-1) ? '\0' : ',';
    std::cout << m_values[i] << e;

  };

  std::cout << ']' << std::endl;

};

// ---   *   ---   *   ---
