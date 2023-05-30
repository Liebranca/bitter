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
      "./",

      comp

    );

    j.add_img_set("ElQuds");

    j.atlas_sz(256);
    j.pack();

  } endchk;

// ---   *   ---   *   ---

  errchk {

    JOJ j("./out.joj");

    j.unpack();

    j.to_png(0,"atlas",JOJ::UNPACK_ATLAS);
    j.to_png(0,"image",JOJ::UNPACK_IMAGE);

  } endchk;

  return 0;

};
