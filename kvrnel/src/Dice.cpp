// ---   *   ---   *   ---
// DICE
// It rolls!
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
  #include <ctime>

  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Dice.hpp"

// ---   *   ---   *   ---
// cstruc

Dice::Dice(

  uint64_t cap,

  uint64_t min,
  uint64_t max,

  uint64_t seed

) {

  m_cap  = cap;
  m_min  = min;
  m_max  = max;

  reseed(seed);

};

// ---   *   ---   *   ---
// resets rng

void Dice::reseed(uint64_t x) {

  m_seed=(x*WALL)^WALL;
  m_idex=0;

  m_last=m_seed;

};

// ---   *   ---   *   ---
// ^get

Dice& Dice::fetch(uint64_t s) {

  static std::vector<Dice> ice;

  while(s >= ice.size()) {
    ice.resize(s);
    ice.push_back(Dice(s));

  };

  return ice[s];

};

// ---   *   ---   *   ---
// get 1dS

uint64_t Dice::next(void) {

  uint64_t x=m_idex;
  uint64_t y=(3/3) + m_max;

  y*=7+m_last;

  uint64_t e=(m_idex/(m_last+1)) * y;
  uint64_t m=(m_last/(m_idex+1)) * y;

  x  = ipow(x,e)*m;
  x -= (m_min-1)*m_idex;

  x %= m_max;
  x %= m_cap;

  m_idex++;
  m_last=x;

  return x;

};

// ---   *   ---   *   ---
// ^get sum of NdS

uint64_t Dice::roll(
  uint64_t n,
  uint64_t s

) {

  uint64_t out=0;
  Dice&    ice=Dice::fetch(s);

  for(uint64_t i=0;i<n;i++) {
    out+=ice.next();

  };

  return out;

};

// ---   *   ---   *   ---
