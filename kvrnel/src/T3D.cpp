// ---   *   ---   *   ---
// TRANSFORM 3D
// For all your spatial needs
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <glm/gtx/transform.hpp>

  #include "kvrnel/Seph.hpp"
  #include "kvrnel/T3D.hpp"

// ---   *   ---   *   ---
// recalculates model matrix

mat4 T3D::calc_model(bool igpar) {

  mat4 parmat;

  mat4 wpos=glm::translate(m_position);
  mat4 wrot=mat4_cast(
    glm::normalize(m_orientation)

  );

  mat4 wscale=glm::scale(m_scaling);

  parmat=(! m_parent.size() || igpar)
    ? IDENTITY
    : m_parent[0].get().get_model()
    ;

  // TODO: separately frac xforms!
  return wpos * wrot * wscale;

};

// ---   *   ---   *   ---
// ^caches results

void T3D::full_update(bool igpar) {

  m_cache.model = this->calc_model(igpar);
  m_cache.nmat  = glm::inverseTranspose(
    m_cache.model

  );

  m_cache.needs_update=0;

};

// ---   *   ---   *   ---
// ^fetches or recalculates

mat4& T3D::get_model(bool igpar) {

  if(

     m_cache.igpar != igpar
  || m_cache.needs_update

  ) {

    this->full_update(igpar);

  };

  return m_cache.model;

};

// ---   *   ---   *   ---

void T3D::rotate(quat delta) {

  m_orientation=glm::normalize(
    m_orientation*delta

  );

  m_cache.needs_update=1;

};

// ---   *   ---   *   ---
// ^update facing vectors
// accto orientation

void T3D::calc_facing(void) {

  m_facing=glm::normalize(
    Z_AXIS * m_orientation

  );

};

// ---   *   ---   *   ---
// cstruc

T3D::Facing::Facing(vec3 fwd) {
  this->set(fwd);

};

T3D::Facing& T3D::Facing::operator=(vec3 fwd) {
  this->set(fwd);
  return *this;

};

void T3D::Facing::set(vec3 fwd) {
  this->fwd=fwd;
  this->calc();

};

// ---   *   ---   *   ---
// ^update facing vectors from fwd

void T3D::Facing::calc(void) {

  vec3 yax=(fabs(this->fwd.y) >= 0.99f)
    ? Y_VOID
    : Y_AXIS
    ;

  this->hax=glm::normalize(
    glm::cross(yax,this->fwd)

  );

  this->up=glm::normalize(
    glm::cross(this->fwd,this->hax)

  );

};

// ---   *   ---   *   ---
// apply model matrix to point

vec2 T3D::point_model(vec2& p) {
  auto& model=this->get_model();
  return vec2(model * vec4(p.x,p.y,0,1));

};

vec3 T3D::point_model(vec3& p) {
  auto& model=this->get_model();
  return vec3(model * vec4(p.x,p.y,p.z,1));

};

vec4 T3D::point_model(vec4& p) {
  auto& model=this->get_model();
  return model * p;

};

// ---   *   ---   *   ---
// ^inverse for normalized

vec2 T3D::point_nmat(vec2& p) {
  auto& nmat=this->get_nmat();
  return vec2(nmat * vec3(p.x,p.y,0));

};

vec3 T3D::point_nmat(vec3& p) {
  auto& nmat=this->get_nmat();
  return nmat * p;

};

vec4 T3D::point_nmat(vec4& p) {
  auto& nmat=this->get_nmat();
  return vec4(nmat * vec3(p),p.w);

};

// ---   *   ---   *   ---
// get quaternion from angle-axis

quat T3D::qang(float ang,vec3 ax) {

  float sang=float(sin(ang));

  return {

    float(cos(ang)),

    ax.x * sang,
    ax.y * sang,
    ax.z * sang

  };

};

// ---   *   ---   *   ---
