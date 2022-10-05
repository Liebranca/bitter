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

typedef struct {

  size_t idex;
  size_t x;
  size_t y;

  char nkey[64];

} Tab_Hash;

// ---   *   ---   *   ---
// info

template <typename K,typename T>
class Tab {

public:

  VERSION     "v2.00.1";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---

private:

  cx64 REAL_MULT=64;
  cx64 NKEY_SZ=64;

  size_t              m_size;
  std::vector<size_t> m_masks;

  std::vector<K>      m_keys;
  std::vector<T>      m_values;

// ---   *   ---   *   ---
// internals

  Tab_Lookup get_mask(K& k);

  size_t hash(K& k);

  inline void hash_f(
    Tab_Hash* h

  );

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

  void irep(
    std::string& x,
    Tab_Hash* h

  );

// ---   *   ---   *   ---
// key==unsigned long

  inline bool key_match(
    size_t key,
    size_t idex

  );

  void irep(size_t x,Tab_Hash* h);

// ---   *   ---   *   ---
// interface

public:

  // compiler trash
  Tab(void) {};

  // nit/del
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
  inline T& get(K& k);

  // ^key already hashed
  inline T& get(Tab_Lookup& lkp);

  // true if key in table
  inline Tab_Lookup has(K& k);

  // assign value to key
  inline void set(K& k,T& v);

  // ^key hashed
  inline void set(Tab_Lookup& lkp,T& v);


};

// ---   *   ---   *   ---

#endif // __24_TAB_H__
