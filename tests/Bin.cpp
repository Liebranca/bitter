#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Bin.cpp"
#include "kvrnel/src/Bytes.cpp"
#include "kvrnel/src/Tab.cpp"

#include "ff/src/Zwrap.cpp"
#include "ff/src/JOJ.cpp"

#include <png++/png.hpp>

int main(void) {

  png::image<png::rgba_pixel> im(
    "/home/lyeb/Downloads/ElQuds.png"

  );

  size_t width=im.get_width()>>2;
  size_t height=im.get_height()>>2;

  size_t sz=width*height;

  std::unique_ptr<float> pixels_p(
    new float[sz*4]

  );

  size_t i=0;

  float* pixels=pixels_p.get();

  for(size_t y=0;y<height;y++) {

    size_t row=y*width;

    for(size_t x=0;x<width;x++) {

      png::rgba_pixel px=im.get_pixel(x,y);

      size_t orig=i;
      pixels[i++]=px.red/255.0f;
      pixels[i++]=px.green/255.0f;
      pixels[i++]=px.blue/255.0f;
      pixels[i++]=px.alpha/255.0f;

      rgba2yauv(pixels+orig);

    };

  };

// ---   *   ---   *   ---

  { JOJ j(

      "./out",

      &pixels[0],
      sz

    );

    j.encoder(JOJ::YAUV,Frac::ENCODE);

    j.compress();
//    j.write();

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
