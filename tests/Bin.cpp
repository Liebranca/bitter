#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Bin.cpp"
#include "kvrnel/src/Bytes.cpp"
#include "kvrnel/src/Tab.cpp"

#include "ff/src/JOJ.cpp"

#include <ctime>
#include <png++/png.hpp>

float rflut(void) {

  return ((float) rand()
  / (((float) RAND_MAX)/2)

  )-1;

};

void fnorm(float* v) {

  float mag=sqrt(

    (v[0]*v[0])
  + (v[1]*v[1])
  + (v[2]*v[2])

  );

  v[0]/=mag;
  v[1]/=mag;
  v[2]/=mag;

};

int main(void) {

//  srand((unsigned int) time(NULL));
//
//  float pixels[]={
//    rflut(),rflut(),rflut(),rflut()
//
//  };
//
//  fnorm(pixels);

  png::image<png::rgba_pixel> im(
    "/home/lyeb/Downloads/ElQuds.png"

  );

  size_t width=im.get_width()>>1;
  size_t height=im.get_height()>>1;

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
    j.write();

  };

// ---   *   ---   *   ---

  JOJ j(

    "./out",
    &pixels[0]

  );

  j.read();
  j.encoder(JOJ::YAUV,Frac::DECODE);

// ---   *   ---   *   ---

  i=0;
  for(size_t y=0;y<height;y++) {

    size_t row=y*width;

    for(size_t x=0;x<width;x++) {

      png::rgba_pixel px=im.get_pixel(x,y);

      yauv2rgba(pixels+i);

      px.red   = pixels[i++]*255.0f;
      px.green = pixels[i++]*255.0f;
      px.blue  = pixels[i++]*255.0f;
      px.alpha = pixels[i++]*255.0f;

      im[y][x]=px;

    };

  };

  im.write("./out_im");

  return 0;

};
