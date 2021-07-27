// *   ---     ---     ---     ---     ---  *
// *  KVR_PATHS                             *
// *                                        *
// *  -stepbrother to zjc_evil              *
// *  -holds some strings with paths        *
// *  -^setters & getters for em            *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __KVR_PATHS_H__
#define __KVR_PATHS_H__

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

#define DECL_KVR_PATH(NAME)                 \
    char* ST##NAME (char* buff  );          \
    char* GT##NAME (            )

//   ---     ---     ---     ---     ---

DECL_KVR_PATH     (RT          );
DECL_KVR_PATH     (CWD         );
DECL_KVR_PATH     (PERT        );
DECL_KVR_PATH     (PECWD       );

// [...]

void NTKVRPTH     (char** pth_l);
void DLKVRPTH     (            );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_PATHS_H__