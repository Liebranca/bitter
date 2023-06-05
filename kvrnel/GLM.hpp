#ifndef __24_GLM_H__
#define __24_GLM_H__

// ---   *   ---   *   ---
// deps

  #include <glm/glm.hpp>
  #include <glm/gtx/quaternion.hpp>
  #include <glm/gtc/matrix_inverse.hpp>

  #include "bitter/kvrnel/Style.hpp"

// ---   *   ---   *   ---
// cute shorthands
//
// could fix through 'using';
// refusal of pettiness

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

typedef glm::uvec2 uvec2;
typedef glm::uvec3 uvec3;
typedef glm::uvec4 uvec4;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

typedef glm::quat quat;

// ---   *   ---   *   ---

#endif // __24_GLM_H__
