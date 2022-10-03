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

void TRHEXVAL(uchar*   src ,
              MEMUNIT* to  )                {

    uchar   cbyte = 0x00;                   // curent byte
    uchar   chxd  = 0x00;                   // current hex digit
    MEMUNIT hxval = 0x00LL;                 // value of char, in hex

    uchar c       = 0x00;                   // empty char

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
            *to += (hxval)<<(cbyte*8);
            chxd++;

        } else {                            // is second digit
            *to += (hxval*16)<<(cbyte*8);
            chxd--; cbyte++;

        }; if(cbyte==UNITSZ) { cbyte=0; to++; }

    } while(*src-- != 0x78);                                                                };

//   ---     ---     ---     ---     ---

uint TRSTRVAL(uchar*   src ,
              MEMUNIT* to  )                {

    uchar  cbyte = 0x00;                    // curent byte
    uchar  c     = 0x00;                    // empty char

    ushort len   = 0;                       // length of string

//   ---     ---     ---     ---     ---

    do { c=*src; if(!c) { break; }

        *to|=((MEMUNIT) c) << (cbyte*8);
        cbyte++; len++;

        if(cbyte==UNITSZ) { cbyte=0; to++; };

    } while(*src++); return len;                                                            };

//   ---     ---     ---     ---     ---

void TRBITVAL(uchar*   src ,
              MEMUNIT* to  )                {

    uchar cbit  = 0x00;                     // current bit
    uchar cbyte = 0x00;                     // curent byte
    uchar c     = 0x00;                     // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts

        if(c != 0x31\
        && c != 0x30) {                     // nuuuuuuuuuuull!
            break;

                                            // easy money
        }; *to +=                           ((c==0x31) << cbit)<<(cbyte*8);

//   ---     ---     ---     ---     ---

        if(cbit==7) {                       // if all bits set move to next byte
            cbyte++; cbit=0;                // ... and go back to first bit!
            if(cbyte==UNITSZ) {
                cbyte=0; to++;

            }; continue;
        }; cbit++;                          // else go to next bit

    } while(*src-- != 0x62);                                                                };

//   ---     ---     ---     ---     ---

void TRDECVAL(uchar*   src ,
              MEMUNIT* to  )                {

    MEMUNIT decval = 0x00;                  // value in decimal
    uchar c        = 0x00;                  // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts
        if(!c) { break; }                   // lazy while

        decval *= 10;                       // left shift
        decval += c - 0x30;

    } while(*src++); (*to)+=(decval)<<(rd_cbyte*8);                                         };

//   ---     ---     ---     ---     ---

void TRFLTVAL(uchar*   src ,
              MEMUNIT* to  )                {

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

    MEMUNIT uval = *((MEMUNIT*) &whole);
    *to+=uval<<(rd_cbyte*8);

};

//   ---     ---     ---     ---     ---