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

  int* rd_values=*mh;
  char* buff=(char*) (rd_values+4);

//   ---     ---     ---     ---     ---

  int st_cnt=rd_values[0];
  int fn_cnt=rd_values[1];

  // parse out
  MEM* sections=sepstr_g(buff, "\n");
  { char** arr=MEMBUFF(sections,char*,0);

    // get all structs
    for(int x=0;x<st_cnt;x++) {

      // get first field
      char* field_name=strtok(*arr,":");
      char* field_type=strtok(NULL,",");

      //iter
      while(field_name && field_type) {

        printf("%s:%s\n",field_name,field_type);

        // next set
        field_name=strtok(NULL,":");
        field_type=strtok(NULL,",");

      };arr++;

      char* st_name=*arr;arr++;
      printf("%s\n\n",st_name);

    };

//   ---     ---     ---     ---     ---

    char* decl=malloc(0x1000);
    char* head=malloc(0x1000);
    char* call=malloc(0x1000);

    // get all functions
    for(int x=0;x<fn_cnt;x++) {

      // get first func to wrap
      char* fn_type=strtok(*arr,":");
      char* fn_name=strtok(NULL,"\x00");arr++;

      char* fn_args=*arr++;

//   ---     ---     ---     ---     ---

      // translate up to args

      sprintf(decl,
        "_%s=wrap_cfunc("

        "%s,\"%s\","
        "%s,[",

        fn_name,"test",fn_name,fn_type

      );sprintf(head,

        "def %s(",
        fn_name

      );sprintf(call,

        "  _%s(",fn_name

      );

//   ---     ---     ---     ---     ---

      // get first arg
      char* ar_type=strtok(fn_args,":");
      char* ar_name=strtok(NULL,",");

      // iter
      size_t len=strlen(decl);
      char* decl_base=decl+len;

      while(ar_type && ar_name) {

        char e_buff[0x200]={0};
        sprintf(e_buff,"%s(%s)",ar_type,ar_name);

        strcpy(call+strlen(call),e_buff);

        strcpy(head+strlen(head),ar_name);
        strcpy(decl+len,ar_name);

        len+=strlen(ar_name);
        decl[len]=',';
        head[strlen(head)]=',';
        call[strlen(call)]=',';

        ar_type=strtok(NULL,":");
        ar_name=strtok(NULL,",");

        len=strlen(decl);
      };

//   ---     ---     ---     ---     ---

      len=strlen(decl);
      decl[len-1]=']';
      decl[len+0]=')';
      decl[len+1]=';';
      decl[len+2]=0x00;

      len=strlen(head);
      head[len-1]=')';
      head[len+0]=':';
      head[len+1]=0x00;

      len=strlen(call);
      call[len-1]=')';
      call[len+0]=';';
      call[len+1]=0x00;

      printf("%s\n",decl);
      printf("%s\n",head);
      printf("%s\n",call);

      memset(decl,0,0x1000);
      memset(head,0,0x1000);
      memset(call,0,0x1000);

//   ---     ---     ---     ---     ---

    };free(call);
    free(head);
    free(decl);

  };DLMEM(sections);

//   ---     ---     ---     ---     ---

  dlmap(mh);

};

//   ---     ---     ---     ---     ---