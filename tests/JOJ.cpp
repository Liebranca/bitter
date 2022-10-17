#include "ff/JOJ.hpp"

int main(void) {

  { JOJ j(

      "./out",
      "/home/lyeb/Cruelty/Medi/"

    );

    j.add_img_set("test");
    j.pack();

  };

  JOJ j("./out");

  j.unpack();
  j.to_png(2,"./out_im");

  return 0;

};
