/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    KVR_BIN                               *
*                                           *
*     -talks to stuff on disk               *
*     -likes siggies                        *
*     -is also a mem                        *
*     -has it's own buffer                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "kvr_bin.h"
#include "kvr_str.h"
#include <string.h>

//   ---     ---     ---     ---     ---
// common statics for read/write

static MEM* KVR_RD = NULL;                  // read buff
static MEM* KVR_WT = NULL;                  // write buff
static BIN* KVR_TO = NULL;                  // bin-to
static BIN* KVR_FR = NULL;                  // bin-from

void  NTRDBUF(void)                         { KVR_RD = MKSTR("KVR_RD", ZJC_DAFPAGE, 1);     };
void  NTWTBUF(void)                         { KVR_WT = MKSTR("KVR_WB", ZJC_DAFPAGE, 1);     };
void  DLRDBUF(void)                         { DLMEM(KVR_WT);                                };
void  DLWTBUF(void)                         { DLMEM(KVR_RD);                                };

void  LDBINTO(BIN* b)                       { KVR_TO=b;                                     };
void  LDBINFR(BIN* b)                       { KVR_FR=b;                                     };
BIN** GTBINTO(void  )                       { return &KVR_TO;                               };
BIN** GTBINFR(void  )                       { return &KVR_FR;                               };

MEM*  GTBINRD(void  )                       { return KVR_RD;                                };
MEM*  GTBINWT(void  )                       { return KVR_WT;                                };

//   ---     ---     ---     ---     ---
// filetypes

static SIG CRKSIG = { 1126442020, 1378100260, 606348324,  606348363  };
static SIG JOJSIG = { 606348324,  606358052,  606359332,  608838692  };
static SIG LNGSIG = { 606348364,  606359332,  609100836, 1193550884  };

// [...]

static SIG DMPSIG = { 606348324,  606348324,  606348324,  606348324  };
static int BADSIG = 0; void STBADSIG(int is_bad) { BADSIG=is_bad!=0; };

//   ---     ---     ---     ---     ---

int CMPSIG(SIG* s0, SIG* s1)                {

                                            // (retx != 0) == badsig
    int retx =                              ( (s0->a!=s1->a)                                \
                                            + (s0->b!=s1->b)                                \
                                            + (s0->c!=s1->c)                                \
                                            + (s0->d!=s1->d)                                );

    return (retx!=0)*ERROR;                                                                 };

//   ---     ---     ---     ---     ---

SIG GTSIG(uint ft)                          {

    SIG sig;

    switch(ft) {                            // easier passing a const than a ptr

        case KVR_FTYPE_CRK :                sig=CRKSIG; break;
        case KVR_FTYPE_JOJ :                sig=JOJSIG; break;
        case KVR_FTYPE_LNG :                sig=LNGSIG; break;

        // [...]

        default            :                sig=DMPSIG; break;

    };

    return sig;                                                                             };

//   ---     ---     ---     ---     ---

char* GTRMODE(uint x)                       {

    char* rmode;

    switch(x) {                             // same concept as with sigs

        case KVR_FMODE_RB :                 rmode="rb";  break;
        case KVR_FMODE_RBP:                 rmode="rb+"; break;
        case KVR_FMODE_WB :                 rmode="wb";  break;
        case KVR_FMODE_WBP:                 rmode="wb+"; break;

        // [...]

        default           :                 rmode="w+";  break;

    };

    return rmode;                                                                           };

//   ---     ---     ---     ---     ---

BIN* MKBIN(char* path, uint mode, uint  ft) {

    BIN* bin;

    char name[ZJC_IDK_WIDTH]; 

    int  len      = strlen(path);           // no overflow
    int  limit    = (ZJC_IDK_WIDTH-1);      if(len < limit) { limit=len;       };
    int  y        = 0;

                                            // make short id key from last chars of path
    for(int x=len; y<limit; x--, y++)       { name[y]=*(path+(len-(limit-y))); }; name[y]='\0';

//   ---     ---     ---     ---     ---

                                            // make id and get mem
    ID id         = IDNEW                   ("BIN*", name                                   );
    MEMGET                                  (BIN, bin, xBYTES(len+1, uchar), &id            );

    y             = 0;                      // copy chars from path into our own buff
    char* p       = MEMBUFF                 (byref(bin->m), char, 0                         );
    do                                      { p[y]=*path; y++; } while(*path++              );

                                            // set values
    bin->sign     = GTSIG                   (ft                                             );
    bin->mode     = mode;

    return bin;                                                                             };

//   ---     ---     ---     ---     ---

int RDBIN(BIN* bin)                         {

                                            // get readmode, then path from buff
    char* rmode =   GTRMODE                 (bin->mode                              );
    char* path  =   MEMBUFF                 (byref(bin->m), char, 0                 );

    int isnew   = ( !strcmp(rmode, "wb+")   // just so we know if we created something
                  | !strcmp(rmode, "wb" ) );


                                            // we consider it a reading sesh if either
    int r       = ( !strcmp(rmode, "rb+")   // -mode is read for update (input & output)
                  | !strcmp(rmode, "rb" ) );// -mode is read only       (just input    )

//   ---     ---     ---     ---     ---

                                            // open and reopen (once) on a failed read
    OPEN: bin->file = fopen                 (path, rmode                            );
    if  (bin->file == NULL)                 {

        if  (isnew >  0   )                 { return ERROR;                         }
        elif(r            )                 { rmode = "wb+"; isnew = 1; goto OPEN;  };
                                                                                    };

//   ---     ---     ---     ---     ---

    if(!isnew) {                            // not new? then check it's a valid file

        SIG sig;                            // read to this
        int rb;                             // bytes read

                                            // read & compare
        BINREAD                             (bin, rb,  SIG, 1, &sig                 );
        BINSIG                              (bin, sig                               );

        if(BADSIG)                          { return ERROR;                         };      }

//   ---     ---     ---     ---     ---

    else {                                  // new? then MAKE IT valid
        int wb;
        BINWRITE                            (bin, wb, SIG, 1, &(bin->sign)          );      };

//   ---     ---     ---     ---     ---

#if KVR_DEBUG & KVR_CALOF
    CALOUT('F', "Opened file <%s>\n\b", path);
#endif

    if  ( !strcmp(rmode, "wb+"))            { isnew = 2;                            };
    return isnew;                                                                           };

//   ---     ---     ---     ---     ---    // just a convenience getter

char* PTHBIN(BIN* bin)                      { return MEMBUFF(byref(bin->m), char, 0);       };

//   ---     ---     ---     ---     ---

int BIN2BIN (uint size)                     {

    int  wb;
    uchar* buff = (uchar*) KVR_RD->buff;

#if KVR_DEBUG & KVR_CALOF
    CALOUT('F', "Writting from <%s> to <%s>\n\b", PTHBIN(KVR_FR), PTHBIN(KVR_TO));
#endif

    while(size) {
        uint readsize  =                    (size<ZJC_DAFSIZE) ? size : ZJC_DAFSIZE;

        fseek                               (KVR_FR->file, 0, SEEK_CUR        );
        BINREAD                             (KVR_FR, wb, uchar, readsize, buff);
        fseek                               (KVR_FR->file, 0, SEEK_CUR        );

        fseek                               (KVR_TO->file, 0, SEEK_CUR        );
        BINWRITE                            (KVR_TO, wb, uchar, readsize, buff);
        fseek                               (KVR_TO->file, 0, SEEK_CUR        );

        size          -= readsize;

    }; return DONE;                                                                         };

//   ---     ---     ---     ---     ---

int DLBIN(BIN* bin)                         {

                                            // errcatch before you print success
    int failure = fclose                    (bin->file                                      );
    if  ( failure)                          { return ERROR;                                 }

#if KVR_DEBUG & KVR_CALOF
    CALOUT('F', "File closed <%s>\n\b", PTHBIN(bin));
#endif

    bin->file   = NULL;
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int RMBIN(BIN* bin)                         {

                                            // close before delete, then errcatch
    if(bin->file)                           { BINCLOSE(bin);                                };
    int retx=remove(PTHBIN(bin)); if(retx)  { return ERROR;                                 }

#if KVR_DEBUG & KVR_CALOF
    CALOUT('F', "Deleted file <%s>\n\b", PTHBIN(bin));
#endif

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---
