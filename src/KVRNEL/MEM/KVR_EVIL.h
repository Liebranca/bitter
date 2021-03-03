#ifndef __KVR_EVIL_H__
#define __KVR_EVIL_H__

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---
// some shorthands

#define CASSERT(boo, dummy)                 /* just a prettier name for static assert      */\
    _Static_assert(boo, dummy)

#define elif else if                        /* and prettier not having that space there    */

#define FATAL -255                          /* also prettier than return -num or some      */
#define ERROR -254                          /* other int errcode; return ERROR is cleaner  */

typedef void (*stark)(void* buff);          // pointer to void func taking a void* arg

//   ---     ---     ---     ---     ---
/* utils */                                 /* generally i wouldn't define shit in a  *
                                             * fucking header but i can't make this   *
                                             * work otherwise; the macros need all    *
                                             * of it to be here, else other files     *
                                             * can't make much use of them            */

static char* __shpath(char* path)           {
                                            // shortens filepath to everything after "/src/"
    char* file    = path;
    char* curchar = '\0';

    int src[3]    = { 0, 0, 0 };

    while (*path++)                         // iter through string

    {   curchar = path;                     /* check if current char is a slash            *
                                             * if so, check that next three match "src"    */
        if (  (*curchar == '\\')
           || (*curchar == '/' )  )         { src[0] = *(path+1) == 's';
                                              src[1] = *(path+2) == 'r';
                                              src[2] = *(path+3) == 'c';

                                            // shorten file string on success
            if(src[0]*src[1]*src[2])        { file = path+5; break; }                       };

    } return file;                          /* original path is returned on failure */      };

void __writoa  (int x, char* buff, int rdx);// wraps itoa
int  __wrstrcmp(char* s0, char* s1        );// wraps strcmp

//   ---     ---     ---     ---     ---

typedef struct KVR_DANGERCALL {             // records info for calls that __can__ fail

    char location[256];                     // line that made the call
    int  depth;                             // is call parent or child of another
    int  state;                             // whether this call was succesful or not

} DANG;

//   ---     ---     ---     ---     ---

DANG* __geterrloc (const char* p,
                   const char* f, int l);   // get error location

void __printcalreg(int flush              );// prints __registered__ calls
void __popcalreg  (                       );// pop from call register
void __terminator (int errcode, char* info);// goes back in time to save john connor

//   ---     ---     ---     ---     ---
// some bug-tracking macros

#define ERRLOC                              /* macro paste for error locations             */\
    __shpath(__FILE__), __func__, __LINE__

#define GETLOC __geterrloc(ERRLOC)          /* spam wherever something may go wrong        */

#define CALL(func, retx, errcode, info)     { DANG* cal = GETLOC;                            \
                                                                                             \
    cal->state            = func;                                                            \
    if  ( (cal->state    == FATAL        )                                                   \
        | (cal->state    == ERROR        )                                                   \
        | (retx && (retx != cal->state)) )  { __terminator(errcode, info);                  }\
                                                                                             \
    retx = cal->state; __popcalreg();                                                       }

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_EVIL_H__