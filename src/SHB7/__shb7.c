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
    mh=ntmap_origin(fpath,4);

  };char* bf=(char*) (*mh); // our shared memory
  char* mh_key=encodemh(mh);// the encoded shmem

  // mh key is needed solely as a brute-force way of
  // telling a child process about the shmem

  // don't need shmem in your child?
  // then you don't need the key

  signal(SIGCONT,oncont);
  int fr=1;// just for testing

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

  // basically stdout;
  // write here and it'll be printed

  shbout=(uint*) malloc(CharCount*sizeof(uint));
  memset(shbout,0,CharCount*sizeof(uint));

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

    if(!fr) {
      fr^=fr;int pid=fork();

      // child: locate requested
      // that means iter through the search path

      if(!pid) {

        char ex_path[PATH_MAX+1]={0};
        {

          int x_err=1;
          for(int x=0;x<PATH_SZ;x++) {
            strcpy(ex_path,PATH[x]);
            strcat(ex_path,"pf.exe");

            x_err=access(ex_path, X_OK);

            if(!x_err) {    // can execute? then do
              break;

            };

          };if(!x_err) {    // else don't

            char* ex_argv[3]={

              ex_path,
              mh_key,
              NULL

            };

            // the exit line should never happen
            // TODO: handle that error

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

        // TODO:don't just push the shared memory!
        // this is okay for testing trivial programs
        // not so pretty for anything else

        pshout(bf,4,1);

      }

//   ---     ---     ---     ---     --- parent exec

    } else {

      // normal execution: check for text input
      uchar* ibuff=GTIBUF();
      if(ibuff) {
        pshout(ibuff,5,0);

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

