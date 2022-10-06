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

  #include <string>

  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// variations on obtaining a numerical
// representation of a key

template <typename K,typename T>
void Tab<K,T>::irep(
  std::string& x,
  Tab_Hash* h

) {

  short limit=x.length()&(NKEY_SZ-1);

  for(h->y=0;h->y<limit;h->y++) {

    h->nkey[h->y]=x[h->y];
    this->hash_f(h);

  };

};

// ---   *   ---   *   ---
// ^straight number

template <typename K,typename T>
void Tab<K,T>::irep(
  size_t x,
  Tab_Hash* h

) {

  short limit=sizeof(x)&(NKEY_SZ-1);

  for(h->y=0;h->y<limit;h->y++) {

    h->nkey[h->y]=(x>>(h->y*8))&0xFF;
    this->hash_f(h);

  };

};

// ---   *   ---   *   ---

template <typename K,typename T>
inline void Tab<K,T>::hash_f(
  Tab_Hash* h

) {

  h->x=h->nkey[h->y];
  h->x*=++h->x;

  h->idex+=h->x+h->y;

};

// ---   *   ---   *   ---
// create idex from key

template <typename K,typename T>
size_t Tab<K,T>::hash(
  K& k

) {

  Tab_Hash h={0};
  irep(k,&h);

  h.idex=h.idex<<REAL_MULT_B;
  h.idex&=m_size-1;

  return h.idex;

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
  m_size = fake<<REAL_MULT_B;

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
  size_t f   = lkp.orig;

  lkp.fake   = lkp.orig>>REAL_MULT_B;
  lkp.mask   = m_masks[lkp.fake];

  lkp.real   = lkp.orig;

  size_t cpy = lkp.mask;
  bool   ful = lkp.mask!=0;

// ---   *   ---   *   ---
// collide

  lkp.key_match=false;

  while(cpy && !lkp.key_match) {
    lkp.key_match=m_keys[lkp.real]==k;

    lkp.real+=!lkp.key_match;
    cpy=cpy>>!lkp.key_match;

  };

  return lkp;

};

// ---   *   ---   *   ---
// find element

template <typename K,typename T>
inline T& Tab<K,T>::get(
  K& k

) {

  Tab_Lookup lkp=get_mask(k);
  return m_values[lkp.real];

};

// ^already hashed
template <typename K,typename T>
inline T& Tab<K,T>::get(
  Tab_Lookup& lkp

) {return m_values[lkp.real];};

// ---   *   ---   *   ---
// set elem

template <typename K,typename T>
inline void Tab<K,T>::set(
  K& k,T& v

) {

  Tab_Lookup lkp=get_mask(k);
  m_values[lkp.real]=v;

};

// ^already hashed
template <typename K,typename T>
inline void Tab<K,T>::set(
  Tab_Lookup& lkp,T& v

) {m_values[lkp.real]=v;};

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
  m_masks[lkp.fake]&=~(1<<diff);

  return out;

};

// ---   *   ---   *   ---
// get key in table

template <typename K,typename T>
inline Tab_Lookup Tab<K,T>::has(
  K& k

) {

  Tab_Lookup lkp=get_mask(k);
  return lkp;

};

// ---   *   ---   *   ---

template <typename K,typename T>
void Tab<K,T>::update_mask(
  Tab_Lookup& lkp

) {

  // get next free slot
  if(lkp.mask&1) {

    size_t x=nbsf(lkp.mask);
    lkp.mask|=1<<x;

  // get first
  } else {
    lkp.mask|=1;

  };

};

// ---   *   ---   *   ---

template <typename K,typename T>
inline void Tab<K,T>::update_entry(
  Tab_Lookup& lkp,
  K& k,T& v

) {

  m_masks[lkp.fake]=lkp.mask;
  m_values[lkp.real]=v;
  m_keys[lkp.real]=k;

};

// ---   *   ---   *   ---
// add element

template <typename K,typename T>
size_t Tab<K,T>::push(
  K& k,T& v

) {

  Tab_Lookup lkp=get_mask(k);

  if(lkp.key_match) {
    m_values[lkp.real]=v;

  } else {
    this->push(lkp,k,v);

  };

  return lkp.real;

};

// ---   *   ---   *   ---
// ^already hashed

template <typename K,typename T>
inline void Tab<K,T>::push(
  Tab_Lookup& lkp,
  K& k,T& v

) {

  this->update_mask(lkp);
  this->update_entry(lkp,k,v);

};

// ---   *   ---   *   ---
