//**************************
// ZJC_CLOCK               *
// a clock                 *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "zjc_clock.h"

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

//   ---     ---     ---     ---     ---
// #:0x0;>


#define CLCK_ONFLG 0x01
#define CLCK_RESTFLG 0x02
static const float CPS=1.0f/CLOCKS_PER_SEC;
static CLCK* cclck=NULL;

//   ---     ---     ---     ---     ---
// #:0x1;>


CLCK* MKCLCK(
  char* chars,
  uint frcap,
  float sctim) {


  CLCK* clck;ID id=IDNEW("CLCK",chars);
  MEMGET(CLCK,clck,8,&id);

//   ---     ---     ---     ---     ---
// #:0x2;>


  clck->frbeg=clock();
  clck->frend=clck->frbeg;

  clck->frdlt=0;
  clck->frtim=0;
  clck->sltim=0;
  clck->sctim=sctim;
  clck->fBy=0.0f;

  clck->frcap=frcap;
  clck->frcnt=0;

  clck->flags=0;

  clck->display[0]='\0';

//   ---     ---     ---     ---     ---
// #:0x3;>


  int i=0;do {

    clck->display[i]=*chars;i++;

  } while(*chars++&&i<4);

  clck->display [4]='\0';

  clck->display_pos[0]=23;
  clck->display_pos[1]=1;

  return clck;

};

//   ---     ---     ---     ---     ---
// #:0x4;>


void CALCFRDLT(void) {
  cclck->frdlt+=(\
  (cclck->frend)
   \
  -(cclck->frbeg)\
  )* CPS;\
  cclck->flags|=CLCK_ONFLG;

};

//   ---     ---     ---     ---     ---
// #:0x5;>


void CLCKWIN(uint w) {
 cclck->frdlt+=w;

};

float GTSCTIM(void) {
  return cclck->sctim;

};

void STSCTIM(float sctim) {
  cclck->sctim=sctim;

};

//   ---     ---     ---     ---     ---
// #:0x6;>


void CALCFRTIM(void) {
  cclck->frtim=(CLOCKS_PER_SEC/cclck->frcap);

};

void STFRCAP(int frcap) {
  cclck->frcap=frcap;CALCFRTIM();

};

//   ---     ---     ---     ---     ---
// #:0x7;>


int GTSLEEP(void) {
  if(cclck->frtim<cclck->frdlt) {

    cclck->frdlt=0;return 0;
  };return cclck->frdlt<cclck->frtim;

};

//   ---     ---     ---     ---     ---
// #:0x8;>


void CALCSLTIM(void) {

  cclck->sltim=(

   (cclck->frtim)
  -(cclck->frdlt)

  );cclck->frdlt=0;

};

//   ---     ---     ---     ---     ---
// #:0x9;>


uint GTSLTIM(void) {
  return cclck->sltim;

};

uint GTUBY(uint base) {
  cclck->uBy=(uint) (cclck->fBy * base);

};

void GTFBY(void) {
  CALCSLTIM();

  cclck->fBy=(

    1.0f
   /(cclck->frtim-cclck->sltim)

  );GTUBY(256);

};

//   ---     ---     ---     ---     ---
// #:0xA;>


void STCLCK(CLCK* clck) {
  cclck=clck;CALCFRTIM();

};

float GTFRDLT(void) {
  return cclck->frdlt;

};

//   ---     ---     ---     ---     ---
// #:0xB;>


void DRCLCK(void) {
  /*CALOUT(S,"\x1b[%i;%iH%c %s",

  cclck->display_pos[0],
  cclck->display_pos[1],
  cclck->display[(cclck->frcnt)%4],

  cclck->m.id.full

  );*/

  cclck->frcnt++;

  if(cclck->frcnt>=cclck->frcap) {
    cclck->frcnt=0;

  };

};

//   ---     ---     ---     ---     ---
// #:0xC;>


void KFRBEG(void) {
  cclck->frbeg=clock();

};

void KFREND(void) {

  cclck->frend=clock();
  // DRCLCK();

  CALCFRDLT();
  GTFBY();

};

//   ---     ---     ---     ---     ---
// #:0xD;>

