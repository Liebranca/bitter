#ifndef __KVR_BIN_H__
#define __KVR_BIN_H__

#include "kvr_mem.h"

#include <stdio.h>

//   ---     ---     ---     ---     ---    // fopen mode defs; syntax dummies

#define KVR_FMODE_RB   0
#define KVR_FMODE_RBP  1
#define KVR_FMODE_WB   2
#define KVR_FMODE_WBP  3

// [...]

#define KVR_FMODE_WP  -1

//   ---     ---     ---     ---     ---    // filetype defs; also syntax dummies

#define KVR_FTYPE_CRK  0
#define KVR_FTYPE_JOJ  1
#define KVR_FTYPE_LNG  2

// [...]

#define KVR_FTYPE_DMP -1

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct HELRNW {                     // helper values for read/write packed in a struct

    uint idex;                              // current position within file
    uint total;                             // total entries in file

    uint size_i;                            // uncompressed size
    uint size_d;                            // compressed size

    uint offs_i                             // *bytes* to skip on uncompressed read/write
    uint offs_d;                            // *bytes* to skip on compressed read/write

} HRW;

//   ---     ---     ---     ---     ---

typedef struct KVR_SIG {                    // a more 'arcane' way to go about signatures

    uint a;
    uint b;
    uint c;
    uint d;

} SIG; CASSERT                              (sizeof(SIG)==sizeof(uint)*4, "Bad SIG size");

//   ---     ---     ---     ---     ---

typedef struct KVR_BIN {                    // helper struct for dealing with files

    MEM   m;                                // mem header

    SIG   sign;                             // file signature
    uint  mode;                             // readmode+flags (maybe?)
    FILE* file;                             // file handle

} BIN;

//   ---     ---     ---     ---     ---

BIN* MKBIN     (char* path      ,
                uint  mode      ,
                uint  ft        );          // init bin for read/write
                                            // -ft determines sign to check

//   ---     ---     ---     ---     ---

void  NTRDBUF  (void            );          // nit read buffer
void  NTWTBUF  (void            );          // nit write buffer
void  DLRDBUF  (void            );          // free read buffer
void  DLWTBUF  (void            );          // free write buffer

void  LDBINTO  (BIN* b          );          // load bin into bin-to
void  LDBINFR  (BIN* b          );          // load bin into bin-from

BIN** GTBINTO  (void            );          // get bin-to
BIN** GTBINFR  (void            );          // get bin-from
MEM*  GTBINRD  (void            );          // get read buff
MEM*  GTBINWT  (void            );          // get write buff
//   ---     ---     ---     ---     ---

SIG   GTSIG   (uint ft         );

int   RDBIN   (BIN* bin        );           // read from bin
int   DLBIN   (BIN* bin        );           // closes a bin file
int   RMBIN   (BIN* bin        );           // removes a bin file from disk
int   CMPSIG  (SIG* s0, SIG* s1);           // compares two signatures
void  STBADSIG(int is_bad      );           // alters read-state on inappropriate signature
char* PTHBIN  (BIN* bin        );           // fetch path to bin

int   BIN2BIN (uint size       );           // write size bytes from src to dst
                                            // -bin-from is src
                                            // -bin-to is dst
                                            // -no rewind

//   ---     ---     ---     ---     ---

#if KVR_DEBUG
                                            /* catch if error on open                      */
    #define BINOPEN(bin, path, mode, ft, retx) { retx = 0;                                   \
        bin=MKBIN(path, mode, ft); ERRCATCH(RDBIN(bin), retx, 0x40,PTHBIN(bin));            }

                                            /* catch if error on close                     */
    #define BINCLOSE(bin)                   { int retx = 0;                                  \
        ERRCATCH(DLBIN(bin), retx , 0x41, PTHBIN(bin));                                     }

                                            /* catch if we don't write as many bytes       */
    #define BINWRITE(bin, wb, type, count, buff){ wb=count;                                  \
        ERRCATCH(fwrite(buff, sizeof(type), count, bin->file), wb, 0x42, PTHBIN(bin));       \
        wb *= sizeof(type);                                                                 }

                                            /* catch if we don't write as many bytes       */
    #define BINWRITE_ATR(bin, wb, type, count, buff, offset){ wb=count;                      \
        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
        ERRCATCH(fwrite(buff, sizeof(type), count, bin->file), wb, 0x42, PTHBIN(bin));       \
        fseek(bin->file, 0, SEEK_CUR);                                                       \
        wb *= sizeof(type);                                                                 }

                                            /* catch if we don't read as many bytes        */
    #define BINREAD(bin, rb, type, count, buff) { rb = count;                                \
        ERRCATCH(fread(buff, sizeof(type), count, bin->file), rb, 0x43, PTHBIN(bin));            \
        rb *= sizeof(type);                                                                 }

    #define BINREAD_ATR(bin, rb, type, count, buff, offset) { rb = count;                    \
        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
        ERRCATCH(fread(buff, sizeof(type), count, bin->file), rb, 0x43, PTHBIN(bin));        \
        fseek(bin->file, 0, SEEK_CUR);                                                       \
        rb *= sizeof(type);                                                                 }

                                            /* catch if file signatures don't match        */
    #define BINSIG(bin, sig)                { int retx = 0;                                  \
        ERRCATCH(CMPSIG(&bin->sign, &sig), retx, 0x44, PTHBIN(bin)); STBADSIG(retx);        }

    #define BINREMOVE(bin)                  { int retx = 0;                                  \
        ERRCATCH(RMBIN(bin), retx, 0x45, PTHBIN(bin));                                      }

//   ---     ---     ---     ---     ---

#else
    #define BINOPEN(bin, path, mode, ft, retx)                                               \
        bin=MKBIN(path, mode, ft); retx=RDBIN(bin)

    #define BINCLOSE(bin) DLBIN(bin)

    #define BINWRITE(bin, wb, type, count, buff)                                             \
        wb=fwrite(buff, sizeof(type), count, bin->file)

    #define BINWRITE_ATR(bin, wb, type, count, buff, offset)                                 \
        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
        wb=fwrite(buff, sizeof(type), count, bin->file); fseek(bin->file, 0, SEEK_CUR)

    #define BINREAD(bin, rb, type, count, buff)                                              \
        rb=fread(buff, sizeof(type), count, bin->file)

    #define BINREAD_ATR(bin, rb, type, count, buff, offset)                                  \
        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
        rb=fread(buff, sizeof(type), count, bin->file); fseek(bin->file, 0, SEEK_CUR)

    #define BINSIG(bin, sig) STBADSIG(CMPSIG(&bin->sign, &sig));

    #define BINREMOVE(bin) RMBIN(bin)

#endif

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_BIN_H__