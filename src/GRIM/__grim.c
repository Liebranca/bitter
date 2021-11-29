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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

//   ---     ---     ---     ---     --- gb:boxes

#define frlen 0x100
#define scrsz_x 0x3F
#define scrsz_y 0x1F

#define utsz 0x4000

char utbuff[utsz]={0};      // output buffer
int ut_i=0;                 // ^cur.idex into

// we write to this custom buffer full of long
// escapes and things, *then* we dump the whole
// string into stdout, in one go
//
// better than repeated printf? i think so

//   ---     ---     ---     ---     --- gb:utwrite

void utwr(char* format,...) {

  va_list args;

  va_start(args, format);
  vsprintf(utbuff+ut_i, format, args);
  va_end(args);

  ut_i+=strlen(utbuff+ut_i);

};

//   ---     ---     ---     ---     --- gb:utflush

void utfl(void) {
  printf("%s",utbuff);fflush(stdout);

  memset(
    utbuff,0,
    ut_i*sizeof(char)

  );ut_i=0;

}

//   ---     ---     ---     ---     ---

enum modes { r_mode,w_mode,c_mode };
static int mode=r_mode;

typedef void (*fr_mode)(uint64_t);

static fr_mode fr_act[]={

  &fr_rmode,
  &fr_wmode,
  &fr_cmode

};

//   ---     ---     ---     ---     --- gb:palette

enum palette_i {

  p_def_b,p_cur_b,p_sel_b,p_cur_d,

  p_def_f,p_str_f,p_car_b,p_num_f,
  p_ope_f,p_key_f,p_ins_f,p_dtv_f,

  

};static char* palette[]={

  "0;0;32",                 // background default
  "0;0;64",                 // bkg current line
  "128;0;0",                // shift-select
  "64;0;64",                // delimiter highlight

  "64;174;64",              // foreground default
  "0;160;160",              // strings/comments
  "176;176;0",              // caret
  "208;152;32",             // numbers
  "176;160;64",             // operators

  "128;32;160",             // keywords
  "0;128;176",              // instructions/types
  "160;16;32"               // directives

};

//   ---     ---     ---     ---     --- grmb

typedef struct {

  int mx;int x;int Mx;
  int my;int y;int My;

  int of;int sz;

  char* s;char* ptr;

} GRMB;

static GRMB scr={
  0,0,scrsz_x-7,
  0,0,scrsz_y-6,

  0,2048

};static GRMB cmd={
  2,2,scrsz_x-7,
  scrsz_y-5,scrsz_y-5,scrsz_y-4,

  1,256

};

//   ---     ---     ---     ---     --- grmb:nit

void ntgrmb(
  GRMB** buffs,
  int num) {

  for(int x=0;x<num;x++) {

    GRMB* buff=buffs[x];

    buff->s=(char*) malloc(buff->sz*sizeof(char));
    memset(buff->s,0,buff->sz*sizeof(char));
    buff->ptr=buff->s;

  };

};

//   ---     ---     ---     ---     --- grmb:drw

int logrmb(GRMB* buff) {

  if(!buff->of) {
    return buff->y*buff->Mx;

  };return (buff->y-(buff->my))*(buff->Mx-buff->mx);

};

void drwgrmb(
  GRMB* buff,
  int iscur) {

  int x=buff->mx;int ofst=logrmb(buff);
  char* s=buff->s+ofst;

  while(!(*s)) {
    s++;x++;

    if(x>=buff->Mx) {
      x=buff->mx;break;

    };
  };

  char* bc=(
    (iscur)
    ? palette[p_cur_b]
    : palette[p_def_b]

  );utwr(
    "\e[%u;1H\e[48;2;%sm\e[38;2;%sm\e[K",
    scr.y+1,bc,palette[p_def_f]

  );utwr(
    "\e[%u;%uH%s\e[0m",
    scr.y+1,x+1,s

  );

};

//   ---     ---     ---     ---     --- grmb:mv

void mvgrmb(
  GRMB* buff,
  int x,
  int y) {

  buff->x+=x;buff->y+=y;

  if(buff->x>=buff->Mx) {
    buff->x=buff->Mx;

  } else if(buff->x<buff->mx) {
    buff->x=buff->mx;

  };

  if(buff->y>=buff->My) {
    buff->y=buff->My-1;

  } else if(buff->y<buff->my) {
    buff->y=buff->my;

  };

//   ---     ---     ---     ---     --- grmb:mvptr

  if(!buff->of) {
    buff->ptr=(
      buff->s

     +(buff->x
     +((buff->y)*buff->Mx))

    );return;

  };

  buff->ptr=(
    buff->s

   +((buff->x-(buff->mx))
   +((buff->y-(buff->my))*(buff->Mx-buff->mx)))

  );
};

//   ---     ---     ---     ---     --- rmode

#define CTRL_X 0x0018       // ctrl+x
#define META_X 0x781B       // alt+x
#define DEL    0x7F         // <- backspace
#define RET    0x0A         // enter/return/newline

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
      mode=w_mode;
      break;

    default:
      break;

  };

//   ---     ---     ---     ---     --- rmode:move

  drwgrmb(&scr,0);
  mvgrmb(&scr,x,y);
  drwgrmb(&scr,1);

  utwr(

    "\e[27;1H\e[48;2;%sm"
    "\e[38;2;%sm\e[K"

    "[%02"PRIX8";%02"PRIX8"] 0x%"
    PRIXPTR"/%"PRIXPTR"\e[0m",

    palette[p_cur_b],palette[p_str_f],

    scr.y+1,scr.x+1,scr.ptr,scr.s+(scr.sz-1)

  );

  if(*scr.ptr) {
    utwr("\e[%u;%uH%c\b",scr.y+1,scr.x+1,*scr.ptr);

  } else {
    utwr("\e[%u;%uH",scr.y+1,scr.x+1);

  };

};

//   ---     ---     ---     ---     --- wmode

void fr_wmode(uint64_t in) {

  if(in && in!=META_X) {

    if(in!=DEL) {

      if(scr.x>=scr.Mx) {
        scr.ptr--;

      };*scr.ptr=(char) in;

      utwr(

        "\e[%u;%uH"
        "\e[48;2;%sm"
        "\e[38;2;%sm"
        "%c\e[0m",

        scr.y+1,scr.x+1,
        palette[p_cur_d],palette[p_car_b],

        *scr.ptr

      );if(in!=RET) {
        mvgrmb(&scr,1,0);
        return;

      };scr.x=0;mvgrmb(&scr,0,1);

//   ---     ---     ---     ---     --- wmode:del

    } else if(scr.x>=scr.mx) {

      mvgrmb(&scr,-1,0);
      *scr.ptr=0x20;

      utwr(

        "\e[%u;%uH"
        "\e[48;2;%sm"
        "\e[38;2;%sm"
        "%c\b\e[0m",

        scr.y+1,scr.x+1,
        palette[p_cur_d],palette[p_car_b],

        *scr.ptr

      );

    };

  };

};

//   ---     ---     ---     ---     --- cmode

void fr_cmode(uint64_t in) {

  if(in && in!=META_X) {

    if(in==RET) {

      utwr(
        "\e[27;1H\e[48;2;%sm\e[K"
        "\e[38;2;%sm$:\e[0m"
        "\e[%u;%uH%s",
        palette[p_cur_d],palette[p_car_b],
        scr.y+1,scr.x+1,cmd.s

      );cmd.x=0;cmd.y=0;mvgrmb(&cmd,0,0);
      memset(cmd.s,0,strlen(cmd.s));

      mode=r_mode;

//   ---     ---     ---     ---     --- cmode:ins

    } else if(in!=DEL) {

      if(cmd.x>=cmd.Mx) {
        cmd.ptr--;

      };*cmd.ptr=(char) in;

      utwr(

        "\e[%u;%uH"
        "\e[48;2;%sm"
        "\e[38;2;%sm"
        "%c\e[0m",

        cmd.y+1,cmd.x+1,
        palette[p_cur_d],palette[p_car_b],

        *cmd.ptr

      );mvgrmb(&cmd,1,0);

//   ---     ---     ---     ---     --- cmode:del

    } else if(cmd.x>=cmd.mx) {

      mvgrmb(&cmd,-1,0);
      *cmd.ptr=0x20;

      utwr(

        "\e[%u;%uH"
        "\e[48;2;%sm"
        "\e[38;2;%sm"
        "%c\b\e[0m",

        cmd.y+1,cmd.x+1,
        palette[p_cur_d],palette[p_car_b],

        *cmd.ptr

      );int end=strlen(cmd.s)-1;
      while(cmd.s[end]==0x20) {
        cmd.s[end]=0x00;end--;

      };

    };

  };
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

//   ---     ---     ---     ---     --- e:setup

  struct termios term;
  tcgetattr(fno, &term);

  term.c_lflag &=~(ICANON|ECHO);
  tcsetattr(fno, TCSANOW, &term);

  fc|=O_NONBLOCK;
  fcntl(fno, F_SETFL, fc);

//   ---     ---     ---     ---     --- e:loop

  char buff[16]={0x00};
  uint64_t clk=(uint64_t) clock();
  uint64_t* btt=(uint64_t*) &buff;

  GRMB* grmbs[]={&scr,&cmd};
  ntgrmb(grmbs, 2);

  while(*btt!=0x1B) {

    *btt^=*btt;fgets(buff, 16, f);
    clk=((uint64_t) clock())-clk;

    if(*btt) {

      if(*btt==META_X && mode!=c_mode) {
        mode=c_mode;
        printf(
          "\e[27;1H\e[48;2;%sm\e[K"
          "\e[38;2;%sm$:\e[0m",
          palette[p_cur_d],palette[p_car_b]

        );cmd.x=0;cmd.y=0;mvgrmb(&cmd,0,0);

      };

      //printf("0x%016" PRIX64 "\n",*btt);

    };fr_act[mode](*btt);
    if(ut_i) {
      utfl();

    };if(clk<frlen) {
      usleep(frlen-clk);

    } else {
      usleep(frlen);

    };

  };

//   ---     ---     ---     ---     --- e:clenup

  free(cmd.s);
  free(scr.s);

  term.c_lflag |= ICANON|ECHO;
  tcsetattr(fno, TCSANOW, &term);

  fc&=~O_NONBLOCK;
  fcntl(fno, F_SETFL, fc);

};

//   ---     ---     ---     ---     --- e:end