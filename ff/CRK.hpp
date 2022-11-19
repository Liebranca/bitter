#ifndef __24_CRK_H__
#define __24_CRK_H__

// ---   *   ---   *   ---
// deps

  #include <vector>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Bin.hpp"

  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---
// info

class CRK: public Bin {

public:

  VERSION     "v2.00.2";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---

  cxr32 CO_STEP = Frac::STEP[Frac::STEP_8BIT];
  cx8   CO_BITS = Frac::BITS[Frac::SIZE_8BIT];

  enum {

    QUAD,

    FRAME,
    SPRITE

  };

// ---   *   ---   *   ---
// basis struct

  typedef struct {

    union {

      struct {

        uint8_t  XYZ[3];
        uint8_t  NTB[3];  // 6
        uint8_t  TEX[2];  // 8

        uint16_t BONE;    // 10
        uint16_t ID;      // 12

      };

      uint8_t data[16];

    };

  } Vertex;

// ---   *   ---   *   ---
// constructor helpers

  typedef struct {

    std::vector<CRK::Vertex> verts;
    std::vector<uint16_t>    indices;

    uint64_t bytesz(void);

    void to_buff(
      Mem<uint8_t>& dst,
      uint64_t&     ptr

    );

    void from_buff(
      Mem<uint8_t>& src,
      uint64_t&     ptr

    );

  } Prim;

  typedef struct {

    Prim*    me;

    uint16_t vert;
    uint16_t idex;

  } Prim_Build;

  typedef std::vector<CRK::Prim> Mesh;

  struct Mesh_Build {

    uint16_t type;
    void*    data;

  };

  typedef std::vector<
    CRK::Mesh_Build

  > Mesh_Builds;

  struct Vertex_Packed {
    uint64_t sz;
    uint64_t cnt;

  };

// ---   *   ---   *   ---
// geometry helpers

  #include "ff/CRK/Quad.hpp"

  struct Frame_Build {
    float scale[2];
    JOJ::Img_Desc img;

  };

  struct Sprite_Build {
    float scale[2];
    JOJ::Atlas_Desc atlas;

  };

// ---   *   ---   *   ---
// bin header

private:

  typedef struct {
    char     sig[4];

    uint16_t vcount;
    uint16_t icount;

  } Header;

// ---   *   ---   *   ---
// virtual consts

  vicstr m_fsig(void) {
    return "%CRK";

  };

  vic64 m_header_sz(void) {
    return sizeof(CRK::Header);

  };

  VIC void* get_header(void) {
    return &m_hed;

  };

// ---   *   ---   *   ---
// attrs

  CRK::Header m_hed;
  CRK::Mesh   m_data;

// ---   *   ---   *   ---
// guts

  CRK::Prim make_sprite_frame(
    CRK::Frame_Build* bld

  );

  void make_sprite(
    CRK::Sprite_Build* bld

  );

  void make_prim(
    uint16_t type,
    void*    data

  );

  Mem<uint8_t> mesh_to_buff(void);
  void buff_to_mesh(Mem<uint8_t>& src);

  void build(CRK::Mesh_Builds& bld);
  void open(std::string fpath,char mode);

  void write(void);
  void read(void);

// ---   *   ---   *   ---
// iface

public:

  // new
  CRK(

    std::string       fpath,
    CRK::Mesh_Builds& bld

  );

  // ^from *.joj
  CRK(std::string fpath,JOJ& joj);

  // load
  CRK(std::string fpath);

  void pack(void);
  void unpack(void);

};

// ---   *   ---   *   ---

#endif // __24_CRK_H__
