#include "kvrnel/Evil.hpp"
#include "ff/DAF.hpp"

int main(void) {

  errchk {

    DAF daf("./a.daf",Bin::NEW);

    Bin blk0("./blk0",Bin::READ);
    Bin blk1("./blk1",Bin::READ);

    daf.push(blk0);
    daf.push(blk0);

    daf.insert(blk1,1);
    daf.push(blk1);

    daf.pop();
    daf.remove(0);

    daf.replace(blk0,0);
    daf.replace(blk1,1);

  } endchk;

// ---   *   ---   *   ---

  errchk {

    DAF daf("./a.daf",Bin::READ);
    auto m=daf.read();

    m.prich();

  } endchk;

  return 0;

};
