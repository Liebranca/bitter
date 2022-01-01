#ifndef __SH_GBL_H__
#define __SH_GBL_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct {            // message protocol

  uint64_t hed[2];          // magic number
  uint32_t len;             // length of message
  uint32_t flg;             // usage flags

  char mess[];              // contents

} SHSIG;

//   ---     ---     ---     ---     ---

static const uint64_t SHSIG_MSIN[2]={

  0x52454E4E49243A24LL,     // $:$INNER
  0x24242424243E3B21LL      // !;>$$$$$

};

//   ---     ---     ---     ---     ---

static const char ICO_SYS[]={

  0x00,                     // empty_file
  0x01,                     // normal_file
  0x02,                     // linked_file
  0x17,                     // cog
  0x1D,                     // sound
  0x1E,                     // bunny

  0x03,                     // close_button
  0x06,                     // weird_loop
  0x16,                     // save

  0x04,                     // empty_folder
  0x05,                     // normal_folder

  0x07,                     // go_back
  0x08,                     // dir_head
  0x09,                     // reload

  0xE6                      // settings

};

static const char ICO_PLAY[]={

  0x0A,                     // reverseX2
  0x0B,                     // reverse
  0x0C,                     // pause
  0x0D,                     // play
  0x0E,                     // playX2
  0x0F,                     // start
  0x11,                     // end

  0x12,                     // brrow up
  0x13,                     // brrow rotd
  0x14,                     // brrow down
  0x15,                     // brrow left
  0x16                      // brrow right

};

static const char ICO_DRAW[]={

  0x19,                     // left down block
  0x1A,                     // right down block
  0x1B,                     // right up block
  0x1C,                     // left up block

  0x7C,                     // sing_v
  0xC0,                     // sing_h
  0xC1,                     // sing_ruc
  0xC2,                     // sing_ldc
  0xC3,                     // sing_cross
  0xC4,                     // sing_rdc
  0xC5,                     // sing_cross_r
  0xC6,                     // sing_cross_l
  0xC7,                     // sing_cross_u
  0xC8,                     // sing_cross_d

  0xC9,                     // doub_h
  0xCA,                     // doub_ruc
  0xCB,                     // doub_rdc
  0xCC,                     // doub_luc
  0xCD,                     // doub_ldc
  0xCE,                     // doub_v
  0xCF,                     // doub_cross
  0xD0,                     // doub_cross_r
  0xD1,                     // doub_cross_l
  0xD2,                     // doub_cross_d
  0xD3,                     // doub_cross_u

  0xDE,                     // block_empty
  0xDF,                     // block_greyed
  0xE0,                     // block_focus
  0xE1,                     // block_gem
  0xE2,                     // block_solid
  0xE3,                     // block_solid_small
  0xE4,                     // block_crosh
  0xE5                      // block_funky

};

static const char ICO_HEART[]={

  0x7F,                     // frame0
  0x80,                     // frame1
  0x81,                     // frame2
  0x82,                     // frame3
  0x83,                     // frame4
  0x84,                     // frame5
  0x85,                     // frame6

  0x86                      // greyed

};

static const char ICO_SHIELD[]={

  0x87,                     // frame0
  0x88,                     // frame1
  0x89,                     // frame2
  0x8A,                     // frame3
  0x8B,                     // frame4
  0x8C,                     // frame5
  0x8D,                     // frame6

  0x8E,                     // greyed
  0x8F,                     // magic
  0x90,                     // focus
  0x9A,                     // ready
  0x91,                     // break block
  0x93,                     // block
  0x94                      // magic block

};

static const char ICO_BATTERY[]={

  0xB1,                     // full
  0xB2,                     // full-tip
  0xB3,                     // full-tip-tip
  0x92,                     // half
  0xB5,                     // quart
  0xB6,                     // falfquart
  0xB7,                     // empty

  0xB8,                     // greyed
  0x10                      // side

};

static const char ICO_BULLET[]={

  0x95,                     // full
  0x96,                     // full-quart
  0x97,                     // half
  0x98,                     // quart
  0x99                      // empty

};

static const char ICO_EYES[]={

  0x9B,                     // frame0_l
  0x9C,                     // frame1_l
  0x9D,                     // frame2_l
  0x9E,                     // frame3_l
  0x9F,                     // frame4_l
  0xA0,                     // frame5_l

  0xA3,                     // frame0_r
  0xA4,                     // frame1_r
  0xA5,                     // frame2_r
  0xA6,                     // frame3_r
  0xA7,                     // frame4_r
  0xA8                      // frame5_r

};

static const char ICO_CLCK[]={

  0xA9,                     // frame0
  0xAA,                     // frame1
  0xAB,                     // frame2
  0xAC,                     // frame3
  0xAD,                     // frame4
  0xAE,                     // frame5
  0xAF,                     // frame6
  0xB0                      // frame7

};

static const char ICO_DIAMOND[]={

  0xB9,                     // full
  0xBA,                     // crossh
  0xBB,                     // crossh_h
  0xBC,                     // crossh_v
  0xBD,                     // crossh_dot
  0xBE,                     // crossh_empty
  0xBF                      // crossh_circle

};

static const char ICO_DEATH[]={

  0xD4,                     // tomb_a
  0xD5,                     // tomb_b
  0xD6,                     // chala
  0xD7,                     // 10
  0xD8,                     // skull_a
  0xD9,                     // skull_b
  0xDA,                     // lemmy
  0xDB,                     // diamond_w
  0xDC,                     // diamond_b
  0xDD,                     // diameter

  0xED,                     // grass
  0xEE                      // flower

};

static const char ICO_SHADES[]={

  0xE7,                     // waves 0
  0xE8,                     // waves 1
  0xE9,                     // checker 0
  0xEA,                     // checker 1
  0xEB,                     // stars 0
  0xEC,                     // stars 1

  0xF1,                     // greyed
  0xFF                      // nonspace

};

static const char ICO_ARABY[]={

  0xEF,                     // dhaal
  0xF0,                     // yaa
  0xF2,                     // kaaf
  0xF3,                     // thaa
  0xF4,                     // qaaf
  0xF5,                     // 7aa
  0xF6                      // 3yn

};

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
};
#endif

#endif //__SH_GBL_H__