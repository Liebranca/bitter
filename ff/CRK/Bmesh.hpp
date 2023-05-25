// ---   *   ---   *   ---
// CRK paste-in

// ---   *   ---   *   ---
// describes a series of fdumps

struct Bmesh_Meta {

  uint16_t num_verts;
  uint16_t num_indices;
  uint16_t num_poses;

};

// ---   *   ---   *   ---
// ^bound box dimensions, per pose

struct Bmesh_Pose_Box {

  float x;
  float y;
  float z;

};

// ---   *   ---   *   ---
// ^body elem

struct Bmesh_Pose_Vert {

  glm::vec3 co;

  glm::vec3 n;
  glm::vec3 t;
  glm::vec3 b;

  glm::vec2 uv;

};

// ---   *   ---   *   ---
// build helper

struct Bmesh {

  std::string m_fpath;
  Bmesh_Meta  m_meta;

  void set_meta(void);
  void build(CRK& crk);

  void set_box(
    CRK::Prim&      me,
    Bmesh_Pose_Box* src

  );

  void set_indices(
    CRK::Prim& me,
    uint16_t*  src

  );

  void set_verts(
    CRK::Prim&       me,
    Bmesh_Pose_Vert* src

  );

};

// ---   *   ---   *   ---
