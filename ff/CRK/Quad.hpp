// ---   *   ---   *   ---
// CRK paste-in

// ---   *   ---   *   ---

struct Quad_Build : Prim_Build {

  union {
    struct {
      uint8_t co_x[2];
      uint8_t co_y[2];

      uint8_t uv_x[2];
      uint8_t uv_y[2];

      uint8_t layer;
      uint8_t id;

    };

    uint8_t data[10];

  };

  float    co_scale;
  float    uv_scale;

  JOJ::Tile_Desc td;

// ---   *   ---   *   ---

  Quad_Build(Prim* p) {
    me   = p;
    vert = 0;
    idex = 0;

  };

// ---   *   ---   *   ---

  inline void push_quad(void) {

    this->get_verts();
    this->get_indices();

    vert+=4;
    idex+=6;

  };

  void calc_verts(void);
  void get_verts(void);
  void get_indices(void);

};

// ---   *   ---   *   ---
