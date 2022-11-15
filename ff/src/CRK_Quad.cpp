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

  uint16_t co[2]={
    uint16_t(desc&0xFFFF),
    uint16_t((desc>>16)&0xFFFF)

  };

  uint16_t uv[2]={
    uint16_t((desc>>32)&0xFFFF),
    uint16_t((desc>>48)&0xFFFF)

  };

  float raw[]={
    float(co[0])*co_scale,0,
    float(co[1])*co_scale,0,
    float(uv[0])*uv_scale,0,
    float(uv[1])*uv_scale,0

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

    me.verts[v+i]=(CRK::Vertex) {

      .XYZ = {
        co_x[j[0]],
        co_y[j[1]],

        layer

      },

      .TEX = {
        uv_x[j[0]],
        uv_y[j[1]]

      },

      .ID  = id

    };

  };

};

// ---   *   ---   *   ---
// fills out the index pattern

void CRK::Quad_Build::get_indices(void) {

  me.indices[idex+0]=vert+0;
  me.indices[idex+1]=vert+1;
  me.indices[idex+2]=vert+2;

  me.indices[idex+3]=vert+2;
  me.indices[idex+4]=vert+3;
  me.indices[idex+5]=vert+0;

};

// ---   *   ---   *   ---
