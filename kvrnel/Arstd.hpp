#ifndef __24_ARSTD_H__
#define __24_ARSTD_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// info

namespace Arstd {

  VERSION   "v0.00.1b";
  AUTHOR    "IBN-3DILA";

};

// ---   *   ---   *   ---
// string ops

strvec split(
  std::string src,
  std::string x=" "

);

std::string join(
  strvec&     tarr,
  std::string x=""

);

// ---   *   ---   *   ---
// path ops

std::string basef(std::string s);
std::string based(std::string s);
std::string nxbasef(std::string s);

std::string dirof(std::string s);
std::string parof(
  std::string s,
  uint64_t    depth=1

);

// ---   *   ---   *   ---

#endif // __24_ARSTD_H__
