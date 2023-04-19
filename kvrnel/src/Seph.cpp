// ---   *   ---   *   ---
// SEPH
// Se-pherical coords ;>
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <cmath>

  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Seph.hpp"

// ---   *   ---   *   ---
// gets inclination and azimuth angles
// from normalized vector

uint64_t Seph::angle_pack(glm::vec3& n) {

  Frac::Rounding_Mode=Frac::NVEC;
  uint64_t out=0x00;

  // get coords as angles
  float zen=acos(n.y);
  float azi=atan2(n.x,n.z);

  // ^pack values
  out|=frac<uint64_t>(

    zen,

    m_zen_step,
    m_zen_nbits,

    Frac::SIGNED

  ) << 0;

  out|=frac<uint64_t>(

    azi,

    m_azi_step,
    m_azi_nbits,

    Frac::SIGNED

  ) << m_zen_nbits;

  return out;

};

// ---   *   ---   *   ---
// ^undo

glm::vec3 Seph::angle_unpack(uint64_t b) {

  Frac::Rounding_Mode=Frac::NVEC;

  // retrieve packed elements
  uint64_t bzen = b&m_zen_mask;
  b >>= m_zen_nbits;

  uint64_t bazi = b&m_azi_mask;

  // float-ify
  float zen=unfrac<uint64_t>(

    bzen,

    m_zen_step,
    m_zen_nbits,

    Frac::SIGNED

  );

  float azi=unfrac<uint64_t>(

    bazi,

    m_azi_step,
    m_azi_nbits,

    Frac::SIGNED

  );

  float szen = sin(zen);

  float x    = szen * sin(azi);
  float y    = cos(zen);
  float z    = szen * cos(azi);

  return {x,y,z};

};

// ---   *   ---   *   ---
// magnitude of vector eq sphere radius

uint64_t Seph::radius_pack(glm::vec3& p) {

  Frac::Rounding_Mode=Frac::LINE;

  return frac<uint64_t>(

    p.length(),

    m_rad_step,
    m_rad_nbits,

    Frac::UNSIGNED

  );

};

float Seph::radius_unpack(uint64_t b) {

  Frac::Rounding_Mode=Frac::LINE;

  return unfrac<uint64_t>(

    b,

    m_rad_step,
    m_rad_nbits,

    Frac::UNSIGNED

  );

};

// ---   *   ---   *   ---
// cstruc

void Seph::set(

  uint32_t mode,

  uint32_t rad_nbits,
  uint32_t zen_nbits,
  uint32_t azi_nbits

) {

  m_mode      = mode;

  m_rad_nbits = rad_nbits;
  m_zen_nbits = zen_nbits;
  m_azi_nbits = azi_nbits;

  // bmask for unpack
  m_rad_mask  = (1 << rad_nbits)-1;
  m_zen_mask  = (1 << zen_nbits)-1;
  m_azi_mask  = (1 << azi_nbits)-1;

  // get wall
  m_rad_maxp  = 1 << (rad_nbits-2);
  m_zen_maxp  = 1 << (zen_nbits-1);
  m_azi_maxp  = 1 << (azi_nbits-1);

  // get precision
  m_rad_step  = 1.0f / m_rad_maxp;
  m_zen_step  = PI   / m_zen_maxp;
  m_azi_step  = PI   / m_azi_maxp;

};

// ---   *   ---   *   ---
// vector to packed spherical coords

uint64_t Seph::pack(glm::vec3& a) {

  if(m_mode==NORMAL) {
    return this->angle_pack(a);

  } else {

    uint64_t radius = this->radius_pack(a);

    a=glm::normalize(a);
    uint64_t angle  = this->angle_pack(a);

    return radius | (angle << m_rad_nbits);

  };

};

// ---   *   ---   *   ---
// ^undo

glm::vec3 Seph::unpack(uint64_t b) {

  if(m_mode==NORMAL) {
    return angle_unpack(b);

  } else {

    float r=this->radius_unpack(b);
    b >>= m_rad_nbits;

    glm::vec3 n=this->angle_unpack(b);

    return n*r;

  };

};

// ---   *   ---   *   ---
