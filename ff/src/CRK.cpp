// ---   *   ---   *   ---
// CRK
// Corrupted geometry
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Bytes.hpp"
  #include "ff/CRK.hpp"

// ---   *   ---   *   ---
// new file

CRK::CRK(
  std::string       fpath,
  CRK::Mesh_Builds& bld

): Bin() {

  this->defopen<CRK::Header>(
    fpath,Bin::NEW

  );

  this->build(bld);

};

// ---   *   ---   *   ---
// read file

CRK::CRK(
  std::string fpath

): Bin() {

  this->defopen<CRK::Header>(
    fpath,Bin::READ

  );

};

// ---   *   ---   *   ---

void CRK::build(CRK::Mesh_Builds& bld) {

  for(CRK::Mesh_Build p : bld) {
    this->make_prim(p.type,p.data);

  };

};

// ---   *   ---   *   ---

CRK::Prim CRK::make_sprite_frame(
  CRK::Build_Sprite* bld

) {

  CRK::Prim       me;
  CRK::Quad_Build qbld(&me);

  uint64_t tile_cnt = bld->img.size();

  qbld.co_scale     = bld->scale[0];
  qbld.uv_scale     = bld->scale[1];

  // 4 verts (2 tris) per tile
  // 3 indices per tri
  me.verts.resize(4*tile_cnt);
  me.indices.resize(6*tile_cnt);

  // push one quad per tile
  for(uint64_t i=0;i<tile_cnt;i++) {
    qbld.td=bld->img[i];
    qbld.push_quad();

  };

  return me;

};

// ---   *   ---   *   ---

void CRK::make_prim(

  uint16_t type,
  void*    data

) {

  CRK::Prim me;

  switch(type) {

  case CRK::SPRITE_FRAME:
    me=this->make_sprite_frame(
      (CRK::Build_Sprite*) data

    );

    break;

  };

  m_hed.vcount+=me.verts.size();
  m_hed.icount+=me.indices.size();

  m_data.push_back(me);

};

// ---   *   ---   *   ---
