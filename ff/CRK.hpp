#ifndef __24_CRK_H__
#define __24_CRK_H__

// ---   *   ---   *   ---
// deps

  #include <glm/glm.hpp>
  #include <vector>

  #include "kvrnel/GLM.hpp"
  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Seph.hpp"

  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---
// info

class CRK: public Bin {

public:

  VERSION     "v2.01.1";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---

  cxr32 CO_STEP = Frac::STEP[Frac::STEP_8BIT];
  cx8   CO_BITS = Frac::BITS[Frac::SIZE_8BIT];

  enum {

    QUAD,

    FRAME,
    SPRITE,

    TRIMESH,
    BMESH

  };

  typedef std::vector<vec3> Points;

// ---   *   ---   *   ---
// basis struct

  struct Vertex {

    union {

      struct {

        uint8_t  XYZ[3];
        uint8_t  TEX[2];  // 5
        uint8_t  PAD[1];  // 6

        uint16_t NTB[3];  // 12

      };

      uint8_t data[16];

    };

    // fill out struct from
    // uncompressed data
    void set_xyz(vec3& co);
    void set_n(vec3& n);

    void set_ntb(
      vec3& n,
      vec3& t,
      vec3& b

    );

    void set_uv(vec2& uv);

    // ^undo, mostly for debug
    vec3 get_xyz(void);
    vec3 get_n(void);
    vec2 get_uv(void);

  };

// ---   *   ---   *   ---
// constructor helpers

  struct Prim {

    std::vector<CRK::Vertex> verts;
    std::vector<uint16_t>    indices;

    vec3 box={0.5f,1,0.5f};

    uint64_t bytesz(void);

    void to_buff(
      Mem<uint8_t>& dst,
      uint64_t&     ptr

    );

    void from_buff(
      Mem<uint8_t>& src,
      uint64_t&     ptr

    );

    // mod index buff for line drawing
    void tris_to_lines(void);

    // ^reverse
    void lines_to_tris(void);

    // generates index array for
    // simple quad arrays
    void gen_qa_indices(void);

    // blank init
    Prim(void) {};

    // generate zeroed buffer
    Prim(uint16_t size);

    // wipe data
    inline void clear(void) {
      verts.clear();
      indices.clear();

    };

  };

  struct Prim_Build {

    Prim*    me;

    uint16_t vert;
    uint16_t idex;

  };

  typedef std::vector<CRK::Prim> Mesh;

  struct Mesh_Build {

    uint16_t type;
    void*    data;

  };

  typedef std::vector<
    CRK::Mesh_Build

  > Mesh_Builds;

// ---   *   ---   *   ---
// geometry helpers

  #include "ff/CRK/Quad.hpp"
  #include "ff/CRK/Bmesh.hpp"

  struct Frame_Build {
    float scale[2];
    JOJ::Img_Desc img;

  };

  struct Sprite_Build {
    float scale[2];
    JOJ::Atlas_Desc atlas;

  };

  struct Tri {

    // cstruc
    void set(Points& pts,vec3& n) {
      m_points=pts;
      m_normal=n;

    };

    Tri(Points& pts,vec3& n) {
      this->set(pts,n);

    };

    // ctrash
    Tri(void) {};
    ~Tri(void) {};

    // converts to CRK format
    void pack(CRK::Prim& me);

    Points m_points;
    vec3   m_normal;

  };

  typedef std::vector<Tri> Tris;

// ---   *   ---   *   ---
// bin header

private:

  struct Header {

    char     sig[4];

    uint16_t vcount;
    uint16_t icount;

    uint64_t prim_cnt;

  };

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
// data packing singletons

  static Seph& nseph(void) {
    static Seph s(Seph::NORMAL,0,8,8);
    return s;

  };

  static Seph& pseph(void) {
    static Seph s(Seph::POINT,8,8,8);
    return s;

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

  CRK::Prim make_trimesh(
    CRK::Tris* bld

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
    CRK::Mesh_Builds& blds

  );

  // ^from *.joj
  CRK(std::string fpath,JOJ& joj);

  // ^from raw
  CRK(
    std::string fpath,
    CRK::Tris&  tris

  );

  // load
  CRK(std::string fpath);

  void pack(void);
  void unpack(void);

  inline CRK::Mesh& data(void) {
    return m_data;

  };

  // push result of builder
  // to pose array
  void push_pose(CRK::Prim& me) {
    m_hed.vcount+=me.verts.size();
    m_hed.icount+=me.indices.size();

    m_data.push_back(me);

  };

};

// ---   *   ---   *   ---

#endif // __24_CRK_H__
