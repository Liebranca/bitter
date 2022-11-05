#include "ff/PNG_Wrap.hpp"

int main(void) {

  PNG im("./test_im");
  Mem<uint8_t> red=im.read();

  for(uint64_t i=0;i<16*4;i+=4) {
    red[i+2]=255;

  };

  im.write(red);

  return 0;

};
