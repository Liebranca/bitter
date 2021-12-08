//**************************
// SPWN                    *
// forker                  *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "KVRNEL/MEM/sh_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>

// make shb7 && gcc -s -Os -fno-unwind-tables -fno-asynchronous-unwind-tables -fsingle-precision-constant -ffast-math -fno-ident -flto -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,-fuse-ld=bfd -I/cygdrive/d/lieb_git/kvr/src/ ./spwn.c -L/cygdrive/d/lieb_git/kvr/bin/x64/ -lkvrnel -o /cygdrive/d/lieb_git/kvr/bin/x64/spwn.exe

static uint* shbout=NULL;
static char* shbout_str=NULL;
static uint shbout_str_mx=0;

void pllout(uint pos) {

  uint x;uint y=0;
  char last=0x00;char c=0x00;

  for(x=0;x<pos;x++) {
    c=shbout[x]&0xFF;

    if(c<0x20) {
      continue;

    // skip repeat spaces!
    } else if(!(c==' ' && c==last)) {
      shbout_str[y]=c;y++;

    };last=c;

  };shbout_str[y]=0x00;

};

void oncont(int sig) {
  return;

};

//   ---     ---     ---     ---     ---
// #:0x0;>

int main(int argc,char** argv) {

  // use origin key to find mmapd file
  void** mh=ntmap_heir(argv[1]);

  if(!mh) {
    printf("spwn could not decipher origin key\n");
    kill(getppid(),SIGCONT);

    return -1;

  };

  shbout=(uint*) (*mh);
  int* shbuf=(int*) (shbout+0x0400);

  signal(SIGCONT,oncont);
  kill(getppid(),SIGCONT);

//   ---     ---     ---     ---     ---

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

    char pwd[PATH_MAX+1];  // find current directory
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

  shbout_str_mx=sysconf(ARG_MAX);
  shbout_str=(char*) (shbout+0x0404);

  STANDBY:

//   ---     ---     ---     ---     ---

  printf("BACK TO SLEEP\n");
  pause();

  printf("OUTTA PAUSE\n");
  pllout((uint) shbuf[1]);

  if(!(shbuf[0]&0x01)) {

    if(!strlen(shbout_str)) {
      goto SPWNSKIP;

    };

    SPWNEX:int pid=fork();

    if(pid<0) {
      printf("\n\n%s\n",strerror(errno));
      goto SPWNSKIP;

    };

//   ---     ---     ---     ---     ---

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

        printf("%s\n",shbout_str+2);

//   ---     ---     ---     ---     ---

        // now alocate a strarr
        char* ex_argv[256]={ex_path+0};

        // first token should be some fpath
        char* token=strtok(shbout_str+2," ");

        // ^validate
        int x_err=1;
        for(x=0;x<PATH_SZ;x++) {

          strcpy(ex_path,PATH[x]);
          ex_path[strlen(PATH[x])]=0x00;

          strcat(ex_path,token);
          x_err=access(ex_path, X_OK);

          if(!x_err) {      // break if found && valid
            break;

          };
        };if(!x_err) {      // else abort
          goto EXEFAIL;

        };x=1;

        ex_argv[0]=ex_path;
        printf("%s\n",ex_argv[0]);

//   ---     ---     ---     ---     ---

        char* ex_base=(
          shbout_str+strlen(shbout_str)+1

        );token=strtok(NULL," ");

        for(;x<(argc-1);x++) {
          ex_argv[x]=ex_base;
          strcpy(ex_argv[x],token);
          ex_base+=(strlen(token)+1);

          token=strtok(NULL," ");
          printf("%s\n",ex_argv[x]);

        };ex_argv[argc-1]=NULL;

//   ---     ---     ---     ---     ---

        // the exit line should never happen
        // TODO: handle that error

        //dup2(mapfd(mh),STDOUT_FILENO);

        execv(ex_argv[0],ex_argv);
        exit(-1);

      };EXEFAIL:
      printf("Could not exec %s\n",ex_path);
      return -1;

//   ---     ---     ---     ---     --- spawner wait

    } else {

      printf("WAITING FOR GRANDCHILD...\n");
      int wst;wait(&wst);

      SPWNSKIP:

      shbuf[0]&=~0x02;
      kill(getppid(),SIGCONT);

      goto STANDBY;

    };

  };

//   ---     ---     ---     ---     --- cleanup

  for(int x=0;x<PATH_SZ;x++) {
    free(PATH[x]);

  };free(PATH);
  dlmap(mh);

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

