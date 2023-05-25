// ---   *   ---   *   ---
// CRK QUAD
// Builds squares
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
// break down descriptor into coords

void CRK::Quad_Build::calc_verts(void) {

  float raw[]={
    float(td.dx)*co_scale,0,
    float(td.dy)*co_scale,0,
    float(td.x)*uv_scale,0,
    float(td.y)*uv_scale,0

  };

  raw[1]=raw[0]+co_scale;
  raw[3]=raw[2]+co_scale;
  raw[5]=raw[4]+uv_scale;
  raw[7]=raw[6]+uv_scale;

  for(int i=0;i<8;i++) {

    data[i]=frac<uint8_t>(

      raw[i],

      CRK::CO_STEP,
      CRK::CO_BITS,

      Frac::UNSIGNED

    );

  };

};

// ---   *   ---   *   ---
// ^make quad from descriptor

void CRK::Quad_Build::get_verts(void) {

  this->calc_verts();

  uint8_t pattern[]={
    1,0,
    0,0,
    0,1,
    1,1

  };

  for(int i=0;i<4;i++) {

    int j[]={i*2,(i*2)+1};

    j[0]=pattern[j[0]];
    j[1]=pattern[j[1]];

    me->verts[vert+i]=(CRK::Vertex) {

      .XYZ = {
        co_x[j[0]],
        co_y[j[1]],

        layer

      },

      .TEX = {
        uv_x[j[0]],
        uv_y[j[1]]

      },

    };

  };

};

// ---   *   ---   *   ---
// rotates uvs 90 degrees clockwise

void CRK::Quad_Build::uv_ror(void) {

  for(uint8_t i=0;i<4;i++) {

    auto& vrt  = me->verts[vert+i];
    auto  cpy  = me->verts[vert+((i+1)&3)];

    vrt.TEX[0] = cpy.TEX[0];
    vrt.TEX[1] = cpy.TEX[1];

  };

};

// ---   *   ---   *   ---
// rotates uvs 90 degrees anti-clockwise

void CRK::Quad_Build::uv_rol(void) {

  for(uint8_t i=0;i<4;i++) {

    auto& vrt  = me->verts[vert+i];
    auto  cpy  = me->verts[vert+((i-1)&3)];

    vrt.TEX[0] = cpy.TEX[0];
    vrt.TEX[1] = cpy.TEX[1];

  };

};

// ---   *   ---   *   ---
// mirrors uvs on x axis

void CRK::Quad_Build::uv_xmir(void) {

  for(uint8_t i=0;i<4;i+=2) {

    auto& vrt  = me->verts[vert+i];
    auto  cpy  = me->verts[vert+vert+((i+1)&3)];

    vrt.TEX[0] = cpy.TEX[0];
    vrt.TEX[1] = cpy.TEX[1];

  };

  for(uint8_t i=1;i<4;i+=2) {

    auto& vrt  = me->verts[vert+i];
    auto  cpy  = me->verts[vert+vert+((i-1)&3)];

    vrt.TEX[0] = cpy.TEX[0];
    vrt.TEX[1] = cpy.TEX[1];

  };

};

// ---   *   ---   *   ---
// mirrors uvs on y axis

void CRK::Quad_Build::uv_ymir(void) {

  for(uint8_t i=1;i<4;i+=2) {

    auto& vrt  = me->verts[vert+i];
    auto  cpy  = me->verts[vert+vert+((i+1)&3)];

    vrt.TEX[0] = cpy.TEX[0];
    vrt.TEX[1] = cpy.TEX[1];

  };

  for(uint8_t i=0;i<4;i+=2) {

    auto& vrt  = me->verts[vert+i];
    auto  cpy  = me->verts[vert+vert+((i-1)&3)];

    vrt.TEX[0] = cpy.TEX[0];
    vrt.TEX[1] = cpy.TEX[1];

  };

};

// ---   *   ---   *   ---

void CRK::Quad_Build::apply_xforms(void) {

  switch(td.rotated) {

  case JOJ::Tiles::ROT_180:
    this->uv_ror();

  case JOJ::Tiles::ROT_90:
    this->uv_ror();
    break;

  case JOJ::Tiles::ROT_240:
    this->uv_rol();
    break;

  };

  switch(td.mirrored) {

  case JOJ::Tiles::MIRROR_XY:
    this->uv_xmir();

  case JOJ::Tiles::MIRROR_Y:
    this->uv_ymir();
    break;

  case JOJ::Tiles::MIRROR_X:
    this->uv_xmir();
    break;

  };

};

// ---   *   ---   *   ---
