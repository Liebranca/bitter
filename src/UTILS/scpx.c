//**************************
// SCPX                    *
// handles peso escapes    *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "KVRNEL/MEM/kvr_str.h"

#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

typedef struct {

  const char* c_name;
  const char* py_name;

} TRTOK; static TRTOK trtoks[]={

  "void*",  "void_p",
  "void",   "None",
  "char**", "star(charstar)",
  "char*",  "charstar",
  "size_t", "size_t",

  "uchar",  "uchar",
  "ushort", "ushort",
  "uint",   "uint",
  "ulong",  "ulong",

  "char",   "char",
  "short",  "short",
  "int",    "int",
  "long",   "long",

  "float*", "star(c_float)"
  "float",  "c_float",

};static const int trtok_len=(
  sizeof(trtoks)/sizeof(TRTOK)

);

//   ---     ---     ---     ---     ---

int genpy_rd(char* new_src) {

  // token cache
  char* token=NULL;
  size_t len=0;

  const char* type=NULL;
  char* name=NULL;
  const char* args[16]={NULL};

  // state cache
  enum states {
    sf_type, sf_name,sf_args,sf_end

  };uint64_t state=sf_type;
  size_t curarg=0;

//   ---     ---     ---     ---     ---

  // reset cache on new_src
  if(new_src) {
    token=new_src;

  };if(!token) {return 1;}

//   ---     ---     ---     ---     ---

  // so this one used to be a state machine,
  // hacked this so please forgvie how messy it is

  // get substr
  char* k=NULL;
  int y=0;int valid_res;

  k=token;

  do {if(!*k) {break;}
  } while((*k++)!='{');

  *k=0x00;

//   ---     ---     ---     ---     ---

  // copy return type
  type=token;

  for(y=0;y<trtok_len;y++) {

    // check token against keywords
    if((k=strstr(type,trtoks[y].c_name))
    != NULL) {valid_res=1;break;};

  };if(valid_res) {
    type=trtoks[y].c_name;

  };token+=strlen(trtoks[y].c_name);
  *token=0x00;token++;

//   ---     ---     ---     ---     ---

  // copy name

  do { if(*token!=' ') {break;};
  } while(*token++);

  k=token;
  name=token;do {

    if(*k=='(' || *k==' ') {
      *k=0x00;k++;

    }
  } while(*k++);

//   ---     ---     ---     ---     ---

  token=name+strlen(name)+1;
  k=token;

  state=sf_args;

  goto CPYARG;

//   ---     ---     ---     ---     ---

  // find first arg
  ARGSLOOP:
  token=k;

  do {char c=*k;
    if(!c || c==' ') {
      break;

    };

  } while(*k++);
  *k=0x00;k++;

//   ---     ---     ---     ---     ---

  valid_res=0;
  CPYARG:

  // parse args
  for(y=0;y<trtok_len;y++) {

    // check token against keywords
    if((strstr(token,trtoks[y].c_name))
    != NULL) {valid_res=1;break;};

  };/*if(!valid_res){break;}*/
  args[curarg]=trtoks[y].py_name;

  curarg++;

//   ---     ---     ---     ---     ---

  do {char c=*k;

    // delimiter check
    if(c==',') {
      break;

    // delimiter end
    } else if(c==')') {
      state=sf_end;
      break;

    };continue;

  } while(*k++);
  *k=0x00;k++;

//   ---     ---     ---     ---     ---

  // loop back if end not set
  if(state!=sf_end) {goto ARGSLOOP;}

  // function fully processed
  SF_END:
  state=sf_type;
  curarg=0;
  token=NULL;

//   ---     ---     ---     ---     ---

  // make py string from parsed data
  printf(

    "wrap_cfunc(LIB,%s,%s,[",
    name,type

  );for(int x=0;x<curarg;x++) {
    printf("%s,",args[x]);

  };printf("\b\b]);\n");

};

//   ---     ---     ---     ---     ---

void genpy(char** expressions,int exp_cnt) {

  // iter expressions
  int beg=1;

  for(int x=0;x<exp_cnt;x++) {

    // process each export block
    char* block=expressions[x];

    if(block) {
      beg=genpy_rd(block);

    // go to next exp
    };GONEXT:block=expressions[x];

  };
};

//   ---     ---     ---     ---     ---

void main(int argc,char** argv) {

  const char* fpath=
    "d:/lieb_git/kvr/src/blkmgk/__blkmgk.c";

  MEM* m;

  { struct stat filestat;
    stat(fpath,&filestat);
    const unsigned int sz=filestat.st_size;
    m=MKSTR("fr",sz,1);

  };char* buff=GTSTR(m);

//   ---     ---     ---     ---     ---

  FILE* f=fopen(fpath,"r");

  int rb=fread(
    buff,sizeof(char),
    m->bsize,f

  );

//   ---     ---     ---     ---     ---

  // make a container
  int exp_cnt=0;
  char** expressions;{

    // count number of expressions
    for(char* c=buff+0;c<buff+rb;c++) {
      exp_cnt+=*c==';';

    };

    // allocate exp_cnt char pointers
    expressions=
      (char**) malloc(exp_cnt*sizeof(char*));

  };

//   ---     ---     ---     ---     ---

  // get first token
  const char* sep="EXPORT";

  int block_n=0;
  char* token=strtok(buff,";");

//   ---     ---     ---     ---     ---

  // parse

  while(token) {

    // if sep found in token

    char* k;
    if((k=strstr(token,sep))!=NULL) {

      // save token after sep
      expressions[block_n]=(char*) (

        ((uintptr_t) token)

        +( ((uintptr_t) k)
          -((uintptr_t) token) )

        +strlen(sep)

      );block_n++;

    // go to next
    };token=strtok(NULL,";");

  };

  // process saved expressions
  genpy(expressions,exp_cnt);

//   ---     ---     ---     ---     ---

  // cleanup
  free(expressions);
  fclose(f);
  DLMEM(m);

};

//   ---     ---     ---     ---     ---
// #:0x1;>

