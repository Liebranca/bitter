#include "ff/JOJ.hpp"
#include "ff/CRK.hpp"
#include "ff/DAF.hpp"

int main(void) {

  const char comp[]={
    JOJ::YAUV,0x7F

  };

  errchk {

    JOJ j(

      "./out",
      "/home/lyeb/Cruelty/Medi/",

      comp

    );

    j.add_img_set("0");
    j.add_img_set("1");

    j.pack();

    CRK c("./out.crk",j);
    c.pack();

  } endchk;

// ---   *   ---   *   ---

  errchk {

    Bin j("./out",Bin::READ);
    Bin c("./out.crk",Bin::READ);

    DAF daf("./a.daf",Bin::NEW);

    daf.push(j);
    daf.push(c);

    j.unlink();
    c.unlink();

  } endchk;

// ---   *   ---   *   ---

  errchk {

    DAF daf("./a.daf",Bin::READ);
    auto joj_path=daf.extract(0);

    JOJ j(joj_path);
    j.unpack();

    j.to_png(0,"atlas",JOJ::UNPACK_ATLAS);

    j.to_png(0,"frame0",JOJ::UNPACK_IMAGE);
    j.to_png(1,"frame1",JOJ::UNPACK_IMAGE);

  } endchk;

  return 0;

};
