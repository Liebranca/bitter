#ifndef __24_T3D_H__
#define __24_T3D_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/GLM.hpp"

// ---   *   ---   *   ---
// consts

  CX mat4 IDENTITY(1);

  CX vec3 X_AXIS( 1, 0, 0);
  CX vec3 Y_AXIS( 0, 1, 0);
  CX vec3 Z_AXIS( 0, 0,-1);

  CX vec3 Y_VOID(-1, 1, 0);

  CX vec3 ID_VEC3(0,0,0);
  CX quat ID_QUAT(1,0,0,0);

// ---   *   ---   *   ---
// info

class T3D {

public:

  VERSION   "v2.01.0";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// helpers

  struct Facing {

    vec3 fwd = Z_AXIS;
    vec3 hax = X_AXIS;
    vec3 up  = Y_AXIS;

    // cstruc
    Facing(vec3 fwd);
    Facing& operator=(vec3 fwd);
    void set(vec3 fwd);
    void calc(void);

    // ctrash
    Facing(void) {};

  };

// ---   *   ---   *   ---

private:

  // spares recalculation of
  // model matrix
  struct Model_Cache {

    mat4 model;
    mat3 nmat;

    union {

      struct {
        uint8_t igpar: 1;
        uint8_t needs_update: 1;

      };

      uint8_t flags;

    };

  };

// ---   *   ---   *   ---
// attrs

  Model_Cache     m_cache;
  svec<sref<T3D>> m_parent;

  vec3   m_position;
  quat   m_orientation;
  vec3   m_scaling;

  Facing m_facing;

public:

// ---   *   ---   *   ---
// iface

  T3D(
    vec3 pos   = {0,0,0},
    quat rot   = {1,0,0,0},
    vec3 scale = {1,1,1}

  ):

  // holy initializers batman
  m_position(pos),
  m_orientation({1,0,0,0}),
  m_scaling(scale) {
    this->rotate(rot);

  };

// ---   *   ---   *   ---

  ~T3D(void) {};

  mat4  calc_model(bool igpar=false);
  mat4& get_model(bool igpar=false);

  inline mat3& get_nmat(void) {
    return m_cache.nmat;

  };

  // fools the C++ standard
  inline void set_parent(T3D& par) {

    if(! m_parent.size()) {
      m_parent.push_back(par);

    } else {
      m_parent[0]=par;

    };

    m_cache.needs_update=1;

  }

  // selfex
  void rotate(quat delta);

  // update facing vectors
  // accto orientation
  void calc_facing(void);

  // add vel to position
  inline void move(vec3& vel) {
    m_position+=vel;
    m_cache.needs_update=1;

  };

  // ^overwrite
  inline void teleport(vec3& pos) {
    m_position=pos;
    m_cache.needs_update=1;

  };

  // getters
  // careful: these do not trigger update
  inline vec3& position(void) {
    return m_position;

  };

  inline quat& orientation(void) {
    return m_orientation;

  };

  // get facing vectors
  inline vec3& fwd(void) {
    return m_facing.fwd;

  };

  inline vec3& up(void) {
    return m_facing.up;

  };

  inline vec3& hax(void) {
    return m_facing.hax;

  };

  // apply model matrix to point
  vec2 point_model(vec2& p);
  vec3 point_model(vec3& p);
  vec4 point_model(vec4& p);

  // ^inverse for normalized
  vec2 point_nmat(vec2& p);
  vec3 point_nmat(vec3& p);
  vec4 point_nmat(vec4& p);

  // get quaternion from angle-axis
  static quat qang(float ang,vec3 ax);

  // get model needs recalc
  inline bool get_updated(void) {
    return m_cache.needs_update;

  };

};

// ---   *   ---   *   ---

#endif // __24_T3D_H__
