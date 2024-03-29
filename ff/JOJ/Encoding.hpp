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

  typedef struct FwdSubEncoding {

    const char* values;
    const int*  cnt;

    bool operator==(FwdSubEncoding& other) {

      return
          this->values == other.values
      &&  this->cnt    == other.cnt
      ;

    };

    bool operator!=(FwdSubEncoding& other) {
      return ! (*this==other);

    };

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
    Frac::STEP_4BIT,
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
    Frac::STEP_4BIT,
    Frac::STEP_4BIT,
    Frac::UNSIGNED

  };

  CX int ENCDEF_SHADE_CNT[]={4,0};

// ---   *   ---   *   ---

public:

  // flags for encoder branching
  cx8 NVEC=0b00;
  cx8 YAUV=0b01;
  cx8 ORME=0b10;

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

  CX char* IMG_TYPES[]={
    "_n","_a","_o"

  };

  CX char DEFCOMP[]={
    NVEC,YAUV,ORME,0x7F

  };

// ---   *   ---   *   ---

#endif // __24_JOJ_ENCODING_H__
