/* rdst.mny

$:db;>$$$$$$$$$$

%iter arg;
  type>con:name,;

/iter;

iter name==

char** name=CHAR_P_MEM+i;
size_t x=0;

name;

*/

//   ---     ---     ---     ---     ---

#include "KVRNEL/MEM/kvr_str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//   ---     ---     ---     ---     ---

#define isletter(c) \
  (  ((c)<='Z' && (c)>='A') \
  || ((c)<='z' && (c)>='a') )

#define isspace(c) \
  (  (c)==' '  || (c)=='\n' \
  || (c)=='\b' || (c)=='\t' )

#define rdbuff_sz 0x40

//   ---     ---     ---     ---     ---

typedef struct {

  char* format;
  int symbol_cnt;

  

} ch_stark;

static MEM* scopes[rdbuff_sz]={NULL};
static size_t scopes_i=0;

//   ---     ---     ---     ---     ---
typedef struct {

  const char* key;
  ch_stark mny;

} MNYK;static const MNYK money_keys[]={

  "%iter",&scp_iter,"%s %s"
  "/iter",&cscp_iter

};ch_stark find_money_keys(char* key) {

  for(

    int x=0;

    x<sizeof(money_keys)
    /sizeof(money_keys[0]);

    x++

  ) {

    if(!strcmp(key,money_keys[x].key)) {
      return money_keys[x].mny;

    };

  };return NULL;

};

//   ---     ---     ---     ---     ---

MEM* opscp(void) {

  scopes[scopes_i]=MKSTR("$BLK",0x4000,1);
  MEM* m=scopes[scopes_i];

  scopes_i++;

};

void clscp(void) {
  scopes_i--;DLMEM(scopes[scopes_i]);

};

//   ---     ---     ---     ---     ---

void mkpat(char* s) {

  size_t sz;
  size_t i;

  // reserve space for strings
  char name_mem[rdbuff_sz]={0x00};
  char sep_mem[rdbuff_sz]={0x00};

  // ^ ptrs for convenience
  char* name=name_mem+0;
  char* sep=sep_mem+0;

  // ^ idex, same reason
  size_t name_i=0;
  size_t sep_i=0;

  // inplace writing backtrack
  char* s_cpy=s;
  char* ptr="NULL";

//   ---     ---     ---     ---     ---

  // switches
  enum states {SYMBOL,SEPARATOR};
  int state=SYMBOL;

  do {

    // pick next symbol
    char c=*s;
    if(isletter(c)) {

      if(state==SEPARATOR) {
        state=SYMBOL;

        *(sep+sep_i)=0x00;
        printf("\"%s\");\n",sep);

        sep_i=0;

      };*(name+name_i)=c;
      name_i++;

//   ---     ---     ---     ---     ---

    // pick next separator
    } else {

      if(state==SYMBOL) {
        state=SEPARATOR;

        *(name+name_i)=0x00;
        printf("char* %s=strtok(%s,",name,ptr);

        name_i=0;

      };*(sep+sep_i)=c;
      sep_i++;

    };sz++;

  } while(*s++);

//   ---     ---     ---     ---     ---

  *(sep+sep_i)=0x00;
  printf("\"%s\");\n",sep);

  sep_i=0;

};

void mkhed(char* s) {

  char* token=strtok(s," ");
  ch_stark mny=find_money_keys(token);

  mny(s+strlen(token)+1);

};

//   ---     ---     ---     ---     ---

char* next(char* s) {

  if(!*s) {return NULL;}

  // skip meaningless pad
  while(isspace(*s)) {
    s++;

  };

  // find end of expression
  char* end=s;
  while(*end!=';') {
    end++;

  };

//   ---     ---     ---     ---     ---

  // trackers
  enum states {PESO,SEC,MONEY};
  int state=PESO;

  int len=strlen(end);
  char* exp;

  // verify sequence type
  if(len>1) {

    // escape
    if(*(end+1)=='>'
    && *((ushort*) s)==0x3A24) {
      state=MONEY;end++;
      exp=strtok(s+2,";>");

    // regular
    } else {
      exp=strtok(s,";");

    };

  // null or invalid
  } else {
    return end+1;

  };

//   ---     ---     ---     ---     ---

  switch(state) {
    case MONEY:
    mkhed(exp);
    break;

    case PESO:
    mkpat(exp);
    break;

    default:break;


  };return end+1;

};

//   ---     ---     ---     ---     ---

void main(int argc,char** argv) {

  char buff[0x400];
  char* s=buff+0;

  memset(s,0,0x400);
  strcpy(s,

    "$:%iter arg \'\\n';>"
    "  type>con:name,;"

    "$:/iter;>"

  );while(*s) {s=next(s);}

  /* generate this...

  char* name=strtok(s," ");

  char* arg_type=strtok(NULL, ">");
  char* arg_con=strtok(NULL, ":");
  char* arg_name=strtok(NULL, ",");*/

};