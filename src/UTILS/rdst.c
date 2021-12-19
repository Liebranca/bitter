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
#define wrbuff_sz 0x400

//   ---     ---     ---     ---     ---
// header blocks generator

typedef struct {

  char* format;
  int order[4];

  char mem[wrbuff_sz];

} HBG;typedef struct {

  const char* format;
  const int order[4];

} HBG_nomem;

static HBG scopes[rdbuff_sz]={0};
static size_t scopes_i=0;

//   ---     ---     ---     ---     --
// keyword/callback table

typedef struct {

  const char* key;
  HBG_nomem mny;

} MNYK;static const MNYK money_keys[]={

  "%iter",
    "char* %s=strtok(%s,\"%s\");\n",
    0,1,2,0,

  "/iter",
    "$:SCOPE_KILL;>",
    0,0,0,0

};const HBG_nomem* find_money_keys(char* key) {

//   ---     ---     ---     ---     ---
// find method is linear search
// TODO:replace by zjc hash table

  for(

    int x=0;

    x<sizeof(money_keys)
    /sizeof(money_keys[0]);

    x++

  ) {

    if(!strcmp(key,money_keys[x].key)) {
      return &(money_keys[x].mny);

    };

  };return NULL;

};

//   ---     ---     ---     ---     ---
// open/close generator steps

HBG* opscp(const HBG_nomem* src) {

  HBG* h=scopes+scopes_i;
  scopes_i++;

  memset(h->mem,0,wrbuff_sz);

  h->format=src->format;

  h->order[0]=src->order[0];
  h->order[1]=src->order[1];
  h->order[2]=src->order[2];

  return h;

};HBG* clscp(void) {

  scopes_i--;
  return scopes+scopes_i;

//   ---     ---     ---     ---     ---
// generator write all steps

};void onscp(char** symbols) {

  for(int x=0;x<scopes_i;x++) {

    HBG* h=scopes+x;

    sprintf(
      h->mem+strlen(h->mem),
      h->format,

      symbols[h->order[0]],
      symbols[h->order[1]],
      symbols[h->order[2]]

    );printf("%s\n",h->mem);
    memset(h->mem,0,wrbuff_sz);

  };
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

        printf(
          "char* %s=strtok(%s"
          ",\"%s\");\n",

          name,ptr,sep

        );sep_i=0;

        char* symbols[]={name,ptr,sep};
        onscp(symbols);

      };*(name+name_i)=c;
      name_i++;

//   ---     ---     ---     ---     ---

    // pick next separator
    } else {

      if(state==SYMBOL) {
        state=SEPARATOR;
        *(name+name_i)=0x00;

        name_i=0;

      };*(sep+sep_i)=c;
      sep_i++;

    };sz++;

  } while(*s++);

//   ---     ---     ---     ---     ---

  *(sep+sep_i)=0x00;

  printf(
    "char* %s=strtok(%s"
    ",\"%s\");\n",

    name,ptr,sep

  );sep_i=0;

  char* symbols[]={name,ptr,sep};
  onscp(symbols);

};

//   ---     ---     ---     ---     ---

void mkhed(char* s) {

  // get header type
  char* token=strtok(s," ");
  const HBG_nomem* mny=find_money_keys(token);

  // scope close
  if(!strcmp(mny->format,"$:KILL_SCOPE;>")) {
    clscp();
    return;

  };

  // scope open
  opscp(mny);

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