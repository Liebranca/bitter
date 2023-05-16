// ---   *   ---   *   ---
// CRK BMESH
// From bad to worse!
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "ff/CRK.hpp"

// ---   *   ---   *   ---
// read poses descriptor

void CRK::Bmesh::set_meta(void) {
  auto m=Bin::orc(m_fpath+"_meta");
  m_meta=reipret(Bmesh_Meta,m[0]);

};

// ---   *   ---   *   ---
// ^uses descriptor to read poses

void CRK::Bmesh::build(CRK& crk) {

  this->set_meta();

  // get data for each pose file
  for(uint16_t i=0;i < m_meta.num_poses;i++) {

    auto src=Bin::orc(
      m_fpath+"_pose"+std::to_string(i)

    );

    CRK::Prim me;
    uint64_t offset=0;

    this->set_box(
      me,(Bmesh_Pose_Box*) &src[offset]

    );

    offset+=sizeof(Bmesh_Pose_Box);

    this->set_indices(
      me,(uint16_t*) &src[offset]

    );

    offset+=sizeof(uint16_t) * m_meta.num_indices;

    this->set_verts(
      me,(Bmesh_Pose_Vert*) &src[offset]

    );

    crk.push_pose(me);

  };

};

// ---   *   ---   *   ---
// extract bounding box
// dimensions from fdump

void CRK::Bmesh::set_box(
  CRK::Prim&      me,
  Bmesh_Pose_Box* src

) {

  me.box.x=src->x;
  me.box.y=src->y;
  me.box.z=src->z;

};

// ---   *   ---   *   ---
// ^same, indices

void CRK::Bmesh::set_indices(
  CRK::Prim& me,
  uint16_t*  src

) {

  me.indices.resize(m_meta.num_indices);

  memcpy(
    me.indices.data(),
    src,

    sizeof(uint16_t) * m_meta.num_indices

  );

};

// ---   *   ---   *   ---
// ^same, vertex data

void CRK::Bmesh::set_verts(
  CRK::Prim&       me,
  Bmesh_Pose_Vert* src

) {

  me.verts.resize(m_meta.num_verts);
  for(uint16_t i=0;i<m_meta.num_verts;i++) {

    auto& vert=src[i];

    me.verts[i].set_xyz(vert.co);
    me.verts[i].set_ntb(vert.n,vert.t,vert.b);
    me.verts[i].set_uv(vert.uv);

  };

};

// ---   *   ---   *   ---
