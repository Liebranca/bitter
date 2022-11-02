// ---   *   ---   *   ---
// ID
// papers, please
//
// LIBRE SOFTWARE
// Licenced under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <algorithm>
  #include "kvrnel/ID.hpp"

// ---   *   ---   *   ---
// constructor

ID::ID(

  const std::string sig,
  const std::string key

) {

  m_sig=sig;
  m_key=key;

};

// ---   *   ---   *   ---
// ^generic version

ID::ID(
  const std::string sig,
  uint64_t          n

) {

  m_sig=sig;

  char key[KEYW]={0};
  sprintf(key,"%016X",n);

  m_key=std::string(key);

};

// ---   *   ---   *   ---
