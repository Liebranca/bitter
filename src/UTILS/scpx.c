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
#include "KVRNEL/MEM/sh_map.h"

#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static char* OUT=NULL;
#define WR(format,...) sprintf(OUT+strlen(OUT),format,##__VA_ARGS__)

typedef struct {

  const char* c_name;
  const char* py_name;

  // TODO: add convertion methods
  // const char* pytoc;
  // const char* ctopy;

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

  "char",   "schar",
  "short",  "sshort",
  "int",    "sint",
  "long",   "slong",

  "float*", "star(c_float)",
  "float",  "c_float"

};static const int trtok_len=(
  sizeof(trtoks)/sizeof(TRTOK)

);

//   ---     ---     ---     ---     ---

void genpy_fn(char* src) {

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

  // reset cache on new src
  if(src) {
    token=src;

  };if(!token) {return;}

//   ---     ---     ---     ---     ---

  // so this one used to be a state machine,
  // hacked this so please forgvie how messy it is

  // get substr
  char* k=NULL;
  int y=0;int valid_res=0;

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

  };if(!valid_res) {y=0;};

  type=trtoks[y].c_name;

  token+=strlen(trtoks[y].c_name);
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

  };if(!valid_res){y=0;}
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

  // make py string from parsed data
  WR(

    "%s=wrap_cfunc(LIB,%s,%s,[",
    name,name,type

  );for(int x=0;x<curarg;x++) {
    if(!args[x]) {break;}
    WR("%s,",args[x]);

  };WR("\b]);\n");

};

//   ---     ---     ---     ---     ---

void genpy_st(char* src) {

  // trackers
  int field_cnt=0;
  int level=0;
  int i=0;

  // curchar
  char c;

  const char* type=NULL;
  const char* name=NULL;
  const char* st_name=NULL;

  WR("_fields_=([");

//   ---     ---     ---     ---     ---

  // run until we consue all fields

  LOOP_HEAD:

  c=*(src+i+0);

//   ---     ---     ---     ---     ---

  switch(c) {

    case '}':
    level--;break;

    case '{':
    level++;break;

//   ---     ---     ---     ---     ---

    default:
    if(c>0x20) {

      // parse fields
      if(level) {

        field_cnt++;

        // put null on next blank char
        type=strtok(src+i," ");i+=strlen(type)+1;
        name=strtok(NULL,"\x00");i+=strlen(name)+1;

        // translate type to py
        int y=0;
        for(;y<trtok_len;y++) {
          if(strstr(type,trtoks[y].c_name)) {break;}

        };type=trtoks[y].py_name;break;
      };

//   ---     ---     ---     ---     ---

      // at least one field parse

      if(field_cnt) {

        // get struct name
        st_name=strtok(src+i,"\x00");
        i+=strlen(st_name)+1;
        goto END;

      };
    };break;

  };

//   ---     ---     ---     ---     ---

  LOOP_TAIL:

  i++;if(!type && !name) {
    goto LOOP_HEAD;

  };WR(

    "(\"%s\",%s),",
    name,type

  );name=NULL;type=NULL;

  if(level) {goto LOOP_HEAD;}

  END:WR("\b]);\nclass %s(c_struct):\n",st_name);

};

//   ---     ---     ---     ---     ---

void genpy(

  char** expressions,
  int exp_cnt,

  int mode

) {

  enum modes {BUILD_ST,BUILD_FN};

  // iter expressions
  for(int x=0;x<exp_cnt;x++) {

    // process each export block
    char* block=expressions[x];

    if(block) {

      if(mode==BUILD_ST) {
        genpy_st(block);

      } else if(mode==BUILD_FN) {
        genpy_fn(block);

      };

    // go to next exp
    };GONEXT:block=expressions[x];

  };
};

//   ---     ---     ---     ---     ---

int fprs(

  const char* fpath,
  const char* sep,

  int mode

) {

  MEM* m;

  { struct stat filestat;
    stat(fpath,&filestat);
    const unsigned int sz=filestat.st_size;
    m=MKSTR("fr",sz,1);

  };char* buff=GTSTR(m);

//   ---     ---     ---     ---     ---

  FILE* f=fopen(fpath,"r");
  if(!f) {
    printf("Can't locate <%s>\n",fpath);
    return FATAL;

  };

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
  int block_n=0;
  char* token=strtok(buff,";");

  // filter out expressions for parser
  while(token) {

    // if sep found in token
    char* k;
    if((k=strstr(token,sep))!=NULL) {

      // save token without sep
      expressions[block_n]=(char*) (

        ((uintptr_t) token)

        +( ((uintptr_t) k)
          -((uintptr_t) token) )

        +strlen(sep)

      );block_n++;

    // go to next
    };token=strtok(NULL,";");

  };

  // parse
  genpy(expressions,exp_cnt,mode);

//   ---     ---     ---     ---     ---

  // note to self: dealloc on each call is pretty
  // terrible, but this is the only clear way within
  // the current state of the core libs

  // cleanup
  free(expressions);
  fclose(f);
  DLMEM(m);

  return DONE;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

void main(int argc,char** argv) {

  // cache-write check
  void** mh=(argc>1)
    ? ntmap_heir(argv[1])
    : NULL;

  if(mh) {
    OUT=*mh;

  } else {
    OUT=malloc(0x1000);

  };

  // look at files at the provided module
  // generate a python interface for it
  int mode=0;

  const char* fpath="./utils/libs/test.h";
  const char* sep="typedef struct";

  int evil;
  if((evil=fprs(fpath,sep,mode))==FATAL) {
    return;

  };sep="EXPORT";mode=1;
  fprs(fpath,sep,mode);

  // close cache if in use
  if(mh) {dlmap(mh);return;}

  printf("%s",OUT);
  free(OUT);

};

//   ---     ---     ---     ---     ---