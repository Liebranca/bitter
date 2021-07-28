#include "kvr_bin.h"
#include <string.h>

//   ---     ---     ---     ---     ---
// filetypes

static SIG CRKSIG = { 1126442020, 1378100260, 606348324,  606348363  };
static SIG JOJSIG = { 606348324,  606358052,  606359332,  608838692  };

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

BIN* MKBIN(char* path, uint mode,
           uint  ft,   uint  ex_alloc)      {

    BIN* bin;

    char name[KVR_IDK_WIDTH]; 

    int  len      = strlen(path);           // no overflow
    int  limit    = (KVR_IDK_WIDTH-1);      if(len < limit) { limit=len;       };
    int  y        = 0;

                                            // make short id key from last chars of path
    for(int x=len; y<limit; x--, y++)       { name[y]=*(path+(len-(limit-y))); }; name[y]='\0';

//   ---     ---     ---     ---     ---

                                            // make id and get mem
    ID id         = IDNEW                   ("BIN*", name                                   );
    ex_alloc      =                         (len+1) + ex_alloc;                             \
    MEMGET                                  (BIN, bin, xBYTES(ex_alloc, uchar), &id         );

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

                                            // just so we know if we created something
    int isnew   =   !strcmp                 (rmode, "wb+"                           );

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

#if KVR_DEBUG
    CALOUT("Opened file <%s>\n\b", path);
#endif

    if  ( !strcmp(rmode, "wb+"))            { isnew = 2;                            };
    return isnew;                                                                           };

//   ---     ---     ---     ---     ---    // just a convenience getter

char* PTHBIN(BIN* bin)                      { return MEMBUFF(byref(bin->m), char, 0);       };

//   ---     ---     ---     ---     ---

int DLBIN(BIN* bin)                         {

                                            // errcatch before you print success
    int failure = fclose                    (bin->file                                      );
    if  ( failure)                          { return ERROR;                                 }

#if KVR_DEBUG
    CALOUT("File closed <%s>\n\b", PTHBIN(bin));
#endif

    bin->file   = NULL;
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int RMBIN(BIN* bin)                         {

                                            // close before delete, then errcatch
    if(bin->file)                           { BINCLOSE(bin);                                };
    int retx=remove(PTHBIN(bin)); if(retx)  { return ERROR;                                 }

#if KVR_DEBUG
    CALOUT("Deleted file <%s>\n\b", PTHBIN(bin));
#endif

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---
