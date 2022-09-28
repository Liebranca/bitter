#include "src/kvrnel/evil.cpp"
#include "src/kvrnel/types/id.cpp"
#include "src/kvrnel/mem/mem.cpp"

//#include <cstdio>

//class Empty {};
//
//template class Mem<Empty,uint8_t>;
//typedef Mem<Empty,uint8_t> Blk;

int moreso_broke(void) {
  return AR_ERROR;

};

int broken(void) {

  int retx=0;
  arcatch(moreso_broke(),retx,0x41,"WAT");

  return AR_FATAL;

};

int main(void) {

  int retx=0;
  arcatch(broken(),retx,0x02,"wat");

//  ID id("$","testy");
//  Blk* m=Blk::nit(65,&id);
//  Blk& rm=*m;
//
//  rm[0]=0x48;
//  rm[1]=0x4C;
//  rm[2]=0x4F;
//  rm[3]=0x57;
//
//  rm[4]=0x52;
//  rm[5]=0x4C;
//  rm[6]=0x44;
//  rm[7]=0x21;
//
//  m->prich(0);

};
