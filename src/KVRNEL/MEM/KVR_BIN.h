#ifndef __KVR_BIN_H__
#define __KVR_BIN_H__

#include "../zjc_evil.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct KVR_BIN {                    // helper struct for dealing with files

    char* path;                             // full path to file, including extension
    char* sign;                             // signature to compare
    char* mode;                             // read/write mode
    FILE* file;                             // handle to the file itself

} BIN;

int MKBIN(BIN* bin);                        // opens a bin file
int DLBIN(BIN* bin);                        // closes a bin file
int RMBIN(BIN* bin);                        // removes a bin file from disk

//   ---     ---     ---     ---     ---

#if KVR_DEBUG
                                            /* catch if error on open                      */
    #define BINOPEN(bin, retx)              { retx = 0;                                      \
        ERRCATCH(MKBIN(bin), retx, 0x40, bin->path);                                        }

                                            /* catch if error on close                     */
    #define BINCLOSE(bin)                   { int retx = 0;                                  \
        ERRCATCH(DLBIN(bin), retx , 0x41, bin->path);                                       }

                                            /* catch if we don't write as many bytes       */
    #define BINWRITE(bin, wb, type, count, buff){ wb = sizeof(type) * count;                 \
        ERRCATCH(fwrite(buff, sizeof(type), count, bin->file), wb, 0x42, bin->path);        }

                                            /* catch if we don't read as many bytes        */
    #define BINREAD(bin, rb, type, count, buff) { rb = sizeof(type) * count;                 \
        ERRCATCH(fread(buff, sizeof(type), count, bin->file), rb, 0x43, bin->path);         }

                                            /* catch if file signatures don't match        */
    #define BINSIG(bin, sig)                { int retx = 0;                                  \
        ERRCATCH(__wrstrcmp(bin->sign, sig), retx, 0x44, bin->path);                        }

    #define BINREMOVE(bin)                  { int retx = 0;                                  \
        ERRCATCH(RMBIN(bin), retx, 0x45, bin->path);                                        }

//   ---     ---     ---     ---     ---

#else
    #define BINOPEN(bin, retx) retx=MKBIN(bin)

    #define BINCLOSE(bin) DLBIN(bin)

    #define BINWRITE(bin, wb, type, count, buff)                                             \
        wb=fwrite(buff, sizeof(type), count, bin->file)

    #define BINREAD(bin, rb, type, count, buff)                                              \
        rb=fread(buff, sizeof(type), count, bin->file)

    #define BINSIG(bin, sig) asm("nop")

    #define BINREMOVE(bin) RMBIN(bin)

#endif

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_BIN_H__