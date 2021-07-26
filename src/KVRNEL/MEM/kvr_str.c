#include "kvr_str.h"
#include <string.h>

//   ---     ---     ---     ---     ---

MEM* MKSTR(char* buff, uint ex_alloc)       {

    char name[KVR_IDK_WIDTH];

    int  len   = strlen(buff)+1;
    int  limit = (KVR_IDK_WIDTH-1);         if(len < limit) { limit=len; }
    int  y     = 0;

    for(int x=(int) len; y<limit; x--, y++) {
        name[y]=*(buff+(x-(limit-y)));

    }; name[y]='\0';

    ID id=IDNEW("STR*", name);
    MEM* str; MEMGET(MEM, str, len+ex_alloc, &id);

    y=0; char* b=MEMBUFF(str, char, 0); do { b[y]=*buff; y++; } while(*buff++);

    return str;                                                                             };

//   ---     ---     ---     ---     ---

MEM* GWSTR(MEM* str, uint mag)              {

    char* buff = MEMBUFF(str, char, 0);
    uint len   = str->bsize+mag;

    MEM* new_str; MEMGET(MEM, new_str, len, byref(str->id)); int y=0;
    char* b=MEMBUFF(str, char, 0); do { b[y]=*buff; y++; } while(*buff++);

    DLMEM(str); return new_str;                                                             };

//   ---     ---     ---     ---     ---

#define CHKSTR  { MEM** pstr=&str;                          \
    if(tot >= (*pstr)->bsize) {                             \
        *pstr=GWSTR (*pstr, 1 + ((*pstr)->bsize - tot));    \
        head=MEMBUFF(*pstr, char, 0                   );    \
    };                                                }

//   ---     ---     ---     ---     ---

void ADSTR(MEM* str, char* tail)            {

    char* head = MEMBUFF(str, char, 0);

    uint clen  = strlen (head        );
    uint tot   = strlen (tail        ); CHKSTR;

    int x=0; do { head[clen+x]=*tail; } while(*tail++);

};

MEM* PLSTR(MEM* str, char* tail)            {

    char* head   = MEMBUFF(str, char, 0);

    uint clen    = strlen (head        );
    uint tot     = strlen (tail        );

    uint ex      = 0; if(tot>=str->bsize) { ex+=(str->bsize - tot); }

    MEM* new_str = MKSTR(head, ex);
    head=MEMBUFF(new_str, char, 0); int x=0; do { head[clen+x]=*tail; } while(*tail++);

    return new_str;                                                                         };

void STSTR(MEM* str, char* buff)            {

    char* head = MEMBUFF(str, char, 0);

    uint clen  = strlen(head);
    uint tot   = strlen(buff); CHKSTR;

    int x=0; do { head[x]=*buff; } while(*buff++);                                          };

//   ---     ---     ---     ---     ---

void RPSTR(MEM* str, char* tail,            \
           uint offset         )            {

    char* head = MEMBUFF(str, char, 0);

    uint clen  = strlen(head);
    if(str->bsize < offset) { return; }

    head       = head+offset;
    uint tot   = strlen(tail); CHKSTR;
    if(str->bsize < tot   ) { return; }

    int x=0; do { head[x]=*tail; x++; } while(*tail++);                                     };

//   ---     ---     ---     ---     ---