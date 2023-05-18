// ---   *   ---   *   ---
// ARSTD
// Stuff I like brief
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Arstd.hpp"

// ---   *   ---   *   ---
// splits string at del

strvec split(
  std::string src,
  std::string x

) {

  strvec      out;
  std::string t;

  uint64_t    len = x.length();
  uint64_t    i   = 0;

  while((i=src.find(x)) != std::string::npos) {

    t=src.substr(0,i);

    if(! t.rfind(x,0)) {
      out.push_back(t);

    };

    src.erase(0,i+len);

  };

  if(src.length()) {
    out.push_back(src);

  };

  return out;

};

// ---   *   ---   *   ---
// cats vec of strings

std::string join(
  strvec&     tarr,
  std::string x

) {

  std::string out;

  for(auto& t : tarr) {
    x=(t==tarr.back()) ? "" : t;
    out+=t+x;

  };

  return out;

};

// ---   *   ---   *   ---
// filename without dir

std::string basef(std::string s) {
  auto tarr=split(s,"/");
  return tarr.back();

};

// ---   *   ---   *   ---
// last directory name in path

std::string based(std::string s) {

  auto tarr=split(s,"/");

  if(tarr.size() > 1) {
    tarr.pop_back();

  };

  return tarr.back();

};

// ---   *   ---   *   ---
// basef without extension

std::string nxbasef(std::string s) {

  s=basef(s);

  auto tarr=split(s,".");
  if(tarr.size() > 1) {
    tarr.pop_back();

  };

  return join(tarr,".");

};

// ---   *   ---   *   ---
// path without file

std::string dirof(std::string s) {
  auto tarr=split(s,"/");
  tarr.pop_back();

  return join(tarr,"/")+"/";

};

// ---   *   ---   *   ---
// Nth parent directory

std::string parof(
  std::string s,
  uint8_t     depth

) {

  auto tarr=split(s,"/");

  while(depth-- && tarr.size() > 1) {
    tarr.pop_back();

  };

  return join(tarr,"/")+"/";

};

// ---   *   ---   *   ---
