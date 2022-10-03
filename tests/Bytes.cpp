#include "kvrnel/src/Bytes.cpp"

#include <cstdio>
#include <cmath>

int main(void) {

  int   nbits = Frac::BITS[2];
  float step  = Frac::STEP[2];

  bool  unsig = Frac::UNSIGNED;

printf("MAX %02X\n",Frac::MAXV[nbits]);

  Frac::Rounding_Mode=Frac::CORD;

  int   top   = Frac::MAXV[nbits];
  int   beg   = 0;//((-top)/2)-1;
  int   end   = top+1;//((top)/2)+1;

  for(int i=beg;i<end;i++) {

    float   x = i*step;

    char    y = frac<char>(x,step,nbits,unsig);
    float   z = unfrac<char>(y,step,nbits,unsig);

    printf("%i : %.04X %f\n",i,y,z);

  };

  return 0;

};
