/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    KVR_STR                               *
*                                           *
*     -plain mem use                        *
*     -can do arrays                        *
*     -all ops are byte-sized               *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "kvr_str.h"
#include <string.h>

//   ---     ---     ---     ---     ---

MEM* MKSTR(char* buff, uint ex_alloc,
           uint disc_buff_len       )       {

    MEM* str;

    char name[ZJC_IDK_WIDTH];

    int  y     = 0;                         // no overflow
    int  len   =                            strlen(buff)+1;                            \
    int  limit =                            (ZJC_IDK_WIDTH-1                           );
    if(len < limit)                         { limit=len;                               };

    ex_alloc-=len*(disc_buff_len!=0);       // discount len of buff if var is set

    for(int x=len; y<limit; x--, y++) {     // make key from buff
        name[y]=*(buff+y);

    }; name[y] = '\0';

    y          = 0;                         // ask for mem
    ID id      = IDNEW                      ("STR*", name                              );
    MEMGET                                  (MEM, str, xBYTES(len+ex_alloc, uchar), &id);

                                            //copy buff into mem
    char* b    = MEMBUFF                    (str, char, 0                              );
    do                                      { b[y]=*buff; y++; } while(*buff++         );

    return str;                                                                             };

//   ---     ---     ---     ---     ---

MEM* GWSTR(MEM* str, uint mag)              {

    MEM*  new_str;

    int   y    = 0;                         // get len of old str and grow it
    char* buff = MEMBUFF                    (str, char, 0                                   );
    uint  len  =                            str->bsize+mag;                                 \

                                            // get mem and copy
    MEMGET                                  (MEM, new_str, xBYTES(len, uchar),byref(str->id));
    char* b    = MEMBUFF                    (str, char, 0);
    do                                      { b[y]=*buff; y++; } while(*buff++              );

                                            // free old mem and return new
    DLMEM                                   (str                                            );
    return new_str;                                                                         };

//   ---     ---     ---     ---     ---    // convenience macro. checks for size

#define CHKSTR  {                                         \
    if(tot >= (*str)->bsize) {                            \
        *str=GWSTR  (*str, 1 + ((*str)->bsize - tot));    \
        head=MEMBUFF(*str, char, 0                  );    \
    };                                              }

//   ---     ---     ---     ---     ---

void ADSTR(MEM** str, char* tail)           {

    int   x    = 0;                         // get str and size it
    char* head = MEMBUFF                    (*str, char, 0                       );
    uint  clen = strlen                     (head                                );
    uint  tot  = strlen                     (tail                                );

                                            // grow str if too small. then add
    CHKSTR; do                              { head[clen+x]=*tail; } while(*tail++);         };

//   ---     ---     ---     ---     ---

MEM* PLSTR(MEM* str, char* tail)            {

    int   x      = 0;                       // get str and size it
    char* head   = MEMBUFF                  (str, char, 0                        );
    uint  clen   = strlen                   (head                                );
    uint  tot    = strlen                   (tail                                );

    uint ex      = 0;                       // check if extra space is necessary
    if(tot>=str->bsize)                     { ex+=(str->bsize - tot);            }

                                            // get mem and strcat. return copy
    MEM* new_str = MKSTR                    (head, xBYTES(ex, uchar), 0          );
    head         = MEMBUFF                  (new_str, char, 0                    );
    do                                      { head[clen+x]=*tail; } while(*tail++);

    return new_str;                                                                         };

//   ---     ---     ---     ---     ---

void STSTR(MEM** str, char* buff)           {

    int   x    = 0;                         // get str and size it
    char* head = MEMBUFF                    (*str, char, 0                  );
    uint  clen = strlen                     (head                           );
    uint  tot  = strlen                     (buff                           );

                                            // grow str if too small. then set
    CHKSTR; do                              { head[x]=*buff; } while(*buff++);
};

//   ---     ---     ---     ---     ---

void RPSTR(MEM** str, char* tail,           \
           uint offset         )            {

                                            // ret without calout, cause if you fail this...
    if((*str)->bsize < offset)              { return;                            };

    int   x    = 0;                         // get str and size it
    char* head = MEMBUFF                    (*str, char, 0                       );
    uint  clen = strlen                     (head                                );
    uint  tot  = strlen                     (tail                                );
    head       =                            head+offset;                         \

                                            // grow str if too small. then replace
    CHKSTR; do                              { head[x]=*tail; x++; } while(*tail++);         };

//   ---     ---     ---     ---     ---