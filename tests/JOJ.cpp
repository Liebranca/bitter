#include "ff/JOJ.hpp"

int main(void) {

//  { JOJ j(
//
//      "./out",
//      "/home/lyeb/Cruelty/Medi/"
//
//    );
//
//    j.add_img_set("test");
//    j.pack();
//
//  };

  JOJ j("./out");

  j.unpack();

  auto buff=j.to_buff(1,255.0f);
  j.to_png(buff.get(),"./out_im");

  return 0;

};
