#ifndef __24_PATH_H__
#define __24_PATH_H__

// ---   *   ---   *   ---
// deps

  #include "Style.hpp"

// ---   *   ---   *   ---

namespace Path {

  class Def {

  private:
    char* m_value;

  public:

    Def(const char* name,char* add_to=NULL);

    void redef(
      const char* name,
      char* add_to=NULL

    );

    inline const char* get(void);

  };

// ---   *   ---   *   ---

  static Def Root("ARPATH");

};

// ---   *   ---   *   ---

#endif // __24_PATH_H__
