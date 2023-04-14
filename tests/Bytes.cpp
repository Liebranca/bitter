#include "kvrnel/src/Bytes.cpp"

#include <cstdio>
#include <cmath>

int main(void) {

  int   nbits = Frac::BITS[Frac::SIZE_4BIT];
  float step  = Frac::STEP[Frac::STEP_3BIT];

  bool  unsig = Frac::SIGNED;

printf("MAX %02X\n",Frac::MAXV[nbits]);
printf("MID %02X\n",1<<nbits);

  Frac::Rounding_Mode=Frac::CORD;

  int   top   = Frac::MAXV[nbits];
  int   half  = (top/2)+1;
  int   beg   = -half;
  int   end   = half;

  for(int i=beg;i<end;i++) {

    float   x = i*step;

    int8_t  y = frac<int8_t>(x,step,nbits,unsig);
    float   z = unfrac<int8_t>(y,step,nbits,unsig);

    printf("%2i -> %.2f : %.04X %.2f\n",i,x,y,z);

  };

  return 0;

};
