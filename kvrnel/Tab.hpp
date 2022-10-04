#ifndef __24_TAB_H__
#define __24_TAB_H__

// ---   *   ---   *   ---
// deps

  #include <vector>
  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---

typedef struct {

  size_t orig;

  size_t real;
  size_t fake;
  size_t mask;

  bool   key_match;

} Tab_Lookup;

// ---   *   ---   *   ---
// info

template <typename K,typename T>
class Tab {

public:

  VERSION     "v2.00.1";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---

private:

  cx64 REAL_MULT=8;

  size_t              m_size;
  std::vector<size_t> m_masks;

  std::vector<K>      m_keys;
  std::vector<T>      m_values;

// ---   *   ---   *   ---
// internals

  Tab_Lookup get_mask(K& k);

  size_t hash(K& k);

  inline void update_entry(
    Tab_Lookup& lkp,
    K& k,T& v

  );

  void update_mask(
    Tab_Lookup& lkp

  );

// ---   *   ---   *   ---
// key==string

  inline bool key_match(
    std::string& key,
    size_t idex

  );

  void irep(std::string x,short* nkey);

// ---   *   ---   *   ---
// key==unsigned long

  inline bool key_match(
    size_t key,
    size_t idex

  );

  void irep(size_t x,short* nkey);

// ---   *   ---   *   ---
// interface

public:

  Tab(size_t fake);
  ~Tab(void);

  // remove entry
  T pop(K& k);

  // add entry
  size_t push(K& k,T& v);

  // ^same, use if key is already hashed
  inline void push(
    Tab_Lookup& lkp,
    K& k,T& v

  );

  // value from key
  inline T get(K& k);

  // true if key in table
  inline Tab_Lookup has(K& k);

  // assign value to key
  inline void set(K& k,T& v);


};

// ---   *   ---   *   ---

#endif // __24_TAB_H__
