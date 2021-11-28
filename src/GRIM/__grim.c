//**************************
// GRIM                    *
// dirty cli modal editor  *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "__grim.h"

#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

//   ---     ---     ---     ---     --- boxes

#define frlen 0x100
#define scrsz_x 0x3F
#define scrsz_y 0x1F

enum modes { r_mode,w_mode,c_mode };
static int mode=r_mode;

typedef void (*fr_mode)(uint64_t);

static fr_mode fr_act[]={

  &fr_rmode,
  &fr_wmode,
  &fr_cmode

};

//   ---     ---     ---     ---     --- grmb

typedef struct {

  int x;int mx;int us;
  int y;int my;int sz;

  char* s;char* ptr;

} GRMB;static GRMB scr={
  0,scrsz_x-7,0,0,scrsz_y-5,2048

};

//   ---     ---     ---     ---     --- grmb:mv

void mvgrmb(
  GRMB* buff,
  int x,
  int y) {

  buff->x+=x;buff->y+=y;

  if(buff->x>=buff->mx) {
    buff->x=0;
    buff->y+=1;

  } else if(buff->x<0) {
    buff->x=buff->mx-1;
    buff->y-=1;

  };

  if(buff->y>=buff->my) {
    buff->y=buff->my-1;

  } else if(buff->y<0) {
    buff->y=0;

  };

  buff->ptr=(
    buff->s

   +(buff->x
   +(buff->y*buff->mx))

  );

};

//   ---     ---     ---     ---     --- rmode

#define LFT 0x6A            // j
#define RGT 0x6C            // l
#define FWD 0x69            // i
#define BAK 0x6B            // k

void fr_rmode(uint64_t in) {

  int x=0;
  int y=0;

  switch(in) {

    case LFT:
      x=-1;break;

    case RGT:
      x=1;break;

    case FWD:
      y=-1;break;

    case BAK:
      y=1;break;

    case 0x77:
      *(scr.ptr)=0x24;
      break;

    default:
      break;

  };

//   ---     ---     ---     ---     --- rmode:move

  mvgrmb(&scr,x,y);

  printf(

    "\e[27;1H\e[48;2;0;0;128m"
    "\e[38;2;64;174;64m\e[K"

    "[%u;%u] | %c | %u/%u\e[0m",

    scr.y+1,scr.x+1,(char) in,scr.ptr,scr.sz

  );

  if(*scr.ptr) {
    printf("\e[%u;%uH%c\b",scr.y+1,scr.x+1,*scr.ptr);

  } else {
    printf("\e[%u;%uH",scr.y+1,scr.x+1);

  };fflush(stdout);

};

//   ---     ---     ---     ---     --- wmode

void fr_wmode(uint64_t in) {

  ;

};

//   ---     ---     ---     ---     --- cmode

#define CTRL_X 0x0018
#define META_X 0x781B

void fr_cmode(uint64_t in) {

  ;

};

//   ---     ---     ---     ---     --- entry

/* unrelated popen reference ;>

  const char* cmd = "ls";
  FILE* pipe = popen(cmd, "r");

  char pbuff[256];int rb;

  while((

  rb=fread(pbuff,
    sizeof(pbuff[0]),
    sizeof(pbuff),pipe)

  )>0) {

    fgets(pbuff, sizeof(pbuff), pipe);
    pbuff[255]=0x00;printf("%s",pbuff);

  };pclose(pipe);

*/

void main(void) {

  FILE* f    = stdin;
  int   fno  = fileno(f);
  int   fc;

  if(!isatty(fno)) {
      printf("STDIN not a TTY\n");
      return;

  };system("clear");
  printf("\e[1;1H");

//   ---     ---     ---     ---     --- setup

  struct termios term;
  tcgetattr(fno, &term);

  term.c_lflag &=~(ICANON|ECHO);
  tcsetattr(fno, TCSANOW, &term);

  fc|=O_NONBLOCK;
  fcntl(fno, F_SETFL, fc);

  scr.s=(char*) malloc(scr.sz*sizeof(char));
  memset(scr.s,0,scr.sz*sizeof(char));
  scr.ptr=scr.s;

//   ---     ---     ---     ---     --- loop

  char buff[16]={0x00};
  uint64_t clk=(uint64_t) clock();
  uint64_t* btt=(uint64_t*) &buff;

  while(*btt!=0x1B) {

    *btt^=*btt;fgets(buff, 16, f);
    clk=((uint64_t) clock())-clk;

    if(*btt) {

      if(*btt==META_X && mode!=c_mode) {
        mode=c_mode;
        printf("\e[27;1H\e[48;2;0;0;0m\e[K");

      };

      //printf("0x%016" PRIX64 "\n",*btt);

    };fr_act[mode](*btt);

    if(clk<frlen) {
      usleep(frlen-clk);

    } else {
      usleep(frlen);

    };

  };

//   ---     ---     ---     ---     --- clenup

  free(scr.s);

  term.c_lflag |= ICANON|ECHO;
  tcsetattr(fno, TCSANOW, &term);

  fc&=~O_NONBLOCK;
  fcntl(fno, F_SETFL, fc);

};

//   ---     ---     ---     ---     --- end