//**************************
// J3L                     *
// avtomake                *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "SHB7/sh_gbl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <time.h>
#include <limits.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

//   ---     ---     ---     ---     ---
// #:0x0;> constants

const char* OFLG = (
  "-s -Os -fno-unwind-tables"
  "-fno-asynchronous-unwind-tables -ffast-math"
  "-fsingle-precision-constant -fno-ident"

);

const char* LFLG = (
  "-flto -ffunction-sections -fdata-sections"
  "-Wl,--gc-sections -Wl,-fuse-ld=bfd"

);

//   ---     ---     ---     ---     ---

int main(int argc,char** argv) {

  char* bf=(char*) malloc(0x1000);
  int cnt=0;

//   ---     ---     ---     ---     ---

  { char cwd[PATH_MAX+1];   // current directory path
    getcwd(cwd,PATH_MAX+1);

    if(argc>1) {
      cwd[strlen(cwd)]='/';
      strcat(cwd,argv[1]);

    };

    int rcnt=0x0800;        // start of file list
    int fcnt=0;             // start of folder list

    char pool[0x110];       // reusable charbuf
    struct stat filestat;   // file/folder info

    DIR* d=opendir(cwd);    // these give us directory
    struct dirent dr;       // info and goodies
    struct dirent* dr_ptr;

//   ---     ---     ---     ---     ---

    // iter through directory and write info to pool

    while( (dr_ptr=readdir(d)) ) {

      // we put a special char next to the name
      // this char changes depending on entry type

      char icon=(
        (dr_ptr->d_type==DT_DIR)
        ? ICO_SYS[9]
        : ICO_SYS[0]

      // it's the 'head'; put name of cwd here
      );if(!strcmp(dr_ptr->d_name,".")) {
        icon=ICO_SYS[12];
        sprintf(pool,"$:%c;> %s",icon,cwd);

      // it's the 'back' button
      } else if(!strcmp(dr_ptr->d_name,"..")) {
        icon=ICO_SYS[11];
        sprintf(pool,"$:%c;> ..",icon);

      // an actual file or subdirectory
      } else {

        // get additional data about this entry
        stat(dr_ptr->d_name,&filestat);

        sprintf(

          pool,"$:%c;> %s",
          icon,dr_ptr->d_name
          /*filestat.st_size,
          filestat.st_mtime*/

        );
      };

//   ---     ---     ---     ---     ---

      // copy pool into the corresponding offset
      // advance that offset after copying

      // folders (includes 'head' and 'back')
      if(dr_ptr->d_type==DT_DIR) {
        strcpy(bf+fcnt,pool);
        fcnt+=strlen(pool)+1;

      // files (basically everything else)
      } else {
        strcpy(bf+rcnt,pool);
        rcnt+=strlen(pool)+1;

      // then clear it!
      };cnt++;memset(pool,0,0x110);

    };
  };

//   ---     ---     ---     ---     ---

  // test: remotely change the state of the renderer

  { SHSIG* s=(SHSIG*) malloc(sizeof(SHSIG)+0x100);

    s->hed[0]=SHSIG_MSIN[0];
    s->hed[1]=SHSIG_MSIN[1];

    s->flg=0x00;

    strcpy(s->mess,"$:test;>");
    s->len=strlen(s->mess)|1;

    fwrite(
      s,sizeof(char),
      sizeof(SHSIG)+s->len,

      stdout

    );free(s);

  };

//   ---     ---     ---     ---     ---

  // now we can print the list itself;
  // directories come first, then files

  char* c=bf;
  for(int x=0;x<cnt;x++) {
    while(!*c) { c++; }
    printf("%s\n",c);
    c+=strlen(c);

  };free(bf);

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

