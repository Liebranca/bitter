#include "KVRNEL/MEM/kvr_str.h"
#include "KVRNEL/MEM/sh_map.h"

#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/stat.h>

//   ---     ---     ---     ---     ---

void main(int argc,char** argv) {

  // get bin fold
  char* b=argv[0];
  int len=strlen(b);

  while(b[len-1]!='/') {
    len--;

  };b[len]=0x00;

  char pbuff[0x100];

  strcpy(pbuff,b);
  strcat(pbuff,"makepy-cache");
  char* fpath=pbuff;

  void** mh=ntmap_origin(fpath,1);
  char* mh_key=encodemh(mh);

//   ---     ---     ---     ---     ---

  int pid;
  if(!(pid=fork())) {

    char fn[0x100];
    strcpy(fn,b);
    strcat(fn,"scpx.exe");

    //dup2(mapfd(mh),STDOUT_FILENO);

    char* ex_argv[]={fn,mh_key,NULL};
    execv(ex_argv[0],ex_argv);

    exit(-1);

//   ---     ---     ---     ---     ---

  };int wst;wait(&wst);

  int i=0;
  char c=0x00;

  char* buff=*mh;

//   ---     ---     ---     ---     ---

  // parse out
  char* fields=strtok(buff,"\n");
  char* clsdef=strtok(NULL,"\n");

  printf(
    "reg rdwr;%s\n"
    "::%s\n\n",

    fields,clsdef

  );

//   ---     ---     ---     ---     ---

  // get all funcs to wrap
  char* wrapfn_type=strtok(NULL,"\n");
  char* wrapfn_args=strtok(NULL,"\n");

  printf(
    "proc %s;\n"
    "%s\n\n",

    wrapfn_type,wrapfn_args

  );

//   ---     ---     ---     ---     ---

  dlmap(mh);

};

//   ---     ---     ---     ---     ---