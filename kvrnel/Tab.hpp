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
  inline size_t collide(size_t idex,K& k);

  inline bool key_match(
    std::string& key,
    size_t idex

  );

// ---   *   ---   *   ---

public:

  Tab(size_t fake);
  ~Tab(void);

  T pop(K& k);
  size_t push(K k,T v);

  inline T get(K& k);
  inline void set(K& k,T v);


};

// ---   *   ---   *   ---

#endif // __24_TAB_H__
