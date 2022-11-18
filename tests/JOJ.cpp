#include "ff/JOJ.hpp"
#include "ff/CRK.hpp"

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

//    CRK::Sprite_Build frames={
//
//      .scale = {
//        j.get_tile_scale(),
//        j.get_atlas_scale()
//
//      },
//
//      .atlas = tab
//
//    };
//
//    CRK::Mesh_Builds blds;
//    CRK::Mesh_Build bld={
//      .type=CRK::SPRITE,
//      .data=&frames
//
//    };
//
//    blds.push_back(bld);
//
//    CRK("./out.crk",blds);

  } endchk;

  errchk {

    JOJ j("./out");
    j.unpack();

    j.to_png(0,"atlas",JOJ::UNPACK_ATLAS);

    j.to_png(0,"frame0",JOJ::UNPACK_IMAGE);
    j.to_png(1,"frame1",JOJ::UNPACK_IMAGE);

  } endchk;

  return 0;

};
