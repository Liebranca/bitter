//**************************
// __SHB7                  *
// launches the kvr shell  *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "__shb7.h"

#include "KVRNEL/MEM/sh_map.h"

#include "CHASM/chMNG.h"

#include "SIN/__sin.h"
#include "SIN/sin_texture.h"
#include "SIN/REND/sin_canvas.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h>

#include <sys/wait.h>
#include <limits.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static uint* shbout=NULL;

static int shbout_mx=0;
static int shbout_my=0;

static int shbout_x=0;
static int shbout_y=0;

static int shbout_flg=0x00;

//   ---     ---     ---     ---     ---

void pshout(
  uchar* str,
  uchar col_id,
  char reset) {

  uint ox=shbout_x;
  uint oy=shbout_y;

  int pos=shbout_x+(shbout_y*shbout_mx);
  do {
    if(!*str) { break; }

    shbout[pos]=(*str)|(col_id<<8);
    pos++;shbout_x++;

    if(shbout_x>=shbout_mx) {
      shbout_x=0;
      shbout_y++;

    };

    if(shbout_y>=shbout_my) {
      shbout_x=0;
      shbout_y=0;
      pos=0;break;

    };

  } while(*str++);

  shbout_flg|=0x01;
  if(reset) {
    shbout_x=ox;
    shbout_y=oy;

  };

};

void oncont(int sig) {
  return;

};

//   ---     ---     ---     ---     ---

int main(int argc,char** argv) {

  char** PATH;
  int PATH_SZ=0;

  {

    // make search path array
    // PATH[0] is always cwd

    char* path=getenv("KVR_PATH");
    int x=0;

    while(*(path+x)) {
      if(*(path+x)==':') {
        PATH_SZ++;

      };x++;
    };

//   ---     ---     ---     ---     ---

    PATH=malloc(PATH_SZ*sizeof(char*));

    char pwd [PATH_MAX+1];  // find current directory
    char* cd;

    {
      cd = realpath(argv[0],pwd);
      int last=strlen(cd)-1;
      do {
        last--;

      } while(cd[last]!='/');
      cd[last+1]=0x00;

    };PATH[0]=malloc(strlen(cd)*sizeof(char)+2);

    strcpy(PATH[0],cd);
    char* lach=PATH[0]+(strlen(cd)-1);

    if(*lach!='/') {
      *(lach+1)='/';
      *(lach+2)=0x00;

    };

//   ---     ---     ---     ---     ---

    char* token=strtok(path,":");

    for(x=1;x<PATH_SZ;x++) {
      PATH[x]=malloc(strlen(token)*sizeof(char)+2);

      strcpy(PATH[x],token);
      lach=PATH[x]+(strlen(token)-1);

      if(*lach!='/') {
        *(lach+1)='/';
        *(lach+2)=0x00;

      };

      token=strtok(NULL,":");

    };
  };

//   ---     ---     ---     ---     ---

  void** mh;                // cat path to outfile
                            // out doubles as shmem
  {

    char fpath[PATH_MAX+1+8];
    strcat(fpath,"shb7wt");

    mh=ntmap_origin(fpath,4);

  };char* bf=(char*) (*mh);
  char* mh_key=encodemh(mh);

  signal(SIGCONT,oncont);
  int fr=1;// just for testing

//   ---     ---     ---     ---     ---

  NTCHMNG("SINx8",0);
  NTSIN(2);

  float sc[2];
  uint  ws[2];
  GTCHRSZ(sc);
  GTSCRSZ(ws);

  uint CharCount=ws[0]*ws[1];
  shbout_mx=ws[0];
  shbout_my=ws[1];

  shbout=(uint*) malloc(CharCount*sizeof(uint));
  memset(shbout,0,CharCount*sizeof(uint));

  BEGPSH();

//   ---     ---     ---     ---     ---

  while(GTCHMNGRUN()) {

    //(shbout_flg&0x01)!=0
    int evilstate=FRBEGCHMNG(0);
    if(evilstate) {
      break;

    };shbout_flg&=~0x01;

//   ---     ---     ---     ---     ---

    if(fr) {
      fr^=fr;int pid=fork();

      if(!pid) {

        char ex_path[PATH_MAX+1]={0};
        {

          int x_err=1;
          for(int x=0;x<PATH_SZ;x++) {
            strcpy(ex_path,PATH[x]);
            strcat(ex_path,"pf.exe");

            x_err=access(ex_path, X_OK);
            if(!x_err) {
              break;

            };

          };if(!x_err) {

            char* ex_argv[3]={

              ex_path,
              mh_key,
              NULL

            };

            execv(ex_argv[0],ex_argv);
            exit(-1);

          };

        };CALOUT(E,"Could not exec %s\n",ex_path);
        return -1;

//   ---     ---     ---     ---     ---

      } else {
        int wst;
        wait(&wst);
        pshout(bf,4,1);

        //pause();

      }
    } else {
      uchar* ibuff=IBUFCHMNG();
      if(ibuff) {
        pshout(ibuff,4,1);

      };
    };

//   ---     ---     ---     ---     ---

    PSHCHR(shbout,shbout_flg&0x01);

    FRENDCHMNG();
    SLEEPCHMNG();

  };

//   ---     ---     ---     ---     --- cleanup

  dlmap(mh);

  ENDPSH();
  free(shbout);

  DLSIN();
  DLCHMNG();

  for(int x=0;x<PATH_SZ;x++) {
    CALOUT(E,"%s\n" ,PATH[x]);
    free(PATH[x]);

  };free(PATH);

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

