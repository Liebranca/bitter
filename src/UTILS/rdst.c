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

static size_t pattern_i=0;

static char* pattern_str=NULL;
static char** pattern_jumps={NULL};

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

  pattern_str=malloc(
    0x2000+(sizeof(pattern_jumps)*0x80)

  );

  pattern_jumps=(char**) (pattern_str+0x1000);
  for(int x=0;x<0x80;x++) {
    pattern_jumps[x]=pattern_str+(0x40*x);

  };

};void dlmod(void) {

  free(pattern_str);

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

  h->off_wr=h->mem+(wrbuff_sz-0x40);
  h->off_wr+=strlen(h->off_wr)+1;
  h->off_wr+=strlen(h->off_wr)+1;

  return h;

};

void clscp(void) {

  // get generator
  scopes_i--;
  HBG* h=scopes+scopes_i;

  // reposition and paste
  sprintf(
    h->off_wr+strlen(h->off_wr),

    "[%u]={NULL};",
    h->token_cnt*0x200

  );printf("%s\n\n",h->off_wr);

  // get additional generator content
  char* clause=h->mem+(wrbuff_sz-0x120);
  strcpy(clause+strlen(clause),") {");

  // print generated buffers
  printf("%s\n",h->mem);
  printf("%s\n",clause);
  printf("%s\n};\n",OUT_A);

  memset(OUT_A,0,wrbuff_sz);

};

//   ---     ---     ---     ---     ---
// generator write all steps

void onscp(char** symbols) {

  for(int x=0;x<scopes_i;x++) {

    HBG* h=scopes+x;

    char* clause=h->mem+(wrbuff_sz-0x120);

    if(!*clause) {
      sprintf(
        clause,
        "while(%s",

        symbols[h->order[0]]

      );

    } else {
      sprintf(
        clause+strlen(clause),
        " && %s",

        symbols[h->order[0]]

      );

    };

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

  // ^ ptrs for convenience
  char* name=pattern_jumps[pattern_i];
  char* sep=pattern_jumps[pattern_i+1];

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
        pattern_i+=2;

        name=pattern_jumps[pattern_i];
        sep=pattern_jumps[pattern_i+1];

        sep_i=0;

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
  pattern_i+=2;

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

      "char* %s",
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
      goto TABLE;

    };

  // null or invalid
  } else if(end==NULL) {
      return end+1;

  // regular
  };exp=strtok(s,";");

//   ---     ---     ---     ---     ---

  TABLE:
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

    "type>con:name,;"

  );

  while(*s) {s=next(s);}

  for(int x=0;x<pattern_i;x+=2) {
    printf(
      "%s %s\n",
      pattern_jumps[x],
      pattern_jumps[x+1]

    );

  };dlmod();

};