//**************************
// BLKMGK                  *
// spiritual succesor of   *
// dsm moduleentry point   *
// for data packersgets    *
// the joj                 *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "zjc_joj.h"
#include "zjc_crk.h"
#include "../KVRNEL/kvr_paths.h"
#include "../KVRNEL/MEM/kvr_str.h"

#define BLKMGK_V 0.1

#define BLKMGK_KRUN 0x4B52554E
#define BLKMGK_DONE 0x444F4E45

//   ---     ---     ---     ---     ---
// #:0x0;>

static HRW DAF_DATA= {0};
static char DAF_JITEMS[ZJC_DAFSIZE][ZJC_IDK_WIDTH];
static CRKH DAF_CITEMS[ZJC_DAFSIZE];

//   ---     ---     ---     ---     ---
// #:0x1;>


EXPORT void NTBLKMGK(char** pth_l) {


  NTKVRPTH(pth_l);

                            // wipe these arrays
                            // on init
  CLMEM2(&DAF_DATA,sizeof(HRW));
  CLMEM2(DAF_JITEMS,xBYTES(ZJC_DAFSIZE,JOJH));
  CLMEM2(DAF_CITEMS,xBYTES(ZJC_DAFSIZE,CRKH));

  CALOUT('K',"BLKMGK v%f\n\b",BLKMGK_V);

};

EXPORT void DLBLKMGK(void) {DLKVRPTH();};
EXPORT void STDAFTOT(uint tot,uint siz) {DAF_DATA.total=tot;if(
  siz) {LDLNG(
  tot*siz);};
};

//   ---     ---     ---     ---     ---
// #:0x2;>


EXPORT void UTCRK(uint i,uint vert_count,
uint idex_count,\
uint cl,char* name) {


                            // kiknit on first
                            // run
  if(!i) {NTCRKENG(CRK_ENCODE);};

//   ---     ---     ---     ---     ---
// #:0x3;>


                            // save entry data
  for(
    int x=0;x<(ZJC_IDK_WIDTH-1);x++

  ) {
    DAF_CITEMS[DAF_DATA.idex].name[x]=*(
      name+x);
    if(*(name+x)=='\0') {break;

  }
};

DAF_CITEMS[DAF_DATA.idex].vert_count=vert_count;
DAF_CITEMS[DAF_DATA.idex].idex_count=idex_count;
DAF_CITEMS[DAF_DATA.idex].fracl=cl;

//   ---     ---     ---     ---     ---
// #:0x4;>


// set compression level and encode
STCFRACL(cl);
ENCCRK(DAF_CITEMS+DAF_DATA.idex,&DAF_DATA.size_i);

DAF_DATA.idex++;

//   ---     ---     ---     ---     ---
// #:0x5;>


if(i==BLKMGK_DONE) {
                            // zip and kikdel on
                            // last run
  ZPCRK(DAF_DATA.size_i,DAF_DATA.idex,DAF_CITEMS);
  DLCRKENG(CRK_ENCODE);
};};

//   ---     ---     ---     ---     ---
// #:0x6;>


EXPORT void BMENCJOJ(uint i) {


                            // kiknit on first
                            // run
  if(i==BLKMGK_KRUN) {NTJOJENG(JOJ_ENCODE);};

  elif(i==BLKMGK_DONE) {
                            // zip and kikdel on
                            // last run
    ZPJOJ(DAF_DATA.size_i,DAF_DATA.idex,DAF_JITEMS);
    DLWTBUF();
    DLJOJENG(JOJ_ENCODE);
  }

  else {STJOJDIM(i);

};};

//   ---     ---     ---     ---     ---
// #:0x7;>


EXPORT void UTJOJ(char* name,float* src) {


  if(GTJOJLST()==4) {

    for(int x=0;x<(ZJC_IDK_WIDTH-1);x++) {

      DAF_JITEMS[DAF_DATA.idex][x
       /4]=*(name+x);
      if(*(name+x)=='\0') {break;};

    };

  };ENCJOJ(src,&DAF_DATA);
  DAF_DATA.idex++;

};

//   ---     ---     ---     ---     ---
// #:0x8;>


EXPORT void INJOJ(uint i) {


  if(!i) {
                            // kiknit and unzip
                            // on first run
    NTJOJENG(JOJ_DECODE);
    uint sizes[4]= {
      DAF_DATA.size_i,DAF_DATA.size_d,\
      DAF_DATA.idex,0};

    UZPJOJ(DAF_JITEMS,sizes);

                            // size values from
                            // header
    DAF_DATA.size_i=sizes[0];\
    DAF_DATA.size_d=sizes[1];\
    DAF_DATA.idex=sizes[2];\

  };

//   ---     ---     ---     ---     ---
// #:0x9;>


  if(i!=BLKMGK_DONE) {
                            // decode joj (writes
                            // to pixdump)
    DECJOJ(DAF_JITEMS+i);
  }

//   ---     ---     ---     ---     ---
// #:0xA;>


  else {
                            // kikdel on added
                            // run
    DLJOJENG(JOJ_DECODE);
  };

};

//   ---     ---     ---     ---     ---
// #:0xB;>

