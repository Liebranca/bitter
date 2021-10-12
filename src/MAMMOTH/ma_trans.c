/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_TRANS                              *
*                                           *
*     -string to num conversions            *
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

#include "ma_trans.h"
#include "KVRNEL/zjc_evil.h"

//   ---     ---     ---     ---     ---

void TRHEXVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    uchar cbyte = 0x00;                     // curent byte
    uchar chxd  = 0x00;                     // current hex digit
    uchar hxval = 0x00;                     // value of char, in hex

    uchar c     = 0x00;                     // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts

        if(c<=0x39) {                       // if 0-9
            hxval=c-0x30;

        }

        elif( (0x41<=c)
        &&    (0x46>=c) ) {                 // if A-F
            hxval=c-0x37;

        }

        else {                              // nuuuuuuuuuuuuuuuull!
            break;

        };

//   ---     ---     ---     ---     ---

        if(!chxd) {                         // is first digit
            to[cbyte]  = hxval;
            chxd++;

        } else {                            // is second digit
            to[cbyte] += hxval*16;
            chxd--; cbyte++;

    }} while(*src-- != 0x78);                                                               };

//   ---     ---     ---     ---     ---

void TRBITVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    uchar cbit  = 0x00;                     // current bit
    uchar cbyte = 0x00;                     // curent byte
    uchar c     = 0x00;                     // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts

        if(c != 0x31\
        && c != 0x30) {                     // nuuuuuuuuuuull!
            break;

        }; to[cbyte] |= (c==0x31) << cbit;  // easy money

//   ---     ---     ---     ---     ---

        if(cbit==7) {                       // if all bits set move to next byte
            cbyte++; cbit=0;                // ... and go back to first bit!
            continue;

        }; cbit++;                          // else go to next bit

    } while(*src-- != 0x62);                                                                };

//   ---     ---     ---     ---     ---

void TRDECVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    ulong decval = 0x0000000000000000;      // value in decimal
    uchar c      = 0x00;                    // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts
        if(!c) { break; }                   // lazy while

        decval *= 10;                       // left shift
        decval += c - 0x30;

    } while(*src++);

//   ---     ---     ---     ---     ---

    for(uint x=0; x<size; x++) {            // copy bytes over
        to[x]=(decval&(0xFF<<(x*8))) >> (x*8);

    };                                                                                      };

//   ---     ---     ---     ---     ---

void TRFLTVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    float  whole = 0.0f;                    // integer portion of number
    float  fract = 0.0f;                    // fraction portion of number
    float  fval  = 0.0f;                    // value at current char

    uint   dotd  = 0;                       // right shift multiplier
    uchar  c     = 0x00;                    // blank char

//   ---     ---     ---     ---     ---

    do { c=*src;

        if(!c) { break; }                   // nuuuuuuuuuuull!

        fval=(float) c-0x30;                // fval be in (0,9), else is dot

        if(c==0x2E) {                       // dot spotted, do fractions now
            dotd=1;                         // start fractions at 0.1 and advance that
            continue;                       // skip to next char...

        }

//   ---     ---     ---     ---     ---

        elif(!dotd) {
            whole*=10;                      // left shift
            whole+=fval;                    // add to total

            continue;                       // skip!

        };

//   ---     ---     ---     ---     ---

        dotd  *= 10;                        // up the right shift factor
        fract += fval*(1.0f/dotd);          // right shift value and add

    } while(*src++); whole+=fract;

//   ---     ---     ---     ---     ---

    uint uval = *((uint*) &whole);          // read these bytes as an int
    for(uint x=0; x<size; x++) {            // copy them over
        to[x]=(uval&(0xFF<<(x*8))) >> (x*8);

    };                                                                                      };

//   ---     ---     ---     ---     ---