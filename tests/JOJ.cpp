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

    j.add_img_set("0");
    j.add_img_set("1");

    j.pack();

  };

  JOJ j("./out");

  j.unpack();
  j.to_png(0,"./frame0");
  j.to_png(1,"./frame1");

  return 0;

};
