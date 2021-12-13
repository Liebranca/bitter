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

#include <inttypes.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <limits.h>
#include <errno.h>

//   ---     ---     ---     ---     ---

#define KVR_PATH_SEP '\xFF'
#define NIX_PATH_SEP ':'
#define WIN_PATH_SEP ';'

//   ---     ---     ---     ---     ---

static uint* shbout=NULL;
static char* shbout_str=NULL;
static uint shbout_str_mx=0;

static char** PATH;
static int PATH_SZ=1;

//   ---     ---     ---     ---     ---

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
  for(uint i=2;i<SHM_STEP;i++) {
    shbout[i]=0x00E2;

  };
};

//   ---     ---     ---     ---     ---

void cd(void) {

  char* cd=PATH[0];
  char cd_old[PATH_MAX+1];
  strcpy(cd_old,cd);

  int len=strlen(cd);
  char* c=shbout_str+5;

//   ---     ---     ---     ---     ---

  // cd =<path> for chaging root into absolute

  if(*c=='=') {
    c++;memset(cd,0,strlen(cd));
    strcpy(cd,c);

    goto SETCWD;

  };

//   ---     ---     ---     ---     ---

  while(*c) {

    ushort nc=*((ushort*) c);
    len=strlen(cd);

    // go back on '..'
    if(nc==0x2E2E) {

      len--;

      // do not go back further on "/<fold>/"
      // we assume that's a root dir

      { int do_brk=0;
        for(char* cd_c=cd+0;cd_c<cd+len;cd_c++) {
          do_brk+=*cd_c=='/';

        };if(do_brk<=2) { c+=2;continue; }
      };

//   ---     ---     ---     ---     ---

      // now simply go back until a seppy is found

      cd[len]=0x00;
      while(cd[len]!='/') {
        cd[len]=0x00;
        len--;

      };c++;

//   ---     ---     ---     ---     ---

    // else append to cd...
    // but avoid duplicate seppy

    } else if(
        !(*c=='/' && cd[len-1]=='/')

      ) {
      cd[len]=*c;len++;

    };c++;
  };

//   ---     ---     ---     ---     ---

  
  { SETCWD:
    struct stat d;

    // check that the new dir exists
    // if so, change dir

    if( (!stat(PATH[0],&d) )
    &&  (d.st_mode & S_IFDIR) ) {

      len=strlen(PATH[0])-1;
      if(PATH[0][len]!='/') {
        len+=PATH[0][len]!=0x00;
        PATH[0][len]='/';
        PATH[0][len+1]=0x00;

      };chdir(PATH[0]);

    // else assume invalid && revert to previous

    } else {
      printf("Invalid path:\n <%s>\n\n",cd);
      memset(cd,0,strlen(cd));
      strcpy(PATH[0],cd_old);

    };
  };

//   ---     ---     ---     ---     ---

  printf("%s",PATH[0]);

};

//   ---     ---     ---     ---     ---

void pwd(void) {
  printf("%s",PATH[0]);

};

//   ---     ---     ---     ---     ---

typedef struct {

  char  name[32];
  void (*fun)(void);

} SHB7_BI;

static SHB7_BI SHB7_BUILTINS[]={
  "cd",&cd,
  "pwd",&pwd

};

//   ---     ---     ---     ---     ---

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

//   ---     ---     ---     ---     ---

  shbout=(uint*) *mh;
  int* shbuf=(int*) (shbout+SHM_STEP);

  signal(SIGCONT,oncont);
  kill(getppid(),SIGCONT);

//   ---     ---     ---     ---     ---

  {

    // make search path array
    // PATH[0] is always cwd

    char* path=getenv("PATH");
    int x=0;

    { int drive_ch=0;
      char c=*(path+x);

      while(c) {

        if(

          ( c==NIX_PATH_SEP
         || c==WIN_PATH_SEP
         || c==KVR_PATH_SEP )

         && drive_ch>1) {
          path[x]=KVR_PATH_SEP;
          PATH_SZ++;drive_ch=0;

        };x++;drive_ch++;c=*(path+x);
      };
    };

//   ---     ---     ---     ---     ---

    // make search path array
    PATH=malloc(PATH_SZ*sizeof(char*));

    // get current directory
    char pwdv[PATH_MAX+1];
    char* cd=pwdv+0;
    getcwd(pwdv,PATH_MAX+1);

//   ---     ---     ---     ---     ---

    // sanitize...
    for(int y=0;y<strlen(cd);y++) {
      if(cd[y]=='\\') {
        cd[y]='/';

      };
    };

    { int len=strlen(cd);
      cd[len]='/';cd[len+1]=0x00;

    };

    // add cwd to arr ;>
    PATH[0]=cd;

//   ---     ---     ---     ---     ---

    // iter through the user's $PATH
    // append each string to the array

    char kvr_path_sep[2]={KVR_PATH_SEP,0x00};
    char* token=strtok(path,kvr_path_sep);
    char* lach;

    for(x=1;x<PATH_SZ;x++) {
      PATH[x]=malloc(strlen(token)*sizeof(char)+2);
      strcpy(PATH[x],token);

      for(int y=0;y<strlen(PATH[x]);y++) {
        if(PATH[x][y]=='\\') {
          PATH[x][y]='/';

        };
      };

//   ---     ---     ---     ---     ---

      lach=PATH[x]+(strlen(token)-1);

      if(*lach!='/') {
        *(lach+1)='/';
        *(lach+2)=0x00;

      };

      token=strtok(NULL,kvr_path_sep);

    };
  };

//   ---     ---     ---     ---     ---

  shbout_str_mx=sysconf(ARG_MAX);
  shbout_str=(char*) (shbuf+0x04);

  uint outjmp=(
   +(SHM_STEP*sizeof(*shbout))
   +(SHM_STEP*sizeof(*shbuf))

//   ---     ---     ---     ---     ---

  );STANDBY:                // rewind out and wait

  //rewind(stdout);
  fseek(stdout,0,SEEK_CUR);
  fseek(stdout,outjmp,SEEK_SET);
  fseek(stdout,0,SEEK_CUR);

  getcwd(PATH[0],PATH_MAX+1);
  PATH[0][strlen(PATH[0])]='/';

  pause();

//   ---     ---     ---     ---     ---

  if(!(shbuf[0]&0x01)) {    
    pllout((uint) shbuf[1]);// decode input

    // input is empty?
    // back to standby

    if(!strlen(shbout_str)) {
      goto SPWNSKIP;

    };

//   ---     ---     ---     ---     ---

    // look for builtins...

    {

      char* token=strtok(shbout_str+2," ");

      for(
        int x=0;

        x<sizeof(SHB7_BUILTINS)/sizeof(SHB7_BI);
        x++) {

        SHB7_BI* bi=SHB7_BUILTINS+x;
        if(!strcmp(token,bi->name)) {
          bi->fun();goto SPWNSKIP;

        };
      };
    };

//   ---     ---     ---     ---     ---

    SPWNEX:int pid=fork();

    // fork error?
    // back to standby

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

        uint ex_argc=0;uint x=0;
        char c=*(shbout_str+x);

        // we need to read the arguments
        // count them first...

        while(c) {
          if(c==0x20) {
            ex_argc++;

          };x++;c=*(shbout_str+x);
        };

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

//   ---     ---     ---     ---     ---

          if(!x_err) {      // break if found && valid
            break;

          };
        };if(x_err) {       // else abort
          goto EXEFAIL;

        };x=1;

        ex_argv[0]=ex_path;

//   ---     ---     ---     ---     ---

        char* ex_base=(
          shbout_str+strlen(shbout_str)+1

        );token=strtok(NULL," ");

        for(;x<(1+ex_argc);x++) {
          ex_argv[x]=ex_base;
          strcpy(ex_argv[x],token);
          ex_base+=(strlen(token)+1);

          token=strtok(NULL," ");

        };ex_argv[1+ex_argc]=NULL;

//   ---     ---     ---     ---     ---

        // the exit line should never happen
        // TODO: handle that error

        execv(ex_argv[0],ex_argv);
        exit(-1);

      };EXEFAIL:
      printf("Could not run %s\n",ex_path);
      exit(-1);

//   ---     ---     ---     ---     --- spawner wait

    } else {

      int wst;wait(&wst);

      SPWNSKIP:

      shbuf[0]&=~0x02;
      kill(getppid(),SIGCONT);

      goto STANDBY;

    };

  };

//   ---     ---     ---     ---     --- cleanup

  for(int x=1;x<PATH_SZ;x++) {
    free(PATH[x]);

  };free(PATH);
  dlmap(mh);

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

