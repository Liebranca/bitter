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

uint GTUNITCNT(uint size, uint mag)         {

    while(size > UNITSZ) {
        mag++; size-=UNITSZ;

    }; return (uint) ((pow(2, 2+mag) + 0.5)/size);                                          };

void MOVBLK(BLK* b, int dirn)               {

    int sign       = (dirn<0) ? -1 : 1;
    for(uint x=0; x<(dirn*sign); x++) {
        b->cbyte      += ((int) (rd_size))*sign;

        if(dirn<0) {
            if(b->cbyte < 0) {
                b->base--;
                b->cbyte+=UNITSZ;

            };

        } else {
            if ( ( b->cbyte > UNITSZ )  \
            || ( !(b->cbyte%UNITSZ))  ) {
                b->base++;
                b->cbyte-=UNITSZ;

            };
        };
    };                                                                                      };

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

//   ---     ---     ---     ---     ---    int, same^

void REGINT(void)                           {
    rd_cast =                                 0x05                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    long, same^

void REGLNG(void)                           {
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
    to[1]         = arrsize;
    to[2]         = (indlvl | arrsize) != 0;

//   ---     ---     ---     ---     ---

    if(base < 0x03) {                       // funptr
        to[0]=sizeof(STARK); return;

    } elif( 0x03 <= base \
      &&    0x06 >= base ) {                // num type
        to[0]=(uchar) (pow(2, base-0x03)+0.5);
        return;

    }; to[0]=sizeof(float);                                                                 };

//   ---     ---     ---     ---     ---

void TPADDR(ADDR* addr)                     {

    uchar szdata[3] = {0,0,0};              // unpack
    VALSIZ                                  (addr->id.type, szdata);

                                            // sizing ints for the read
    rd_elems        = GTUNITCNT             (szdata[0], szdata[1] );
    rd_step         = rd_size/UNITSZ;

    if(!rd_step) {
        rd_step     = 1;

    }; rd_units     =                       (rd_elems*rd_size)/UNITSZ;

//   ---     ---     ---     ---     ---

    szmask_a        = SIZMASK(rd_size);

    uint cbyte      = 0;
    uint i          = 0;                                                                    };

//   ---     ---     ---     ---     ---

void BYTESTEP(void)                         {

    rd_cbyte      += rd_size;

    if(!(rd_cbyte%UNITSZ)) {
        rd_result += rd_step;
        rd_lhand   = rd_result;
        rd_value   = rd_lhand+rd_step;

        CLMEM2(rd_value, rd_size);

        lngptr    += rd_step;
        rd_cbyte   = 0;

    };                                                                                      };

//   ---     ---     ---     ---     ---

void RSTSEC(void)                           {

    sec_beg.base  = lngptr;
    sec_beg.cbyte = rd_cbyte;

    sec_cur.base  = lngptr;
    sec_cur.cbyte = rd_cbyte;

    sec_end.base  = (rd_elems*rd_size)/UNITSZ;
    sec_end.cbyte = UNITSZ;

    rd_rawv++;                                                                              };

void RSTPTRS(void)                          {

    rd_result = ((MEMUNIT*) memlng->buff)+lngptr;
    rd_lhand  = rd_result;
    rd_value  = rd_lhand+rd_step;

    CLMEM2(rd_value, rd_size);
    CLMEM2(rd_lhand, rd_elems*rd_size);                                                     };

//   ---     ---     ---     ---     ---

void MAFETCH(MEMUNIT* r, MEMUNIT* v)        {

    MEMUNIT* addr = (MEMUNIT*) mammi->vaddr;

    uint size     =  mammi->vtype&0xFF;
    uint mag      = (mammi->vtype&0xFF00)>>8;

    uint elems    = GTUNITCNT(size, mag);

    uint units    = (elems*size)/UNITSZ;
    uint cbyte    = ((MEMUNIT)(*v))*size;
    uint cunit    = 0;

    uint step     = size/UNITSZ;

    if(!step) {
        step      = 1;

    };

//   ---     ---     ---     ---     ---

    while(cbyte>(UNITSZ-size)) {
        cbyte-=UNITSZ; cunit++;

    }; addr+=FETMASK(units, cunit);

//   ---     ---     ---     ---     ---

    (*r) = ((*addr) & (SIZMASK(size)<<(cbyte*8))) >> (cbyte*8);

    rd_flags&=~OP_AT;
    mammi->state&=~MAMMIT_SF_PFET; };

//   ---     ---     ---     ---     ---

void CHKMEMLAY(void)                        {


    MEMUNIT* ptr = mammi->lvalues+0;

    while(ptr) {

        pe_reg = (REG*) ptr;                // print regdata
        CALOUT(E, "\n0x%" PRIXPTR " %s\n\n",           pe_reg, pe_reg->id.full        );
        CALOUT(E, "0x%"   PRIXPTR " START\t%u\n",      &(pe_reg->start), pe_reg->start);
        CALOUT(E, "0x%"   PRIXPTR " ELEMS\t%u\n",      &(pe_reg->elems), pe_reg->elems);
        CALOUT(E, "0x%"   PRIXPTR " BOUND\t%u\n",      &(pe_reg->bound), pe_reg->bound);
        CALOUT(E, "0x%"   PRIXPTR " SIZE\t%u units\n", &(pe_reg->size ), pe_reg->size );

        for(uint x=0; x<pe_reg->bound; x++) {
            CALOUT(E, "0x%" PRIXPTR " JMP%u\t%u\n", pe_reg->jmpt+x, x, pe_reg->jmpt[x]);

        };

        for(uint x=0; x<pe_reg->bound; x++) {

            ADDR* addr      = (ADDR*) (mammi->lvalues+pe_reg->jmpt[x]);

            if(addr!=NULL) {
                                            // unpack and print addr && name
                TPADDR                      (addr                                           );
                CALOUT                      (E, "\n0x%" PRIXPTR " ID %s\n0x%"               \
                                                PRIXPTR " \t\t0x",                          \
                                                                                            \
                                            addr, addr->id.key, addr->box+0                 );

                for(uint x=0; x<rd_units; x++) {
                    CALOUT                  (E, "%016" PRIX64 " ", addr->box[x]             );

                }; ptr+=rd_units;
            };

        }; if(strstr(((REG*) ptr)->id.full, "REG*")==NULL) { break; }
    }; CALOUT(E, "\n\nLNAMES at {%u/%u} capacity | %u units remaining\n",                   \
                 mammi->lvaltop, NAMESZ, NAMESZ - mammi->lvaltop        );                  };

//   ---     ---     ---     ---     ---