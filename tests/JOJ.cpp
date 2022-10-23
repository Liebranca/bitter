#include "ff/JOJ.hpp"

int main(void) {

  const char comp[]={
    JOJ::YAUV,0x7F

  };

  { JOJ j(

      "./out",
      "/home/lyeb/Cruelty/Medi/",

      comp

    );

    j.add_img_set("test");
    j.pack();

  };

  JOJ j("./out");

  j.unpack();
  j.to_png(0,"./out_im");

  return 0;

};
