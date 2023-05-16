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
  out|=frac<int64_t>(

    zen,

    m_zen_step,
    m_zen_nbits-1,

    Frac::SIGNED

  ) << 0;

  out|=frac<int64_t>(

    azi,

    m_azi_step,
    m_azi_nbits-1,

    Frac::SIGNED

  ) << m_zen_nbits;

  return out;

};

// ---   *   ---   *   ---
// ^undo

glm::vec3 Seph::angle_unpack(uint64_t b) {

  // retrieve packed elements
  uint64_t bzen = b&m_zen_mask;
  b >>= m_zen_nbits;

  uint64_t bazi = b&m_azi_mask;

  // float-ify
  float zen=unfrac<int64_t>(

    bzen,

    m_zen_step,
    m_zen_nbits-1,

    Frac::SIGNED

  );

  float azi=unfrac<int64_t>(

    bazi,

    m_azi_step,
    m_azi_nbits-1,

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

    glm::length(p),

    m_rad_step,
    m_rad_nbits-1,

    Frac::UNSIGNED

  );

};

float Seph::radius_unpack(uint64_t b) {

  return unfrac<uint64_t>(

    b&m_rad_mask,

    m_rad_step,
    m_rad_nbits-1,

    Frac::UNSIGNED

  );

};

// ---   *   ---   *   ---
// byte-sized rotations ;>

uint64_t Seph::quat_pack(glm::quat& q) {

  Frac::Rounding_Mode=Frac::LINE;
  uint64_t out=0x00;

  // ^pack values
  out|=frac<int64_t>(

    q.w,

    m_rad_step,
    m_rad_nbits-1,

    Frac::SIGNED

  ) << 0;

  out|=frac<int64_t>(

    q.x,

    m_zen_step,
    m_zen_nbits-1,

    Frac::SIGNED

  ) << m_rad_nbits;

  out|=frac<int64_t>(

    q.y,

    m_azi_step,
    m_azi_nbits-1,

    Frac::SIGNED

  ) << (m_rad_nbits + m_zen_nbits);

  out|=frac<int64_t>(

    q.z,

    m_zen_step,
    m_zen_nbits-1,

    Frac::SIGNED

  ) << (m_rad_nbits + m_zen_nbits + m_azi_nbits);

  return out;

};

glm::quat Seph::quat_unpack(uint64_t b) {

  uint64_t bw=b&m_rad_mask;
  b >>= m_rad_nbits;

  uint64_t bx=b&m_zen_mask;
  b >>= m_zen_nbits;

  uint64_t by=b&m_azi_mask;
  b >>= m_azi_nbits;

  uint64_t bz=b&m_zen_mask;

  // float-ify
  float w=unfrac<int64_t>(

    bw,

    m_rad_step,
    m_rad_nbits-1,

    Frac::SIGNED

  );

  float x=unfrac<int64_t>(

    bx,

    m_zen_step,
    m_zen_nbits-1,

    Frac::SIGNED

  );

  float y=unfrac<int64_t>(

    by,

    m_azi_step,
    m_azi_nbits-1,

    Frac::SIGNED

  );

  float z=unfrac<int64_t>(

    bz,

    m_zen_step,
    m_zen_nbits-1,

    Frac::SIGNED

  );

  glm::quat q(w,x,y,z);
  return glm::normalize(q);

};

// ---   *   ---   *   ---
// handling of *esoteric* pixel type
// holding normal+curvature

uint64_t Seph::nc_pack(float* pixel) {

  // color to vector
  glm::vec3 n={
    (pixel[0]*2)-1,
    (pixel[1]*2)-1,
    (pixel[2]*2)-1

  };

  // normal data as spherical coords
  n=glm::normalize(n);
  uint64_t angle=this->angle_pack(n);

  // ^pack alpha/curvature
  uint64_t curv=frac<uint64_t>(

    pixel[3],

    m_rad_step,
    m_rad_nbits-1,

    Frac::UNSIGNED

  );

  return curv | (angle << m_rad_nbits);

};

void Seph::nc_unpack(float* dst,uint64_t b) {

  float curv=unfrac<uint64_t>(

    b&m_rad_mask,

    m_rad_step,
    m_rad_nbits-1,

    Frac::UNSIGNED

  );

  b>>=m_rad_nbits;

  glm::vec3 n=this->angle_unpack(b);

  // vector to color
  dst[0]=(n.x+1)/2;
  dst[1]=(n.y+1)/2;
  dst[2]=(n.z+1)/2;

  // alpha channel
  dst[3]=curv;

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
  m_rad_maxp  = (m_mode==Seph::NC)
    ? 1 << (rad_nbits)
    : 1 << (rad_nbits-2)
    ;

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
