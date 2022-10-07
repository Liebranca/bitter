// ---   *   ---   *   ---
// boring details that'd make the header
// too long for the casual reader
// meant to be pasted in!

#ifndef __24_JOJ_ENCODING_H__
#define __24_JOJ_ENCODING_H__

// ---   *   ---   *   ---
// meant as a helper, don't use directly
// I can't get it to work if made private :/

public:

  typedef struct {

    const char* values;
    const int*  cnt;

  } SubEncoding;

// ---   *   ---   *   ---
// from 0 (lowest) to 7(highest)
// level of precision to use for
// each field of a material

  typedef struct {

    SubEncoding normal;
    SubEncoding color;
    SubEncoding shade;

  } Encoding;

// ---   *   ---   *   ---
// represents a 2x2 block of pixels

  typedef struct {

    uint64_t   idex;
    uint64_t   value;
    uint64_t   freq;

    uint64_t color_id[4];

  } Pixel_Block;

// ---   *   ---   *   ---
// normal maps

private:

  CX char ENCDEF_NORMAL[]={

    // xz
    Frac::STEP_2BIT,
    Frac::SIZE_3BIT,
    Frac::SIGNED,

    // y
    Frac::STEP_1BIT,
    Frac::SIZE_2BIT,
    Frac::SIGNED,

    // curv
    Frac::STEP_3BIT,
    Frac::SIZE_4BIT,
    Frac::UNSIGNED

  };

  CX int ENCDEF_NORMAL_CNT[]={2,1,1,0};

// ---   *   ---   *   ---
// albedo/diffuse/picture

  CX char ENCDEF_COLOR[]={

    // alpha && luma
    Frac::STEP_4BIT,
    Frac::SIZE_4BIT,
    Frac::UNSIGNED,

    // chroma uv
    Frac::STEP_4BIT,
    Frac::SIZE_4BIT,
    Frac::SIGNED,

  };

  CX int ENCDEF_COLOR_CNT[]={2,2,0};

// ---   *   ---   *   ---
// shading maps made up of multiple
// grayscale images

  CX char ENCDEF_SHADE[]={

    // occlu,rough,metal,emit
    Frac::STEP_3BIT,
    Frac::STEP_4BIT,
    Frac::UNSIGNED

  };

  CX int ENCDEF_SHADE_CNT[]={4,0};

// ---   *   ---   *   ---

public:

  // flags for encoder branching
  cx8 NVEC=0b001;
  cx8 YAUV=0b010;
  cx8 ORME=0b100;

  // the intended JOJ experience
  CX Encoding ENCDEF={

    .normal={
      ENCDEF_NORMAL,
      ENCDEF_NORMAL_CNT

    },

    .color={
      ENCDEF_COLOR,
      ENCDEF_COLOR_CNT

    },

    .shade={
      ENCDEF_SHADE,
      ENCDEF_SHADE_CNT

    }

  };

// ---   *   ---   *   ---

#endif // __24_JOJ_ENCODING_H__
