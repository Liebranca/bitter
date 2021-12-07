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
#include <errno.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static uint* shbout=NULL;
static char* shbout_str=NULL;

static uint shbout_str_mx=0;

static uint shbout_mx=0;
static uint shbout_my=0;

static uint shbout_x=0;
static uint shbout_y=0;

static int shbout_flg=0x00;

//   ---     ---     ---     ---     ---

void pshout(
  uchar* str,
  uchar col_id,
  char reset) {

  uint ox=shbout_x;
  uint oy=shbout_y;

  uint pos=shbout_x+(shbout_y*shbout_mx);
  do {

    if(!*str) {
      break;

    } else if(*str<0x20 && *str!=0x0A) {
      continue;

    };

    shbout[pos]=(*str)|(col_id<<8);
    pos++;shbout_x++;

    if(shbout_x>=shbout_mx
    || *str==0x0A) {
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

void pllout(uint pos) {

  uint x;char last=0x00;char c=0x00;

  for(x=0;x<pos;x++) {
    c=shbout[x]&0xFF;

    // skip repeat spaces!
    if(!(c==' ' && c==last)) {
      shbout_str[x]=c;

    };last=c;

  };shbout_str[x]=0x00;

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

    // make search path array
    };PATH[0]=malloc(strlen(cd)*sizeof(char)+2);

    // add cwd ;>
    strcpy(PATH[0],cd);
    char* lach=PATH[0]+(strlen(cd)-1);

    if(*lach!='/') {
      *(lach+1)='/';
      *(lach+2)=0x00;

    };

//   ---     ---     ---     ---     ---

    // iter through the user's $PATH
    // append each string to the array

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

  void** mh;

  {

    // set outfile as an abspath

    // why? because it doubles as shared memory
    // we don't just need to know where it is
    // every child process needs to know as well

    char fpath[PATH_MAX+1+8];

    strcpy(fpath,PATH[0]);  // set cwd
    strcat(fpath,"shb7wt"); // cat cwd+outfile

    // now make an mmapd file ;>
    mh=ntmap_origin(fpath,1);

  };

  // shmem is a stdout of sorts;
  // write here and it'll be printed

  shbout=(uint*) (*mh);
  int* shbuf=(int*) (shbout+0x1000);

  char* mh_key=encodemh(mh);

  // mh key is needed solely as a brute-force way of
  // telling a child process about the shmem

  // don't need shmem in your child?
  // then you don't need the key

  signal(SIGCONT,oncont);

  int spwn_pid=fork();
  if(!spwn_pid) {

    shbout_str_mx=sysconf(ARG_MAX);
    shbout_str=(char*) malloc(
      shbout_str_mx*sizeof(char)

    );STANDBY:

    CALOUT(E,"BACK TO SLEEP\n");
    pause();
    CALOUT(E,"OUTTA PAUSE\n");

    if(!(shbuf[0]&0x01)) {

      pllout((uint) shbuf[1]);

      goto SPWNEX;

    };free(shbout_str);
    goto SPWNFRE;

  };

//   ---     ---     ---     ---     --- window init

  // create the window manager
  // that in turn spawns a window!

  NTCHMNG("SHB7x64",0);

  // *then* init the renderer
  // this is simply so there is an OpenGL context
  // before the system is even able to try and draw
  // honestly? you could flip them around
  // i init in this order because it makes sense to me

  NTSIN(2);

  // get charsize as a value between {0.0f,1.0f}
  // this value is relative to screen size

  // why? basis: the screen is a grid of characters
  // you want to know the percentage of the screen
  // taken up by each one of those

  float sc[2];GTCHRSZ(sc);

  // related to charsize, this gives you how many
  // characters fit in the screen on a given direction
  // x gives you the number of columns
  // y is number of rows

  uint  ws[2];GTSCRSZ(ws);

  // now multiply those, and you know how many
  // characters total a single screen can have

  uint CharCount=ws[0]*ws[1];
  shbout_mx=ws[0];shbout_my=ws[1];

  BEGPSH();pshout("$:",4,0);

//   ---     ---     ---     ---     --- loop head

  // the window manager will know when to exit
  while(GTRUN()) {

    // frame start is very important!

    // based on the value passed to frbeg, the manager
    // can speed up or slow down the program

    // this translates to longer or shorter sleep time
    // so mind this arg -- it controls everything

    // for now we only care if out has been written to

    int evilstate=FRBEG(
      ((shbout_flg&0x01)!=0)*4

    );if(evilstate) {
      break;

    };shbout_flg&=~0x01;

//   ---     ---     ---     ---     --- child exec

    if(!spwn_pid) {

      // TODO:move this block to it's own process
      // that might fix the e1455 pop

      SPWNEX:int pid=fork();

      if(pid<0) {
        CALOUT(E,"\n\n%s\n",strerror(errno));
        exit(-1);

      };

      // grandchild: locate requested
      // that means iter through the search path

      if(!pid) {

        char ex_path[PATH_MAX+1]={0};
        {

          uint ex_argc=2;uint x=0;
          char c=*(shbout_str+x);

          // we need to read the arguments
          // count them first...

          while(c) {

            if(c==0x20) {
              ex_argc++;

            };x++;c=*(shbout_str+x);
          };

          CALOUT(E, "%s\n",shbout_str);

//   ---     ---     ---     ---     ---

          // now alocate a strarr and tokenize!

          char** ex_argv=malloc(
            ex_argc*sizeof(char*)

          );ex_argv[0]=(ex_path+0);

          x=1;char* token=strtok(shbout_str," ");
          strcpy(ex_argv[0],PATH[x]);
          strcat(ex_argv[0],token);

          CALOUT(E, "%s\n",ex_argv[0]);

          token=strtok(NULL," ");

          for(;x<(argc-1);x++) {
            ex_argv[x]=(char*) malloc(
              strlen(token)+1

            );strcpy(ex_argv[x],token);
            token=strtok(NULL," ");
            CALOUT(E, "%s\n",ex_argv[x]);

          };

//   ---     ---     ---     ---     ---

          int x_err=1;
          for(int x=0;x<PATH_SZ;x++) {

            x_err=access(ex_path, X_OK);

            if(!x_err) {    // break if found && valid
              break;

            };

//   ---     ---     ---     ---     ---

          };if(!x_err) {    // then execute

            // the exit line should never happen
            // TODO: handle that error

            //dup2(mapfd(mh),STDOUT_FILENO);

            execv(ex_argv[0],ex_argv);
            exit(-1);

          };

        // or just print something vague, whatever
        };CALOUT(E,"Could not exec %s\n",ex_path);
        return -1;

//   ---     ---     ---     ---     --- parent wait

      } else {

        // TODO:look into another way of locking...
        // we will need to pause and resume childs
        // at some point

        int wst;wait(&wst);
        goto STANDBY;

        // TODO:don't just push the shared memory!
        // this is okay for testing trivial programs
        // not so pretty for anything else

        //pshout(bf,4,1);

      }

//   ---     ---     ---     ---     --- parent exec

    } else {

      // normal execution: check for text input
      uchar* ibuff=GTIBUF();
      if(ibuff) {

        pshout(ibuff,5,0);
        if(ibuff[strlen(ibuff)-1]==0x0A) {
          shbuf[1]=shbout_x+(shbout_y*shbout_mx);
          kill(spwn_pid,SIGCONT);

        };

      };CLIBUF();
    };

//   ---     ---     ---     ---     --- loop tail

    // draw the screen buffer, it's done in one go!

    // how many chars you have is irrelevant, so
    // large prints are insanely cheap

    // do note that the buffer is only updated
    // when shbout is written to, and so if you
    // don't register a write then the previous
    // frame is drawn instead, repeating right
    // until you set the flag

    PSHCHR(shbout,shbout_flg&0x01);

    // these two must always go at the very bottom!
    // order matters: one gets current time, the
    // other calculates frame delta

    FREND();
    FRSLP();

  };

//   ---     ---     ---     ---     ---

  if(spwn_pid) {
    shbuf[0]|=0x01;
    kill(spwn_pid,SIGCONT);
    int wst;wait(&wst);

    ENDPSH();

    DLSIN();
    DLCHMNG();
    dlmap(mh);

  };

  SPWNFRE:

  for(int x=0;x<PATH_SZ;x++) {
    free(PATH[x]);

  };free(PATH);

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

