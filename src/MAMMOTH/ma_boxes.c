/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_BOXES                              *
*                                           *
*     -buncha vars here                     *
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

#include "ma_boxes.h"

//   ---     ---     ---     ---     ---

TYPEDATA typedata    = {0};

MAMMIT*  mammi       = NULL;
HASH*    GNAMES_HASH = NULL;                // globals and built-ins
HASH*    LNAMES_HASH = NULL;                // user-defined symbols

//   ---     ---     ---     ---     ---

uchar    tokens[MAMMIT_TK_COUNT][MAMMIT_TK_WIDTH];

uchar*   rd_tk       = tokens[0];           // components of current statement
uint     rd_tki      = 0;                   // num of tokens
uint     rd_tkp      = 0;                   // token char idex
uint     rd_tkx      = 0;                   // (iter) current token idex

uchar*   rd_buff     = NULL;                // char buffer containing statements

uchar    rd_cur      = '\0';                // current char in buff
uchar    rd_prv      = '\0';                // previous char in buff
uchar    rd_nxt      = '\0';                // next char in buff

ushort   rd_wid      = 0x0000;              // next | (cur<<8)
uint     rd_pos      = 0;                   // position into buffer

MEM*     memlng      = NULL;                // big block of memory
uint     lngptr      = 0;                   // cur idex into mem, in bytes

//   ---     ---     ---     ---     ---

uint     rd_cast     = 0;                   // for "dynamic" type-casting;
                                            // actually, just cases in a switch

                                            // 0x00 void
                                            // 0x01 nihil
                                            // 0x02 stark

                                            // 0x03 int8_t
                                            // 0x04 int16_t
                                            // 0x05 int32_t
                                            // 0x06 int64_t

                                            // 0x07-0A ^unsigned idem...

                                            // 0x0B float

//   ---     ---     ---     ---     ---

uint   gblevil       =  0x00000000;         // global evilstate
uint   rd_line       =  1;                  // current line

//   ---     ---     ---     ---     ---

uint   rd_flags      = 0;                   // tracks opstate at current level
uint   rd_size       = 0;                   // bytesize of current fetch/assign op
uint   rd_elems      = 0;                   // element count of array

uchar* rd_rawv       = NULL;                // raw value from buff, before processing
uchar* rd_lhand      = NULL;                // current point in stack
uchar* rd_value      = NULL;                // next point in stack
uchar* rd_oldval     = NULL;                // next from next, used to remember things

ulong  szmask_a      = 0x0000000000000000;  // lower 64-bit of rd mask. used for casting
ulong  szmask_b      = 0x0000000000000000;  // ^upper, idem

uint   sec_beg       = 0;                   // start "$" idex of sec operation
uint   sec_end       = 0;                   // end   "&" idex of sec operation

//   ---     ---     ---     ---     ---