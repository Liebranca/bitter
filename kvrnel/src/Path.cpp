// ---   *   ---   *   ---
// PATH
// Keeps oft-used dirs handy
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <cstdlib>
  #include <cstring>

  #include "Path.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---

Path::Def::Def(
  const char* name,
  char* add_to

) {

  this->redef(name);

};

// ---   *   ---   *   ---

void Path::Def::redef(
  const char* name,
  char* add_to

) {

  char* buff=std::getenv(name);
  m_value=buff;

};

inline const char* Path::Def::get(void) {
  return m_value;

};

// ---   *   ---   *   ---
