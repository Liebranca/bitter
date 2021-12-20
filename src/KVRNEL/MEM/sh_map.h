#ifndef __SH_MAP_H__
#define __SH_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define SHM_STEP 0x0800

//   ---     ---     ---     ---     ---

// he shot first
void** ntmap_solo(
  const char* fpath,
  size_t fsz

);

// (child process) get mmapped file
void** ntmap_heir(char* key);

// generate maphed key
char* encodemh(void** mh_b);

// get file descriptor for map
int mapfd(void** mh_a);

// (parent process) get mmapped file
void** ntmap_origin(const char* fpath,int pages);

// cleanup
void dlmap(void** mh_b);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SH_MAP_H__