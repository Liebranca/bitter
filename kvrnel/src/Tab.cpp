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
  #include "kvrnel/Bin.hpp"

  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// variations on obtaining a numerical
// representation of a key

template <typename K,typename T>
void Tab<K,T>::irep(
  std::string& x,
  TAB::Hash* h

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
  uint64_t x,
  TAB::Hash* h

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
  TAB::Hash* h

) {

  h->x=h->nkey[h->y];
  h->x*=++h->x;

  h->idex+=h->x+h->y;

};

// ---   *   ---   *   ---
// create idex from key

template <typename K,typename T>
uint64_t Tab<K,T>::hash(
  K& k

) {

  TAB::Hash h={0};
  irep(k,&h);

  h.idex=h.idex<<REAL_MULT_B;
  h.idex&=m_size-1;

  return h.idex;

};

// ---   *   ---   *   ---
// constructor

template <typename K,typename T>
Tab<K,T>::Tab(
  uint64_t fake

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
  for(uint64_t i=0;i<m_size;i++) {
    m_keys.push_back(K());
    m_values.push_back(T());

  };

  // speak of the devil
  for(uint64_t i=0;i<fake;i++) {
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
TAB::Lookup Tab<K,T>::get_mask(K& k) {

  TAB::Lookup lkp={0};

  lkp.orig     = this->hash(k);
  uint64_t f   = lkp.orig;

  lkp.fake     = lkp.orig>>REAL_MULT_B;
  lkp.mask     = m_masks[lkp.fake];

  lkp.real     = lkp.orig;

  uint64_t cpy = lkp.mask;
  bool     ful = lkp.mask!=0;

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

  TAB::Lookup lkp=get_mask(k);
  return m_values[lkp.real];

};

// ^already hashed
template <typename K,typename T>
inline T& Tab<K,T>::get(
  TAB::Lookup& lkp

) {return m_values[lkp.real];};

// ---   *   ---   *   ---
// set elem

template <typename K,typename T>
inline void Tab<K,T>::set(
  K& k,T& v

) {

  TAB::Lookup lkp=get_mask(k);
  m_values[lkp.real]=v;

};

// ^already hashed
template <typename K,typename T>
inline void Tab<K,T>::set(
  TAB::Lookup& lkp,T& v

) {m_values[lkp.real]=v;};

// ---   *   ---   *   ---
// remove element

template <typename K,typename T>
T Tab<K,T>::pop(
  K& k

) {

  T out=T();
  TAB::Lookup lkp=get_mask(k);

  out=m_values[lkp.real];

  uint64_t diff=(lkp.real-lkp.orig);
  m_masks[lkp.fake]&=~(1<<diff);

  return out;

};

// ---   *   ---   *   ---
// get key in table

template <typename K,typename T>
inline TAB::Lookup Tab<K,T>::has(
  K& k

) {

  TAB::Lookup lkp=get_mask(k);
  return lkp;

};

// ---   *   ---   *   ---

template <typename K,typename T>
void Tab<K,T>::update_mask(
  TAB::Lookup& lkp

) {

  // get next free slot
  if(lkp.mask&1) {

    uint64_t x=nbsf(lkp.mask);
    lkp.mask|=1<<x;

  // get first
  } else {
    lkp.mask|=1;

  };

};

// ---   *   ---   *   ---

template <typename K,typename T>
inline void Tab<K,T>::update_entry(
  TAB::Lookup& lkp,
  K& k,T& v

) {

  m_masks[lkp.fake]=lkp.mask;
  m_values[lkp.real]=v;
  m_keys[lkp.real]=k;

};

// ---   *   ---   *   ---
// add element

template <typename K,typename T>
uint64_t Tab<K,T>::push(
  K& k,T& v

) {

  TAB::Lookup lkp=get_mask(k);

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
  TAB::Lookup& lkp,
  K& k,T& v

) {

  this->update_mask(lkp);
  this->update_entry(lkp,k,v);

};

// ---   *   ---   *   ---
// sort symbols by frequency

template <>
void Tab<uint64_t,TAB::Symbol>::sort(

  std::vector<uint64_t>& keys,
  std::vector<uint64_t>& values,

  uint64_t hi_freq

) {

// ---   *   ---   *   ---
// get highest freq for each run
//
// 'sorted' symbols have a freq of 0
// and will be skipped

  uint64_t limit  = keys.size();
  uint64_t bottom = limit-1;

  values.resize(limit);

  for(uint64_t i=0;i<limit;i++) {

    uint64_t     top     = 0;
    TAB::Symbol* top_sym = NULL;

// ---   *   ---   *   ---
// walk remaining

    for(uint64_t key : keys) {

      // get entry
      TAB::Symbol& sym=this->get(key);

      // skip already sorted
      if(!sym.freq) {
        continue;

      // discard low-frequency blocks
      // from even being sorted
      } else if(sym.freq<hi_freq) {
        sym.freq=0;
        sym.idex=bottom--;

        values[sym.idex]=sym.value;

        continue;

      };

      // compare
      if(sym.freq>top) {
        top=sym.freq;
        top_sym=&sym;

      };

    };

// ---   *   ---   *   ---
// assign idex to result and
// mark symbol as sorted

    if(top_sym==NULL) {break;};

    top_sym->idex=i;
    top_sym->freq=0;

    values[i]=top_sym->value;

    if(bottom<=i) {break;};

  };

};

// ---   *   ---   *   ---

template <>
void Tab<uint64_t,TAB::Symbol>::dump(
  std::string fpath

) {

  // number of occupied slots
  uint64_t elem_cnt=0;

  // get mem
  std::vector<uint64_t> buff;
  buff.reserve(m_size<<REAL_MULT_B);

  for(

    uint64_t fake=0;

    fake < (m_size>>REAL_MULT_B);
    fake++

  ) {

// ---   *   ---   *   ---
// skip unused slots

    uint64_t mask=m_masks[fake];
    if(!mask) {continue;};

// ---   *   ---   *   ---
// push elems to buff

    // bits in mask is number of
    // elems in this slot
    uint64_t cnt=popcount(mask);

    // get idex of value
    uint64_t real=fake<<REAL_MULT_B;

    // walk the inner array
    for(uint64_t j=0;j<cnt;j++) {

      // get elem
      TAB::Symbol sym=m_values[real+j];

      // skip unused
      if(!sym.freq) {continue;};

      elem_cnt++;

      // save values
      buff.push_back(sym.value);
      buff.push_back(sym.freq);

    };

// ---   *   ---   *   ---
// write elems to disk

  };

  Bin b(fpath,Bin::NEW);
  b.write(buff.data(),elem_cnt*2*sizeof(uint64_t));

};

// ---   *   ---   *   ---
