#include "kvrnel/Evil.hpp"
#include "ff/DAF.hpp"

int main(void) {

  DAF daf("./a.daf",Bin::NEW);
  Bin blk0("./blk0",Bin::READ);
  Bin blk1("./blk1",Bin::READ);

  errchk;
    daf.push(blk0);
    daf.push(blk0);

    daf.insert(blk1,2);
    daf.push(blk1);

  endchk;

  return 0;

};
