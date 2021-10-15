/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_BOILER                             *
*                                           *
*     -annoying bits of rd                  *
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

#include "ma_boiler.h"
#include <string.h>
#include <math.h>

//   ---     ---     ---     ---     ---

int NOREDCL(uchar* name)                    {

    void* nulmy = NULL;                     // check if name exists in global scope
    STR_HASHGET                             (GNAMES_HASH, name, nulmy, 0);

    if(nulmy) {                             // freak out if it does;
        return ERROR;

                                            // now check if name exists in *local* scope
    }; STR_HASHGET                          (LNAMES_HASH, name, nulmy, 0);

    if(nulmy) {                             // ... and freak out if it does
        return ERROR;

    }; return DONE;                                                                         };

int NOOVERSZ(uint sz, uint f)               { if(sz%f) { return ERROR; }; return DONE;      };

//   ---     ---     ---     ---     ---

// these funcs below? lazy nihil way to identify type
// without having to do a strcmp or str_hashget...

// it IS a little silly to hardcode it like this
// BUT: aren't base types HARDCODED to BEG with?

// the initial hash-gotten symbol can redirect
// to a single function, that function KNOWS
// how to interpret the data; this is simpler

// repetitive? yes
// does it work? yes
// optimal? depends

// its either this or using arguments
// AND yet more branches per call, mind you
// so no thanks, this mess will do

//   ---     ---     ---     ---     ---    void, nihil, stark

void REGVOI(void)                           { rd_cast = 0x00; REGTP();                      };
void REGNHL(void)                           { rd_cast = 0x01; REGTP();                      };
void REGTRK(void)                           { rd_cast = 0x02; REGTP();                      };

//   ---     ---     ---     ---     ---    char

void REGCHR(void)                           {

                                            // int(bitsize) type + is_unsigned
    rd_cast =                                 0x03                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );

                                            // evaluate the expression;
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    wide, same^

void REGWID(void)                           {
    rd_cast =                                 0x04                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    long, same^

void REGLNG(void)                           {
    rd_cast =                                 0x05                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    quat, same^

void REGQAT(void)                           {
    rd_cast =                                 0x06                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    float

void REGFLT(void)                           { rd_cast = 0x0B; REGTP();                      };

//   ---     ---     ---     ---     ---

void UPKTYPE(uchar* typeval)                {

    CLMEM2(                                 // clean-up leftovers from previous use
        (void*) &typedata,
        sizeof(TYPEDATA)

    );

    uchar* base = typedata.base+0;          // point to our static block of chars
    uint   len  = strlen(typeval);          // length of original string

    ushort i    = 0;                        // offset into original string
    ushort j    = 0;                        // offset into __target__ string

    uchar  c    = 0x00;                     // character currently being pointed at

//   ---     ---     ---     ---     ---

    TOP:                                    // read through original string and decompose

        c=typeval[i];                       // get next char

        if((c >= 0x30) && (0x39 >= c)) {    // if char is number
            typedata.arrsize*=10;           // left-shift
            typedata.arrsize+=c-0x30;       // add unit
            goto BOT;                       // skip to end

        }; if(j || i>2) { goto MID; };      // skip if char > original[0-2]

//   ---     ---     ---     ---     ---

    switch(c) {

        case 0x7A:                          // z, zzzzstatic (lots o' volts)
            typedata.flags|=0x01; goto BOT;
        case 0x71:                          // q, qqqqconstant
            typedata.flags|=0x02; goto BOT;
        case 0x75:                          // u, uuuuunsigned
            typedata.flags|=0x04; goto BOT;

        default:
            break;

    };

//   ---     ---     ---     ---     ---

    MID:

    switch(c) {

        case 0x2A:                          // ptr to ptr to ptr to ptr to ptpr to what
            typedata.indlvl++; break;

        default:                            // everything else goes
            base[j]=c; j++; break;

    }; BOT: i++; if(i<len) { goto TOP; }                                                    };

//   ---     ---     ---     ---     ---

SYMBOL SYMNEW(uchar* fam,
              uchar* key,
              NIHIL  fun)                   {

    SYMBOL sym = {0};                       // simple 'constructor', so to speak

    sym.id     = IDNEW(fam, key);           // fill out the id
    sym.onrd   = fun;                       // when !fun, you're bored and null

    return sym;                                                                             };

//   ---     ---     ---     ---     ---

void VALNEW(uchar*   name,
            MEMUNIT* val ,
            uint     size)                  {

    MAMMIT_REG_ADD;

    uchar base_type = rd_cast;
    if( 0x07 <= base_type
    &&  0x0A >= base_type) {
        base_type-=0x04;

    };

    uchar type[4]   = { base_type, typedata.arrsize, typedata.indlvl, typedata.flags };

    ADDR* addr      = (ADDR*) CURLVAL;
    addr->id        = IDNEW(type, name);

    mammi->lvaltop += size;
    INCLVAL(sizeof(ID));

    for(uint x=0; x<size; x++) {            // copy qbs over
        addr->box[x]=val[x];
                                            // insert in hash for later fetch by key
    }; HASHSET                              (LNAMES_HASH, byref(addr->id));                 };

//   ---     ---     ---     ---     ---

void VALSIZ(uchar* type, uchar* to) {

    uchar base    = type[0];
    uchar arrsize = type[1];
    uchar indlvl  = type[2];
    uchar flags   = type[3];

    to[0]         = 4;
    to[1]         = (uint) (pow(2, arrsize)+0.5);
    to[2]         = (indlvl | arrsize) != 0;

//   ---     ---     ---     ---     ---

    if(base < 0x03) {                       // funptr
        to[0]=sizeof(STARK); return;

    } elif( 0x03 <= base \
      &&    0x06 >= base ) {                // num type
        to[0]=2*(base-0x03);
        if(!to[0]) { to[0]=1; }

        return;

    }; to[0]=sizeof(float);                                                                 };

//   ---     ---     ---     ---     ---