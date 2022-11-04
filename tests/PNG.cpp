#include "ff/PNG_Wrap.hpp"

int main(void) {

  PNG im("./test_im",4);
  Mem<uint8_t> red(16*4);

  for(uint64_t i=0;i<16*4;i+=4) {

    red[i+0]=255;
    red[i+1]=0;
    red[i+2]=0;
    red[i+3]=255;

  };

  im.write(red);

  return 0;

};
