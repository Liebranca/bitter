#ifndef __SH_MAP_H__
#define __SH_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SHM_STEP 0x0800

//   ---     ---     ---     ---     ---

// (child process) get mmapped file
void** ntmap_heir(char* key);

// generate maphed key
char* encodemh(void** mh_b);

// get file descriptor for map
int mapfd(void** mh_a);

// (parent process) get mmapped file
void** ntmap_origin(char* fpath,int pages);

// cleanup
void dlmap(void** mh_b);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SH_MAP_H__