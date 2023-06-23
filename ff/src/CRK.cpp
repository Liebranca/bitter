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
  #include "kvrnel/Seph.hpp"

  #include "ff/CRK.hpp"

// ---   *   ---   *   ---
// fill out struct from
// uncompressed data

void CRK::Vertex::set_xyz(vec3& co) {

  uint32_t b=CRK::pseph().pack(co);

  XYZ[0]=b&0xFF;b >>= 8;
  XYZ[1]=b&0xFF;b >>= 8;
  XYZ[2]=b&0xFF;

};

void CRK::Vertex::set_n(vec3& n) {
  NTB[0]=CRK::nseph().pack(n) & 0xFFFF;

};

void CRK::Vertex::set_ntb(
  vec3& n,
  vec3& t,
  vec3& b

) {

  NTB[0]=CRK::nseph().pack(n) & 0xFFFF;
  NTB[1]=CRK::nseph().pack(t) & 0xFFFF;
  NTB[2]=CRK::nseph().pack(b) & 0xFFFF;

};

void CRK::Vertex::set_uv(
  vec2& uv

) {

  TEX[0]=frac<uint8_t>(

    uv[0],

    Frac::STEP[Frac::STEP_8BIT],
    Frac::BITS[Frac::SIZE_8BIT],

    Frac::UNSIGNED

  );

  TEX[1]=frac<uint8_t>(

    uv[1],

    Frac::STEP[Frac::STEP_8BIT],
    Frac::BITS[Frac::SIZE_8BIT],

    Frac::UNSIGNED

  );

};

// ---   *   ---   *   ---
// ^undo, mostly for debug

vec3 CRK::Vertex::get_xyz(void) {

  return CRK::pseph().unpack(
    uint64_t(XYZ[0] <<  0)
  | uint64_t(XYZ[1] <<  8)
  | uint64_t(XYZ[2] << 16)

  );

};

vec3 CRK::Vertex::get_n(void) {
  return CRK::nseph().unpack(NTB[0]);

};

vec2 CRK::Vertex::get_uv(void) {

  float x=unfrac<uint8_t>(

    TEX[0],

    Frac::STEP[Frac::STEP_8BIT],
    Frac::BITS[Frac::SIZE_8BIT],

    Frac::UNSIGNED

  );

  float y=unfrac<uint8_t>(

    TEX[1],

    Frac::STEP[Frac::STEP_8BIT],
    Frac::BITS[Frac::SIZE_8BIT],

    Frac::UNSIGNED

  );

  return {x,y};

};

// ---   *   ---   *   ---

void CRK::open(
  std::string fpath,
  char        mode

) {

  this->defopen<CRK::Header>(
    fpath,mode

  );

};

// ---   *   ---   *   ---
// new file

CRK::CRK(
  std::string       fpath,
  CRK::Mesh_Builds& blds

): Bin() {

  this->open(fpath,Bin::NEW);
  this->build(blds);

};

// ---   *   ---   *   ---
// ^from *.joj

CRK::CRK(
  std::string fpath,
  JOJ&        joj

) {

  this->open(fpath,Bin::NEW);

  CRK::Mesh_Builds  blds;
  CRK::Sprite_Build frames={

    .scale = {
      joj.get_tile_scale(),
      joj.get_atlas_scale()

    },

    .atlas = joj.get_atlas_desc()

  };

  CRK::Mesh_Build bld={
    .type=CRK::SPRITE,
    .data=&frames

  };

  blds.push_back(bld);
  this->build(blds);

};

// ---   *   ---   *   ---
// from raw

CRK::CRK(
  std::string fpath,
  CRK::Tris&  tris

) {

  this->open(fpath,Bin::NEW);

  CRK::Mesh_Builds  blds;

  CRK::Mesh_Build bld={
    .type=CRK::TRIMESH,
    .data=&tris

  };

  blds.push_back(bld);
  this->build(blds);

};

// ---   *   ---   *   ---
// read file

CRK::CRK(
  std::string fpath

): Bin() {

  this->open(fpath,Bin::READ);

};

// ---   *   ---   *   ---

void CRK::build(CRK::Mesh_Builds& bld) {

  for(CRK::Mesh_Build p : bld) {
    this->make_prim(p.type,p.data);

  };

};

// ---   *   ---   *   ---
// size of primitive in bytes

uint64_t CRK::Prim::bytesz(void) {

  uint64_t out=0;

  out+=verts.size()*sizeof(verts[0]);
  out+=indices.size()*sizeof(indices[0]);

  return out;

};

// ---   *   ---   *   ---
// primitive to bytearray

void CRK::Prim::to_buff(
  Mem<uint8_t>& dst,
  uint64_t&     ptr

) {

  uint64_t sz;
  uint16_t cnt[2]={
    (uint16_t) verts.size(),
    (uint16_t) indices.size()

  };

  sz   = sizeof(cnt[0])*arrsize(cnt);
  dst.write(&cnt[0],sz,ptr);

  ptr += sz;
  sz   = cnt[0]*sizeof(verts[0]);
  dst.write(verts.data(),sz,ptr);

  ptr += sz;
  sz   = cnt[1]*sizeof(indices[0]);

  // indices are skipped for 2D sprites
  // as the pattern can be generated
  if(sz) {
    dst.write(indices.data(),sz,ptr);
    ptr+=sz;

  };

  sz=sizeof(vec3);
  dst.write(&box[0],sz,ptr);

};

// ---   *   ---   *   ---
// ^inverse

void CRK::Prim::from_buff(
  Mem<uint8_t>& src,
  uint64_t&     ptr

) {

  // get size of parts
  uint16_t cnt[2]={
    reipret(uint16_t,src[ptr+0]),
    reipret(uint16_t,src[ptr+2]),

  };

  uint64_t sz;

  ptr+=sizeof(cnt[0])*arrsize(cnt);

  verts.resize(cnt[0]);
  indices.resize(cnt[1]);

  sz=sizeof(verts[0])*cnt[0];
  memcpy(verts.data(),&src[ptr],sz);

  ptr += sz;
  sz   = sizeof(indices[0])*cnt[1];

  // indices skipped for 2D sprites
  if(sz) {
    memcpy(indices.data(),&src[ptr],sz);
    ptr+=sz;

  };

  sz=sizeof(vec3);
  memcpy(&box[0],&src[ptr],sz);

};

// ---   *   ---   *   ---
// mod index buff for line drawing

void CRK::Prim::tris_to_lines(void) {

  uint64_t sz = indices.size();
  uint64_t j  = 0;

  // new vec is sz + [1 per every 3rd idex]
  std::vector<uint16_t> mod;
  mod.resize(sz+(sz/3));

  // walk current
  for(uint64_t i=0;i<sz;i+=3) {

    // A to B
    mod[j++]=indices[i+0];
    mod[j++]=indices[i+1];

    // B to C
    mod[j++]=indices[i+1];
    mod[j++]=indices[i+2];

  };

  // ^subst
  indices=mod;

};

// ---   *   ---   *   ---
// ^reverse

void CRK::Prim::lines_to_tris(void) {

  uint64_t sz = indices.size();
  uint64_t j  = 0;

  // new vec is sz - [1 per every 4th idex]
  std::vector<uint16_t> mod;
  mod.resize(sz-(sz/4));

  // walk current
  for(uint64_t i=0;i<sz;i+=4) {

    // A to B to C
    mod[j++]=indices[i+0];
    mod[j++]=indices[i+1];
    mod[j++]=indices[i+3];

  };

  // ^subst
  indices=mod;

};

// ---   *   ---   *   ---
// generate zeroed buffer

CRK::Prim::Prim(uint16_t size) {
  verts.resize(size);
  indices.resize(size);

};

// ---   *   ---   *   ---
// generates index array for
// simple quad arrays

void CRK::Prim::gen_qa_indices(void) {

  // 2 additional indices per
  // every 4 verts
  uint64_t sz=verts.size();
  sz+=2*(sz/4);

  // ensure enough room
  indices.resize(sz);
  uint16_t vi=0;

  // calc pattern
  for(uint16_t i=0;i<sz;i+=6) {

    // tri A
    indices[i+0]=vi+0;
    indices[i+1]=vi+3;
    indices[i+2]=vi+2;

    // tri B
    indices[i+3]=vi+0;
    indices[i+4]=vi+2;
    indices[i+5]=vi+1;

    vi+=4;

  };

};

// ---   *   ---   *   ---
// util for dumping mesh to file

Mem<uint8_t> CRK::mesh_to_buff(void) {

  uint64_t sz  = 0;
  uint64_t ptr = 0;

  for(auto& prim : m_data) {
    sz+=
      prim.bytesz()
    + sizeof(uint16_t)*2
    ;

  };

  Mem<uint8_t> out(sz);

  for(auto& prim : m_data) {
    prim.to_buff(out,ptr);

  };

  return Mem<uint8_t>(out);

};

// ---   *   ---   *   ---
// ^reverse

void CRK::buff_to_mesh(
  Mem<uint8_t>& src

) {

  uint64_t ptr=0;
  m_data.resize(m_hed.prim_cnt);

  for(uint64_t i=0;i<m_hed.prim_cnt;i++) {
    m_data[i].from_buff(src,ptr);

  };

};

// ---   *   ---   *   ---
// dump to disk

void CRK::write(void) {

  m_hed.prim_cnt=m_data.size();
  this->write_header(&m_hed);

  auto m=this->mesh_to_buff();
  Bin::write(&m[0],m.used());

};

// ---   *   ---   *   ---
// ^retrieve

void CRK::read(void) {

  auto m=Bin::read();
  this->buff_to_mesh(m);

};

// ---   *   ---   *   ---
// placeholders

void CRK::pack(void) {
  this->write();

};

void CRK::unpack(void) {
  this->read();

};

// ---   *   ---   *   ---
// build triangle

void CRK::Tri::pack(CRK::Prim& me) {

  uint16_t i=me.verts.size();

  // fill out struct
  for(auto& p : m_points) {

    me.verts.push_back(CRK::Vertex());
    auto& vert=me.verts.back();

    vert.set_xyz(p);
    vert.set_n(m_normal);

  };

  // make index array for this tri
  std::vector<uint16_t> iar {

    uint16_t(i+0),
    uint16_t(i+1),
    uint16_t(i+2),

    uint16_t(i+2),
    uint16_t(i+3),
    uint16_t(i+0),

  };

  // ^quads implicitly accepted
  uint16_t vcount = m_points.size();
  uint8_t  limit  = (vcount==3) ? 3 : 6;

  // ^append to me's array
  for(uint8_t j=0;j<limit;j++) {
    me.indices.push_back(iar[j]);

  };

};

// ---   *   ---   *   ---
// ^batch

CRK::Prim CRK::make_trimesh(
  CRK::Tris* bld

) {

  CRK::Prim me;
  for(auto& tri : *bld) {
    tri.pack(me);

  };

  return me;

};

// ---   *   ---   *   ---

CRK::Prim CRK::make_sprite_frame(
  CRK::Frame_Build* bld

) {

  CRK::Prim       me;
  CRK::Quad_Build qbld(&me);

  uint64_t tile_cnt = bld->img.size();

  qbld.co_scale     = bld->scale[0];
  qbld.uv_scale     = bld->scale[1];

  // 4 verts (2 tris) per tile
  me.verts.resize(4*tile_cnt);

  // push one quad per tile
  for(uint64_t i=0;i<tile_cnt;i++) {

    qbld.td    = bld->img[i];
    qbld.layer = 0;
    qbld.id    = i+(m_hed.vcount/4);

    qbld.push_quad();

  };

  return me;

};

// ---   *   ---   *   ---
// ^gives all frames through atlas

void CRK::make_sprite(
  CRK::Sprite_Build* bld

) {

  uint64_t i=0;

  for(JOJ::Img_Desc& img : bld->atlas) {

    if(i==bld->atlas.size()-1) {
      break;

    };

    CRK::Frame_Build frame={

      .scale = {
        bld->scale[0],
        bld->scale[1]

      },

      .img   = img

    };

    this->make_prim(CRK::FRAME,&frame);
    i++;

  };

};

// ---   *   ---   *   ---

void CRK::make_prim(

  uint16_t type,
  void*    data

) {

  CRK::Prim me;

  switch(type) {

  case CRK::FRAME:
    me=this->make_sprite_frame(
      (CRK::Frame_Build*) data

    );

    break;

  case CRK::SPRITE:

    this->make_sprite(
      (CRK::Sprite_Build*) data

    );

    return;

  case CRK::TRIMESH:
    me=this->make_trimesh(
      (CRK::Tris*) data

    );

    break;

  case CRK::BMESH:
    auto& crk=*this;
    ((CRK::Bmesh*) data)->build(crk);

    return;

  };

  this->push_pose(me);

};

// ---   *   ---   *   ---
