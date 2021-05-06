/*  this files a WIP with some notes      **
**  completely liable to be rewritten     **
**  dont you dare judge!                  */

#ifndef __ZJC_DESCRIPTOR_H__
#define __ZJC_DESCRIPTOR_H__

#include "../zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

#define ZJC_DESCF_CONST  0x01
#define ZJC_DESCF_UNSIG  0x02
#define ZJC_DESCF_STATIC 0x04

#define ZJC_DESCF_PTR    0x08
#define ZJC_DESCF_PTRPTR 0x10

#define ZJC_DESCF_FUNC   0x20
#define ZJC_DESCF_STRUCT 0x40
#define ZJC_DESCF_ALIAS  0x80

/*  TYPE_ID: TYPE

    00: char  | 01: short  | 02: int  | 03: long  |
    04: float | 05: double | 06: void | 07: stark |
    08: str   | 09: vec    | 10: mat  | 11: mem   |
    12: com   | 13: ret    | 14: arg  | 15: def   |

istr4* arr-> str* arr[16]; -> 16 i*str4
 _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _ 
|_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_|0 y
|_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_|1 y
|_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_|2 y
|_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_|3 y

$:FOR2D<str*, int*> y, x, arr(4), y(4)
    setup;
        %1: xec: args;
        /1; //close

    end;
;>

$:0x24;>

$:IF x THEN y
    setup;
        %1: xec: args;
        /1;

    end;

;>

for(str* y=arr+0; y<arr+4; x++)
{

    :

    for(int* x=y+0; x<y+4; y++)
    {
        --->;
    }
}

*/

//   ---     ---     ---     ---     ---

typedef struct ZJC_DATA_DESCRIPTOR {

    char* id;                               // name for this address
    uint  type;                             // data type of the block
    uint  flags;                            // indirection, const, unsigned, static, etc.

} DESC;

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_DESCRIPTOR_H__