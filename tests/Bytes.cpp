#include "kvrnel/src/Bytes.cpp"
#include <cstdio>
#include <cmath>

int main(void) {


  int   nbits = 2;
  float step  = FRAC1;

  int   top   = 1<<nbits;
  int   beg   = (-top)/2;
  int   end   = (top)/2;

  for(int i=beg;i<end;i++) {

    float   x = i*step;

    char    y = frac<char>(x,step,nbits);
    float   z = unfrac<char>(y,step,nbits);

    printf("%f : %.04X %f\n",x,y&0xF,z);

  };

  return 0;

};
