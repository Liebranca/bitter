//**************************
// KVR_STR                 *
// plain mem usecan do     *
// arraysall ops are       *
// bytesized               *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "kvr_str.h"
#include <string.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static MEM* MLNG=NULL;      // big mem for
                            // temporal storage
void LDLNG(uint size) {
  DLMEM(MLNG);MLNG=MKSTR("MEMLNG",size,1);

};MEM* GTLNG(void) {return MLNG;};

//   ---     ---     ---     ---     ---
// #:0x1;>


MEM* MKSTR(
  const char* buff,
  uint ex_alloc,
  uint disc_buff_len) {


  MEM* str;

  char name[ZJC_IDK_WIDTH];

  int y=0;                  // no overflow
  int len=strlen(buff)+1;
  int limit=(ZJC_IDK_WIDTH-1);
  if(len<limit) {limit
   =len;};


  // discount len of buff if var is set
  ex_alloc-=len*(disc_buff_len!=0);

  for(int x=len;y<limit;x--,y++) {
                            // make key from buff
    name[y]=*(buff+y);

  };name[y]='\0';

  y=0;                      // ask for mem
  ID id=IDNEW("STR*",name);
  MEMGET(MEM,str,xBYTES(len+ex_alloc,uchar),
     &id);

                            // copy buff into mem
  char* b=MEMBUFF(str,char,0);
  do {b[y]=*buff;y++;} while(*buff++);

  return str;

};

//   ---     ---     ---     ---     ---
// #:0x2;>


MEM* GWSTR(MEM* str,uint len) {


  MEM* new_str;

  int y=0;                  // get len of old str
                            // and grow it
  char* buff=MEMBUFF(str,char,0);

                            // get mem and copy
  MEMGET(
    MEM,new_str,
    xBYTES(len,uchar),
    byref(str->id)

  );char* b=MEMBUFF(str,char,0);
  strcpy(b,buff);

  DLMEM(str);               // free old mem and
  return new_str;           // return new

};

//   ---     ---     ---     ---     ---
// #:0x3;> convenience macro. checks for size


#define CHKSTR { \
    if(required>=(*str)->bsize) {\
        *str=GWSTR(*str,1+required);\
    };}

//   ---     ---     ---     ---     ---
// #:0x4;>

void CTSTR(
  MEM** str,
  const char* tail) {

  // get size and compare
  uint required=(
    strlen(tail)
   +strlen(GTSTR(*str))

  );CHKSTR;

  // cat
  char* head=GTSTR(*str);
  strcat(head,tail);

};

//   ---     ---     ---     ---     ---
// #:0x5;>


MEM* CCTSTR(
  MEM* str,
  const char* tail) {

  // get size and compare
  uint required=(
    strlen(tail)
   +strlen(GTSTR(str))

  );uint ex=((required>=str->bsize)
    ? required
    : str->bsize

  );

  // make new mem
  MEM* new_str=MKSTR(
    str->id.key,
    xBYTES(ex,uchar),1

  );

  // cat
  char* head=GTSTR(new_str);
  strcpy(head,GTSTR(str));
  strcat(head,tail);

  // return copy
  return new_str;

};

//   ---     ---     ---     ---     ---
// #:0x6;>


void STSTR(

  MEM** str,
  const char* buff) {

  // get size and compare
  uint required=(
    strlen(buff)

  );CHKSTR;

  // cpy
  char* head=GTSTR(*str);
  strcpy(head,buff);

};

char* GTSTR(MEM* str) {
  return MEMBUFF(str,char,0);

}

//   ---     ---     ---     ---     ---
// #:0x7;>


void RPSTR(
  MEM** str,
  const char* tail,
  uint offset) {


                            // ret without
                            // calout, cause if
                            // you fail this...
  if((*str)->bsize<offset) {return;};

  int x=0;                  // get str and size
                            // it
  char* head=MEMBUFF(*str,char,0);
  uint avail=strlen(head);
  uint required=strlen(tail);
  head=head+offset;\

                            // grow str if too
                            // small. then
                            // replace
  CHKSTR;do {head[x]=*tail;x++;

} while(*tail++);};

//   ---     ---     ---     ---     ---
// #:0x8;>

MEM* sepstr_g(char* s, char* sep) {

  // count instances
  int cnt;{

    char* token=strtok(s,sep);
    while(token) {
      cnt++;token=strtok(NULL,sep);

    };
  };

//   ---     ---     ---     ---     ---

  // alloc storage
  MEM* m=MKSTR(sep,(cnt+1)*sizeof(char*),1);
  char** arr=MEMBUFF(m,char*,0);

  // store all instances
  for(int x=0;x<cnt;x++) {
    arr[x]=s;s+=strlen(s)+1;

  };arr[cnt]=NULL;

  return m;

};

//   ---     ---     ---     ---     ---