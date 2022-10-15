#include "ff/DAF.hpp"

int main(void) {

  DAF daf("./a.daf",Bin::NEW);
  Bin txt("./todo.mny",Bin::READ);

  daf.push(txt);
  daf.close();

  return 0;

};
