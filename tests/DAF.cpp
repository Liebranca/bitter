#include "ff/DAF.hpp"

int main(void) {

  DAF daf("./a.daf",Bin::NEW);
  Bin out("./out",Bin::READ);
  Bin txt("./todo.mny",Bin::READ);

  daf.push(txt);
  daf.push(txt);

  daf.insert(out,0);

  return 0;

};
