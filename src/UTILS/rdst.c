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
#include "KVRNEL/TYPES/zjc_hash.h"

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
// static mem

static char OUT_A[wrbuff_sz]={0};

//   ---     ---     ---     ---     ---
// header blocks generator

typedef struct {

  char* format;
  int order[4];

  int token_cnt;
  char* off_wr;
  char mem[wrbuff_sz];

} HBG;typedef struct {

  const char* format;
  const int order[4];

} HBG_nomem;

static HBG scopes[rdbuff_sz]={0};
static size_t scopes_i=0;

static char* strrptr[]={

  "NULL",
  "NULL",
  "NULL",
  "NULL"

};

//   ---     ---     ---     ---     --
// keyword/callback table

typedef struct {

  ID id;

  const char* key;
  HBG_nomem mny;

} MNYK;static MNYK money_keys[]={

  EMPTY_ID,"%iter",
    "char* %s=strtok(%s,\"%s\");\n",
    0,1,2,0,

  EMPTY_ID,"/iter",
    "$:SCOPE_KILL;>",
    0,0,0,0

};

//   ---     ---     ---     ---     ---
// storage management

static HASH* lkp_table=NULL;
static ID* lkp_table_ids=NULL;

void ntmod(void) {

  lkp_table=MKHASH(5,"mny-builtins");
  lkp_table_ids=malloc(
    (sizeof(ID)*szhash(lkp_table))
   +(sizeof(HBG_nomem)*szhash(lkp_table))

  );

  for(

    int x=0;

    x<sizeof(money_keys)
    /sizeof(money_keys[0]);

    x++

  ) {

    MNYK* h=money_keys+x;
    h->id=IDNEW("lkp_",(char*) h->key);

    HASHSET(lkp_table,byref(h->id));

  };

};void dlmod(void) {
  free(lkp_table_ids);
  DLMEM(lkp_table);

};

//   ---     ---     ---     ---     ---
// find (str)key in hash

const HBG_nomem* find_money_keys(char* key) {

  void* nulmy;
  STR_HASHGET(lkp_table,key,nulmy,0);

  if(nulmy) {
    return &( ((MNYK*) nulmy)->mny );

  };return NULL;

};

//   ---     ---     ---     ---     ---
// open/close generator steps

HBG* opscp(const HBG_nomem* src) {

  HBG* h=scopes+scopes_i;
  scopes_i++;

  memset(h->mem,0,wrbuff_sz);

  h->format=(char*) src->format;

  h->order[0]=src->order[0];
  h->order[1]=src->order[1];
  h->order[2]=src->order[2];

  // save this special format
  // we'll use it later :>

  strcpy(
    h->mem+(wrbuff_sz-0x80),
    "char* %s"

  );

  h->off_wr=h->mem+(wrbuff_sz-0x40);
  h->off_wr+=strlen(h->off_wr)+1;
  h->off_wr+=strlen(h->off_wr)+1;

  return h;

};

void clscp(void) {

  scopes_i--;

  // get generator
  HBG* h=scopes+scopes_i;

  // last-minute stuff
  char* format="[%u]={NULL};";
  char* beg=h->off_wr;

  // seek to end
  h->off_wr+=strlen(h->off_wr);

  // paste
  sprintf(h->off_wr,
    format,
    h->token_cnt*0x200

  );printf("%s\n\n",beg);

  // print generated buffers
  printf("%s\n",h->mem);
  printf("%s\n",OUT_A);

  memset(OUT_A,0,wrbuff_sz);

};

//   ---     ---     ---     ---     ---
// generator write all steps

void onscp(char** symbols) {

  for(int x=0;x<scopes_i;x++) {

    HBG* h=scopes+x;

    sprintf(
      h->mem+strlen(h->mem),
      h->format,

      symbols[h->order[0]],
      symbols[h->order[1]],
      symbols[h->order[2]]

    );h->token_cnt++;
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

        char* symbols[]={name,strrptr[0],sep};
        onscp(symbols);

        sprintf(

          OUT_A+strlen(OUT_A),

          "char* %s=strtok(%s"
          ",\"%s\");\n",

          name,"NULL",sep

        );sep_i=0;

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

  char* symbols[]={name,strrptr[0],sep};
  onscp(symbols);

  sprintf(

    OUT_A+strlen(OUT_A),

    "char* %s=strtok(%s"
    ",\"%s\");\n",

    name,"NULL",sep

  );sep_i=0;

};

//   ---     ---     ---     ---     ---

void mkhed(char* s) {

  // get header type
  char* token=strtok(s," ");
  const HBG_nomem* mny=find_money_keys(token);

  // read as instruction
  if(!mny) {

    int x;char* sub;
    while((sub=strtok(NULL,","))!=NULL) {
      strrptr[x]=sub;
      sub=strtok(NULL,",");

      x++;

    };return;

  };

//   ---     ---     ---     ---     ---

  // scope close
  if(!strcmp(mny->format,"$:SCOPE_KILL;>")) {
    clscp();
    return;

  };

  // scope open
  int x;char* sub;
  while((sub=strtok(NULL,","))!=NULL) {
    strrptr[x]=sub;
    sub=strtok(NULL,",");

    x++;

  };opscp(mny);

//   ---     ---     ---     ---     ---

  // string array for scope tokens
  if(!strcmp(strrptr[x],"NULL")) {

    // fill in name
    sprintf(
      scopes[scopes_i-1].off_wr,

      scopes[scopes_i-1].mem+(wrbuff_sz-0x80),
      strrptr[0]

    );

  };
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

  ntmod();memset(OUT_A,0,wrbuff_sz);

  char buff[wrbuff_sz];
  char* s=buff+0;

  memset(s,0,0x400);
  strcpy(s,

    "$:%iter arg;>"
    "  type>con:name,;"

    "$:/iter;>"

  );

  while(*s) {s=next(s);}

  /* generate this...

  char* name=strtok(s," ");

  char* arg_type=strtok(NULL, ">");
  char* arg_con=strtok(NULL, ":");
  char* arg_name=strtok(NULL, ",");*/

  dlmod();

};