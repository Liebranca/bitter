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

  #include "id.h"

// ---   *   ---   *   ---

ID::ID(

  const char* sig,
  const char* key

) {

  int i=0;

  do {

    this->sig[i++]=*sig++;
    if(i==SLIMIT || !*sig) {
      break;

    };

  } while(*sig);

  while(i<=SLIMIT) {
    this->sig[i++]='_';

  };

// ---   *   ---   *   ---

  i=0;

  do {

    this->key[i++]=*key;

    if(i==KLIMIT) {
      this->key[KLIMIT]='\0';
      break;

    }

  } while(*key++);

// ---   *   ---   *   ---

  if(i!=KLIMIT) {
    this->key[i]='\0';

  };

};

// ---   *   ---   *   ---
