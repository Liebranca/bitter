#ifndef __24_SEPH_H__
#define __24_SEPH_H__

// ---   *   ---   *   ---
// deps

  #include <glm/glm.hpp>
  #include <glm/gtx/quaternion.hpp>

  #include "kvrnel/GLM.hpp"
  #include "kvrnel/Char_Sprite.hpp"

// ---   *   ---   *   ---
// info

class Seph {

public:

  VERSION   "v0.00.6b";
  AUTHOR    "IBN-3DILA";

  // pack/unpack modes
  enum {
    NORMAL,
    POINT,
    QUAT,
    NC

  };

  cxr32 PI  = 3.14159265f;
  cxr32 PI2 = PI*2;

// ---   *   ---   *   ---
// attrs

private:

  uint32_t m_mode;

  uint32_t m_rad_nbits;
  uint32_t m_zen_nbits;
  uint32_t m_azi_nbits;

  uint32_t m_rad_mask;
  uint64_t m_zen_mask;
  uint64_t m_azi_mask;

  uint32_t m_rad_maxp;
  uint64_t m_zen_maxp;
  uint64_t m_azi_maxp;

  float    m_rad_step;
  float    m_zen_step;
  float    m_azi_step;

// ---   *   ---   *   ---
// guts

  // gets inclination and azimuth angles
  // from normalized vector
  uint64_t angle_pack(vec3& n);

  // ^undo
  vec3 angle_unpack(uint64_t b);

  // magnitude of vector eq sphere radius
  uint64_t radius_pack(vec3& p);
  float radius_unpack(uint64_t b);

  // byte-sized rotations ;>
  uint64_t quat_pack(quat& q);
  quat quat_unpack(uint64_t);

// ---   *   ---   *   ---
// iface

public:

  // cstruc
  void set(

    uint32_t mode,

    uint32_t rad_nbits,
    uint32_t zen_nbits,
    uint32_t azi_nbits

  );

  // ctrash
  Seph(void) {};
  ~Seph(void) {};

  // cstruc
  Seph(

    uint32_t mode,

    uint32_t rad_nbits,
    uint32_t zen_nbits,
    uint32_t azi_nbits

  ) {

    this->set(
      mode,rad_nbits,zen_nbits,azi_nbits

    );

  };

  // entry points
  uint64_t pack(vec3& a);
  vec3 unpack(uint64_t b);

  // ^quat variant
  inline uint64_t qpack(quat& q) {
    return this->quat_pack(q);

  };

  inline quat qunpack(uint64_t b) {
    return this->quat_unpack(b);

  };

  // handling of *esoteric* pixel type
  // holding normal+curvature
  uint64_t nc_pack(float* pixel);
  void nc_unpack(float* dst,uint64_t b);

};

// ---   *   ---   *   ---

#endif // __24_SEPH_H__
