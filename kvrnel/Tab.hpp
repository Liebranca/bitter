#ifndef __24_TAB_H__
#define __24_TAB_H__

// ---   *   ---   *   ---
// deps

  #include <vector>
  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// helpers

namespace TAB {

  typedef struct {

    uint64_t orig;

    uint64_t real;
    uint64_t fake;
    uint64_t mask;

    bool   key_match;

  } Lookup;

// ---   *   ---   *   ---

  typedef struct {

    uint64_t   idex;
    uint64_t   value;

    uint64_t   freq;

  } Symbol;

// ---   *   ---   *   ---

  typedef struct {

    uint64_t idex;
    uint64_t x;
    uint64_t y;

    char nkey[64];

  } Hash;

// ---   *   ---   *   ---
// namespace TAB

};

// ---   *   ---   *   ---
// info

template <typename K,typename T>
class Tab {

public:

  VERSION     "v2.00.1";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---

private:

  cx8  REAL_MULT_B = 6;
  cx64 REAL_MULT   = 1<<REAL_MULT_B;
  cx64 NKEY_SZ     = 64;

  uint64_t              m_size;
  std::vector<uint64_t> m_masks;

  std::vector<K>        m_keys;
  std::vector<T>        m_values;

  // used for symtabs
  std::vector<K>        m_keys_s;
  std::vector<K>        m_values_s;

  bool                  m_sorted;

// ---   *   ---   *   ---
// internals

  TAB::Lookup get_mask(K& k);

  uint64_t hash(K& k);

  void hash_f(
    TAB::Hash* h

  );

  void update_entry(
    TAB::Lookup& lkp,
    K& k,T& v

  );

  void update_mask(
    TAB::Lookup& lkp

  );

// ---   *   ---   *   ---

  void irep(
    std::string& x,
    TAB::Hash* h

  );

  void irep(uint64_t x,TAB::Hash* h);

// ---   *   ---   *   ---
// interface

public:

  // compiler trash
  Tab(void) {};

  // nit/del
  Tab(uint64_t fake);
  ~Tab(void);

  // remove entry
  T pop(K& k);

  // add entry
  uint64_t push(K& k,T& v);

  // ^same, use if key is already hashed
  void push(
    TAB::Lookup& lkp,
    K& k,T& v

  );

  // value from key
  inline T& get(K& k) {
    TAB::Lookup lkp=get_mask(k);
    return m_values[lkp.real];

  };

  // ^key already hashed
  inline T& get(TAB::Lookup& lkp) {
    return m_values[lkp.real];

  };

  // true if key in table
  inline TAB::Lookup has(K& k) {
    TAB::Lookup lkp=get_mask(k);
    return lkp;

  };

  // assign value to key
  inline void set(K& k,T& v) {
    TAB::Lookup lkp=get_mask(k);
    m_values[lkp.real]=v;

  };

  // ^key hashed
  inline void set(TAB::Lookup& lkp,T& v) {
    m_values[lkp.real]=v;

  };

// ---   *   ---   *   ---
// getters

  inline bool sorted(void) {
    return m_sorted;

  };

  inline T iget(uint64_t idex) {
    return this->get(m_keys_s[idex]);

  };

  inline std::vector<K>& sorted_buff(void) {
    return m_values_s;

  };

// ---   *   ---   *   ---
// specialized

  // given a table of symbols,
  // sort them into a vector by frequency
  void sort(
    uint64_t hi_freq=128

  );

  // given a table of symbols,
  // write it to disk
  void dump(std::string fpath);

  // conditional push for sorted symtab
  TAB::Symbol* cpush(uint64_t key);

};

// ---   *   ---   *   ---
// sugar

typedef Tab<uint64_t,TAB::Symbol> Symtab;

// ---   *   ---   *   ---

#endif // __24_TAB_H__
