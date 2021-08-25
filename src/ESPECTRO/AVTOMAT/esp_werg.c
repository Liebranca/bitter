/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ESP_WERG                              *
*                                           *
*     -its werg                             *
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

#include "KVRNEL/kvr_paths.h"
#include "CHASM/chMNG.h"

#include "CHASM/SIN/__sin.h"
#include "CHASM/SIN/sin_texture.h"
#include "CHASM/SIN/REND/sin_canvas.h"

#include <libpng16/png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

//   ---     ---     ---     ---     ---

int main(int argc, char** argv) {

    char* paths[4]={
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\"
    };

    NTKVRPTH(paths     );
    NTCHMNG ("SINx8", 0);

    NTSIN();

    while(GTCHMNGRUN()) {

        int evilstate=FRBEGCHMNG();
        if(evilstate) { break; };

        DRCANVAS();

        FRENDCHMNG();
        SLEEPCHMNG();

    };

    DLSIN   ();
    DLCHMNG ();
    DLKVRPTH();

    return 0;

};

//   ---     ---     ---     ---     ---

static void
fatal_error (const char * message, ...)
{
    va_list args;
    va_start (args, message);
    vfprintf (stderr, message, args);
    va_end (args);
    exit (EXIT_FAILURE);
}

int opim ()
{
    system("clear");

    const char * png_file = "D:/lieb_git/KVR/bin/x64/lycon.png";
    png_structp png_ptr;
    png_infop info_ptr;
    FILE * fp;
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;
    int j;
    png_bytepp rows;
    fp = fopen (png_file, "rb");
    if (! fp) {
    fatal_error ("Cannot open '%s': %s\n", png_file, strerror (errno));
    }
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (! png_ptr) {
    fatal_error ("Cannot create PNG read structure");
    }
    info_ptr = png_create_info_struct (png_ptr);
    if (! png_ptr) {
    fatal_error ("Cannot create PNG info structure");
    }
    png_init_io (png_ptr, fp);
    png_read_png (png_ptr, info_ptr, 0, 0);
    png_get_IHDR (png_ptr, info_ptr, & width, & height, & bit_depth,
          & color_type, & interlace_method, & compression_method,
          & filter_method);
    rows = png_get_rows (png_ptr, info_ptr);
    int rowbytes;
    rowbytes = png_get_rowbytes (png_ptr, info_ptr);
    printf("\e[1;1H\e[31m");

    #define NFO_SIZ 11
    char* NFO[NFO_SIZ] = {

"******************************",
"*                            *",
"*      < ESPECTRO~GNU >      *",
"*          es-shell          *",
"*                            *",
"*       LIBRE SOFTWARE       *",
"*     Licenced under GPL3    *",
"*                            *",
"* Copyleft (c) CIXXPAKK 2021 *",
"*                            *",
"******************************" };

    for (j = 0; j < height; j++) {
    int i;
    png_bytep row;
    row = rows[j];

    if(j<NFO_SIZ) {

        printf("\e[%u;%uH%s", j+2,67,NFO[j]);

    }

    for (i = 0; i < rowbytes; i+=2) {

        png_byte pixel;
        pixel = row[i]; printf("\e[%u;%uH", j+1, (i/2)+1); // +35
        if (pixel < 32) {
        printf ("*");
        }
        else if (pixel < 64) {
        printf ("%%");
        }
        else if (pixel < 96) {
        printf (";");
        }

    }
    if(j<30) { printf("\n"); };
    }
    printf("\e[0m\e[1;1H");
    return 0;

};

//   ---     ---     ---     ---     ---