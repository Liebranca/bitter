#include "ff/JOJ.hpp"

int main(void) {

  { JOJ j(

      "./out",
      "/home/lyeb/Cruelty/Medi/"

    );

    j.add_img_set("test");
    j.pack();

  };

//// ---   *   ---   *   ---
//
//  JOJ j(
//
//    "./out",
//    &pixels[0]
//
//  );
//
//  j.read();
//  j.encoder(JOJ::YAUV,Frac::DECODE);
//
//// ---   *   ---   *   ---
//
//  i=0;
//  for(size_t y=0;y<height;y++) {
//
//    size_t row=y*width;
//
//    for(size_t x=0;x<width;x++) {
//
//      png::rgba_pixel px=im.get_pixel(x,y);
//
//      yauv2rgba(pixels+i);
//
//      px.red   = pixels[i++]*255.0f;
//      px.green = pixels[i++]*255.0f;
//      px.blue  = pixels[i++]*255.0f;
//      px.alpha = pixels[i++]*255.0f;
//
//      im[y][x]=px;
//
//    };
//
//  };
//
//  im.write("./out_im");

  return 0;

};
