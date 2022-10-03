// ---   *   ---   *   ---
// TAB
// Also known as hash
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <functional>
  #include <string>

  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// series of compares

template <typename K,typename T>
inline bool Tab<K,T>::key_match(
  std::string& key,
  size_t idex

) {

  return

     m_keys[idex].length()
  && m_keys[idex]==key

  ;

};

// ---   *   ---   *   ---
// variations on obtaining a numerical
// representation of a key

void irep(
  std::string x,
  short* nkey

) {

  short limit=nkey[0]=x.length();

  for(short i=1;i<64;i++) {
    if(i<limit) {
      nkey[i]=x[i];

    } else {
      nkey[i]=1<<i;

    };

  };

};

// ---   *   ---   *   ---
// create idex from key

template <typename K,typename T>
size_t Tab<K,T>::hash(
  K& k

) {

  size_t idex = 0;
  short  x    = 0;
  short  y    = 0;

  short  nkey[64];
  short* nkey_p=&nkey[0];

  irep(k,nkey_p);

  // rehash teh hash
  for(int i=0;i<REAL_MULT;i++) {

    x=(*nkey_p++);
    x*=++x;

    idex^=x+y++;

  };

  idex*=REAL_MULT;
  idex%=m_size;

  return idex;

};

// ---   *   ---   *   ---
// constructor

template <typename K,typename T>
Tab<K,T>::Tab(
  size_t fake

) {

  // force a power of two to
  // ease up the bitter bitty
  fake   = near_pow2(fake);
  m_size = fake*REAL_MULT;

  // claim upfront
  m_keys.reserve(m_size);
  m_values.reserve(m_size);
  m_masks.reserve(fake);

  // cast the stones in their place
  for(size_t i=0;i<m_size;i++) {
    m_keys.push_back(K());
    m_values.push_back(T());

  };

  // speak of the devil
  for(size_t i=0;i<fake;i++) {
    m_masks.push_back(0x00);

  };

};

// ---   *   ---   *   ---
// destructor

template <typename K,typename T>
Tab<K,T>::~Tab(void) {};

// ---   *   ---   *   ---
// essentially a constructor :B

template <typename K,typename T>
Tab_Lookup Tab<K,T>::get_mask(K& k) {

  Tab_Lookup lkp={0};

  lkp.orig   = this->hash(k);

  lkp.fake   = lkp.orig/REAL_MULT;
  lkp.mask   = m_masks[lkp.fake];

  lkp.real   = lkp.orig;

  size_t cpy = lkp.mask;

// ---   *   ---   *   ---
// collide

  while(cpy) {
    lkp.key_match=key_match(k,lkp.real);
    if(lkp.key_match) {break;};

    lkp.real++;cpy=cpy>>1;

  };

  return lkp;

};

// ---   *   ---   *   ---
// find element

template <typename K,typename T>
inline T Tab<K,T>::get(
  K& k

) {

  Tab_Lookup lkp=get_mask(k);
  return m_values[lkp.real];

};

// ---   *   ---   *   ---
// set elem

template <typename K,typename T>
inline void Tab<K,T>::set(
  K& k,T v

) {

  Tab_Lookup lkp=get_mask(k);
  m_values[lkp.real]=v;

};

// ---   *   ---   *   ---
// remove element

template <typename K,typename T>
T Tab<K,T>::pop(
  K& k

) {

  T out=T();
  Tab_Lookup lkp=get_mask(k);

  out=m_values[lkp.real];

  size_t diff=(lkp.real-lkp.orig);
  m_masks[lkp.fake]^=1<<diff;

  return out;

};

// ---   *   ---   *   ---
// add element

template <typename K,typename T>
inline size_t Tab<K,T>::push(
  K k,T v

) {

  Tab_Lookup lkp=get_mask(k);
  if(lkp.key_match) {
    m_values[lkp.real]=v;
    goto TAIL;

  };

  // get next free slot
  if(lkp.mask) {
    size_t x=nbsf(lkp.mask);

    lkp.mask|=1<<x;
    lkp.real+=x;

  // get first
  } else {
    lkp.mask|=1;

  };

  m_masks[lkp.fake]=lkp.mask;
  m_values[lkp.real]=v;
  m_keys[lkp.real]=k;

TAIL:
  return lkp.real;

};

// ---   *   ---   *   ---
