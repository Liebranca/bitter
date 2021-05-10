#include "kvr_bin.h"
#include "kvr_mem.h"

#include <string.h>

//   ---     ---     ---     ---     ---

static int MUTEBIN = 0;                     // disables "file closed/opened... " messages

//   ---     ---     ---     ---     ---

int MKBIN(BIN* bin)                         {

    int isnew   = 0;                        // just so we know if we created something

                                            // we consider it a reading sesh if either
    int r = ( !strcmp(bin->mode, "rb+")     // -mode is read for update (input & output)
            | !strcmp(bin->mode, "rb" ) );  // -mode is read only       (just input    )

//   ---     ---     ---     ---     ---

                                            // open and reopen (once) on a failed read
    OPEN: bin->file = fopen                 (bin->path, bin->mode                           );
    if  (bin->file == NULL)                 {

        if  (isnew >  0   )                 { return ERROR;                                 }
        elif(r            )                 { bin->mode = "wb"; isnew = 1;  goto OPEN;      };
                                                                                            };

//   ---     ---     ---     ---     ---

    if(bin->sign)                           // check for file signature if present
    {

        if(!isnew)                          // not new? then check it's a valid file
        {
            MEM* m;

            char buff[64];                  // read to this
            int  rb;                        // bytes read

                                            // read & compare
            BINREAD                         (bin, rb,  char, 1, buff                        );
            BINSIG                          (bin, buff                                      );

        }

//   ---     ---     ---     ---     ---

        else                                // new? then MAKE IT valid
        {
            int wb;
            BINWRITE                        (bin, wb, char, strlen(bin->sign), bin->sign    );
            rewind                          (bin->file                                      );
        }
    }

//   ---     ---     ---     ---     ---

                                            // inform console rats like me about the fopen
    if  ( !MUTEBIN                 )        { printf("Opened file <%s>\n", bin->path);      };
    if  ( !strcmp(bin->mode, "wb+"))        { isnew = 2;                                    };

    return isnew;                                                                           };

//   ---     ---     ---     ---     ---

int DLBIN(BIN* bin)                         {

    int failure = fclose(bin->file);

    if  ( failure)                          { return ERROR;                                 }
    elif(!MUTEBIN)                          { printf("File closed <%s>\n", bin->path);      };

    bin->file = NULL;
    return 0;                                                                               };

int RMBIN(BIN* bin)                         {

    if(bin->file)                           { BINCLOSE(bin)                                 };

    int retx = remove(bin->path); if(retx)  { return ERROR;                                 };
    printf                                  ( "Deleted file <%s>\n", bin->path              );

    return 0;                                                                               };

//   ---     ---     ---     ---     ---
