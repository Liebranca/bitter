/*   ---     ---     ---     ---     ---    *
 *  KVR_BIN                                 *
 *                                          *
 *  baseline behaviours of common           *
 *  binary files; open/close, read/write    *
 *  defines signatures of ZJC filetypes     *
 *                                          *
 *   ---     ---     ---     ---     ---    */

#include "kvr_bin.h"
#include "kvr_mem.h"

#include <string.h>

//   ---     ---     ---     ---     ---

static int MUTEBIN = 0;                     // disables "file closed/opened... " messages

//   ---     ---     ---     ---     ---
// filetypes

static SIG DMPSIG = { 606348324,  606348324,  606348324,  606348324 };
static SIG CRKSIG = { 1126442020, 1378100260, 606348324,  606348363 };
static SIG JOJSIG = { 606348324,  606358052,  606359332,  608838692 };

static int BADSIG = 0; void STBADSIG(int is_bad) { BADSIG=is_bad!=0; };

int CMPSIG(SIG* s0, SIG* s1)                { if  (s0->a!=s1->a) { return ERROR; }
                                              elif(s0->b!=s1->b) { return ERROR; }
                                              elif(s0->c!=s1->c) { return ERROR; }
                                              elif(s0->d!=s1->d) { return ERROR; } return 0;};

SIG GTSIG(uint ft)                          {

    SIG sig;

    switch(ft) {

        case 0 : sig=CRKSIG; break;
        case 1 : sig=JOJSIG; break;

        default: sig=DMPSIG; break;

    };

    return sig;                                                                             };

//   ---     ---     ---     ---     ---

BIN* MKBIN(char* path, char* mode,
           uint  ft,   uint  ex_alloc)      {

    char name[20];

    int  pathsize = strlen(path);
    int  limit    = 18; if(pathsize < limit) { limit=pathsize; }
    int  y        = 0;

    for(int x=pathsize; y<limit; x--, y++)  { name[y]=*(path+(x-(limit-y))); }; name[y]='\0';

    ID id=IDNEW("BIN*", name);
    BIN* bin; MEMGET(BIN, bin, pathsize+1+4+ex_alloc, &id);

    y=0; char* m = MEMBUFF(byref(bin->m), char, 0); do { m[y]=*mode; y++; } while(*mode++);
    y=0; char* p = MEMBUFF(byref(bin->m), char, 4); do { p[y]=*path; y++; } while(*path++);

    bin->sign=GTSIG(ft);

    return bin;                                                                             };

int RDBIN(BIN* bin)                         {

    char* rmode = MEMBUFF(byref(bin->m), char, 0);
    char* path  = MEMBUFF(byref(bin->m), char, 4);

    int isnew   = !strcmp(rmode, "wb+");    // just so we know if we created something

                                            // we consider it a reading sesh if either
    int r = ( !strcmp(rmode, "rb+")         // -mode is read for update (input & output)
            | !strcmp(rmode, "rb" ) );      // -mode is read only       (just input    )

//   ---     ---     ---     ---     ---

                                            // open and reopen (once) on a failed read
    OPEN: bin->file = fopen                 (path, rmode                                    );
    if  (bin->file == NULL)                 {

        if  (isnew >  0   )                 { return ERROR;                                 }
        elif(r            )                 { rmode = "wb"; isnew = 1; goto OPEN;           };
                                                                                            };

//   ---     ---     ---     ---     ---

    if(!isnew)                              // not new? then check it's a valid file
    {

        SIG sig;                            // read to this
        int rb;                             // bytes read

                                            // read & compare
        BINREAD                             (bin, rb,  SIG, 1, &sig                         );
        BINSIG                              (bin, sig                                       );

        if(BADSIG)                          { return ERROR;                                 };

    }

//   ---     ---     ---     ---     ---

    else                                    // new? then MAKE IT valid
    {
        int wb;
        BINWRITE                            (bin, wb, SIG, 1, &(bin->sign)                  );
        rewind                              (bin->file                                      );
    }

//   ---     ---     ---     ---     ---

                                            // inform console rats like me about the fopen
    if  ( !MUTEBIN             )            { CALOUT("Opened file <%s>\n\b", path);         };
    if  ( !strcmp(rmode, "wb+"))            { isnew = 2;                                    };

    return isnew;                                                                           };

//   ---     ---     ---     ---     ---

char* PTHBIN(BIN* bin)                      { return MEMBUFF(byref(bin->m), char, 4);       };

int DLBIN(BIN* bin)                         {

    int failure = fclose(bin->file);

    if  ( failure)                          { return ERROR;                                 }
    elif(!MUTEBIN)                          { CALOUT("File closed <%s>\n\b", PTHBIN(bin));  };

    bin->file = NULL;
    return 0;                                                                               };

int RMBIN(BIN* bin)                         {

    if(bin->file)                           { BINCLOSE(bin)                                 };

    int retx=remove(PTHBIN(bin)); if(retx)  { return ERROR;                                 }
    elif(!MUTEBIN)                          { CALOUT("Deleted file <%s>\n\b", PTHBIN(bin)); };

    return 0;                                                                               };

//   ---     ---     ---     ---     ---
