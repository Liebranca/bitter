#include "kvrnel/src/Bytes.cpp"
#include <cstdio>
#include <cmath>

int main(void) {


  int   nbits = 4;
  float step  = FRAC3;

  for(int i=0;i<(1<<nbits);i++) {

    float   x = i*step;

    char    y = frac<char>(x,step,nbits);
    float   z = unfrac<char>(y,step,nbits);

    printf("%f : %.04X %f\n",x,y&0xF,z);

  };

  return 0;

};
