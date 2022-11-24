#include "ff/JOJ.hpp"
#include "ff/CRK.hpp"
#include "ff/DAF.hpp"

int main(void) {

  const char comp[]={
    JOJ::YAUV,0x7F

  };

  errchk {

    JOJ j(
      "./out.joj",
      "/home/lyeb/Cruelty/Medi/",

      comp

    );

    j.add_img_set("0");
    j.add_img_set("1");

    j.atlas_sz(256);
    j.pack();

  } endchk;

// ---   *   ---   *   ---

  errchk {

    JOJ j("./out.joj");
    j.unpack();

    j.to_png(0,"atlas",JOJ::UNPACK_ATLAS);

    j.to_png(0,"frame0",JOJ::UNPACK_IMAGE);
    j.to_png(1,"frame1",JOJ::UNPACK_IMAGE);

  } endchk;

  return 0;

};
