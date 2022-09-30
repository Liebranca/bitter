#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Bin.cpp"
#include "kvrnel/src/Bytes.cpp"

#include "ff/src/JOJ.cpp"

#include <ctime>

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

  srand((unsigned int) time(NULL));

  float pixels[]={
    rflut(),rflut(),rflut(),rflut()

  };

  fnorm(pixels);

  printf(

    "%.4f %.4f %.4f %.4f\n",
    pixels[0],pixels[1],pixels[2],pixels[3]

  );

  JOJ j("./out",pixels,1);

//  fnorm(pixels);
  printf(

    "%.4f %.4f %.4f %.4f\n",
    pixels[0],pixels[1],pixels[2],pixels[3]

  );

  return 0;

};
