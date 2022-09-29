// ---   *   ---   *   ---
// ID
// papers, please
//
// LIBRE SOFTWARE
// Licenced under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "kvrnel/ID.hpp"

// ---   *   ---   *   ---

ID::ID(

  const char* sig,
  const char* key

) {

  int i=0;

  do {

    m_sig[i++]=*sig++;
    if(i==SLIMIT || !*sig) {
      break;

    };

  } while(*sig);

  while(i<=SLIMIT) {
    m_sig[i++]='_';

  };

// ---   *   ---   *   ---

  i=0;

  do {

    m_key[i++]=*key;

    if(i==KLIMIT) {
      m_key[KLIMIT]='\0';
      break;

    }

  } while(*key++);

// ---   *   ---   *   ---

  if(i!=KLIMIT) {
    m_key[i]='\0';

  };

};

// ---   *   ---   *   ---
