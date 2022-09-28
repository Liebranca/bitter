#ifndef __LYM_QUAT_H__
#define __LYM_QUAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vec4.h"

//   ---     ---     ---     ---     ---

// only so we don't get confused
typedef vec4 quat;

char lenq(quat a);
quat normq(quat a);
quat conjq(quat a);

quat mulq(quat a,quat b);
quat qaxang(vec4 v,char ang);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYM_QUAT_H__