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

static uintptr_t addr_table[16][(NAMESZ/16)+1] = {0};

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
    }; b->base=(b->base)&(rd_units-1);                                                      };

uint GTARRSZ(uint loc)                      { uintptr_t a = mammi->jmpt[loc+0];             \
                                              uintptr_t b = mammi->jmpt[loc+1];             \
                                              return (b-a)/UNITSZ;                          };

//   ---     ---     ---     ---     ---

uintptr_t uiptr_gtmax(uintptr_t a,
                      uintptr_t b)          { if(a<b) { return b; } return a;               };

uintptr_t uiptr_gtmin(uintptr_t a,
                      uintptr_t b)          { if(a>b) { return b; } return a;               };

uintptr_t uiptr_submin(uintptr_t a,
                       uintptr_t b)         { return uiptr_gtmax(a, b) - uiptr_gtmin(a, b); };

uint addrdist(void* a, void* b)             { return (uint) uiptr_submin((uintptr_t) a,     \
                                                                         (uintptr_t) b);    };

uintptr_t JMPTCLSR(uintptr_t addr) {

    TOP:

    uchar     ffam     = (addr&0xF0)>>4;
    uchar     fam      = (addr&0xF0)>>4;

    uintptr_t size     = addr_table[fam][(NAMESZ/16)];

    uintptr_t step     = (size)/2;
    uintptr_t next     = (size)/2;
    uintptr_t near     = 0x00;

    uintptr_t old_dist = -1;

//   ---     ---     ---     ---     ---

    for(uintptr_t x=0; x<size; x++) {

        uintptr_t cur  = addr_table[fam][next];
        uintptr_t dist = uiptr_submin(addr, cur);

//   ---     ---     ---     ---     ---

        if(dist) {

            step /= 2;
            if(cur<addr) {
                if(dist<old_dist) {
                    old_dist = dist;
                    near     = cur;

                }; next-=step; continue;

            }; next+=step; continue;

//   ---     ---     ---     ---     ---

        }; near = cur; break;
    };

    return near;                                                                            };

//   ---     ---     ---     ---     ---

uint ADDRTOLOC(uintptr_t addr)              {

    for(uint y=0; y<16; y++) {

        uintptr_t near = JMPTCLSR(addr-(y<<4));
        if(!near) { continue; }

        for(uint x=0; x<mammi->jmpt_i; x++) {
            if(near==mammi->jmpt[x]) {
                return x;

            };
        };

    }; return (uint) FATAL;                                                                 };

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

//   ---     ---     ---     ---     ---    misc hooks

void NONSTR(void)                           { typedata.flags &=~0x10;                       };

//   ---     ---     ---     ---     ---    void, nihil, stark

void REGVOI(void)                           { rd_cast = 0x00; NONSTR();                     };
void REGNHL(void)                           { rd_cast = 0x01; NONSTR();                     };
void REGTRK(void)                           { rd_cast = 0x02; NONSTR();                     };

//   ---     ---     ---     ---     ---    char

void REGCHR(void)                           { rd_cast = 0x03; NONSTR();                     };
void REGWID(void)                           { rd_cast = 0x04; NONSTR();                     };
void REGWRD(void)                           { rd_cast = 0x05; NONSTR();                     };
void REGLNG(void)                           { rd_cast = 0x06; NONSTR();                     };

//   ---     ---     ---     ---     ---    arrays

void REGSTR(void)                           { rd_cast = 0x06;
                                              typedata.strsz=1; typedata.flags |= 0x10;     };

void REGVEC(void)                           { NONSTR();                                     };

//   ---     ---     ---     ---     ---    float

void REGFLT(void)                           { rd_cast = 0x0F; NONSTR();                     };

//   ---     ---     ---     ---     ---    flags

void REGSGN(void)                           { typedata.flags |= 0x04;                       };
void REGUSG(void)                           { typedata.flags &=~0x04;                       };

//   ---     ---     ---     ---     ---

void UPKTYPE(void)                          {

    CLMEM2(                                 // clean-up leftovers from previous use
        (void*) &typedata,
        sizeof(TYPEDATA)

    );

    uchar  len  = strlen(rd_rawv);          // length of original string

    uchar  i    = 0;                        // offset into string
    uchar  j    = 0;                        // skip prefix part
    uchar  c    = 0x00;                     // current char

//   ---     ---     ---     ---     ---

    TOP:                                    // read through original string and decompose
        c=rd_rawv[i];                       // but first, get next char

//   ---     ---     ---     ---     ---

    if(i<2 || j) { switch(c) {

        case 0x7A:                          // z, zzzzstatic (lots o' volts)
            typedata.flags|=0x01;
            rd_rawv++; goto BOT;

        case 0x71:                          // q, qqqqconstant
            typedata.flags|=0x02;
            rd_rawv++; goto BOT;

        case 0x75:                          // u, uuuuunsigned
            typedata.flags|=0x04;
            rd_rawv++; goto BOT;

        default:
            break;

    }}; if(c==0x2A) {
        typedata.flags |= 0x08;             // it's a pointer!
        rd_rawv[i]      = 0x00;             // pop

    }  elif((c >= 0x30) && (0x39 >= c)) {   // char is number
        typedata.strsz *= 10;               // left-shift
        typedata.strsz += c-0x30;           // add unit
        rd_rawv[i]      = 0x00;             // pop

    }; j=1;

//   ---     ---     ---     ---     ---

    BOT: i++; if(i<len) { goto TOP; }
    typedata.strsz=(typedata.strsz<2) ? 2 : 1+typedata.strsz;                               };

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

    if(size%2) { size++; }

    MEMUNIT* data    = CURLVAL;
    mammi->lvaltop  += size;

    cur_cntx->size  += size;
    cur_cntx->elems++;

    for(uint x=0; x<size; x++) {            // copy units over
        data[x]=val[x];
                                            // send lookup data to table
    }; JMPT_INSERT                          (data, size, "VAL*", name);                     };

//   ---     ---     ---     ---     ---

uchar VALSIZ(uchar type)                    {

    if(type < 0x03) {                       // funptr
        return sizeof(STARK);

    } elif( 0x03 <= type \
      &&    0x06 >= type ) {                // num type
        return (uchar) (pow(2, type-0x03)+0.5);

    }; return sizeof(float);                                                                };

//   ---     ---     ---     ---     ---

void PROCADD(uchar* name,
             CODE*  val ,
             uint   size)                   {

    while(!size || size%2) { size++; }

    mammi->lvaltop  += size;
    cur_cntx->size  += size;
    cur_cntx->elems++;

    JMPT_INSERT(val, size, "INS*", name);                                                   };

//   ---     ---     ---     ---     ---

void TPADDR(uchar type, uint elems)         {

                                            // sizing ints for the read
    rd_size         = VALSIZ                (type                );

    if(elems == -1) {
        elems = 0;
        while((elems*rd_size)<UNITSZ*2) {
            elems++;

        };
    };

    rd_elems        = elems;
    rd_step         = rd_size/UNITSZ;

//   ---     ---     ---     ---     ---

    if(!rd_step) {
        rd_step     = 1;

    }; rd_units     =                       (rd_elems*rd_size)/UNITSZ;

    if(!rd_units) {
        rd_units    = 1;

    };

//   ---     ---     ---     ---     ---

    szmask_a        = SIZMASK(rd_size);
    if(!szmask_a) {
        szmask_a    = 0xFFFFFFFFFFFFFFFFLL;

    }; rd_cbyte    ^= rd_cbyte;                                                             };

//   ---     ---     ---     ---     ---

void BYTESTEP(int clear)                    {

    rd_cbyte      += rd_size;

    for(uint x=0;x<(rd_cbyte/UNITSZ);x++) {
        rd_result += rd_step;
        rd_lhand   = rd_result;
        rd_value   = rd_lhand+rd_step;

        if(clear) { 
            CLMEM2(rd_value, rd_size);

        };

        lngptr    += rd_step;
        rd_cbyte  -= UNITSZ*(rd_cbyte>=UNITSZ);
        if(rd_cbyte<UNITSZ) { break; }

    };                                                                                      };

//   ---     ---     ---     ---     ---

void RSTSEC(void)                           {

    sec_beg.base  = lngptr;
    sec_beg.cbyte = rd_cbyte;

    sec_cur.base  = lngptr;
    sec_cur.cbyte = rd_cbyte;

    sec_end.base  = ((rd_elems*rd_size)/UNITSZ)-1;
    sec_end.cbyte = UNITSZ-rd_size;

    rd_rawv++;                                                                              };

void RSTPTRS(void)                          {

    rd_result = ((MEMUNIT*) memlng->buff)+lngptr;
    rd_lhand  = rd_result;
    rd_value  = rd_lhand+rd_step;

    CLMEM2(memlng->buff, 128*UNITSZ);                                                       };

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

    cunit         = cbyte  / UNITSZ;
    cbyte        -= cunit  * UNITSZ;

    addr         += FETMASK(units, cunit);

//   ---     ---     ---     ---     ---

    (*r) = ((*addr) & (SIZMASK(size)<<(cbyte*8))) >> (cbyte*8);

    rd_flags&=~OP_AT;
    mammi->state&=~MAMMIT_SF_PFET;                                                          };

//   ---     ---     ---     ---     ---

void CHKMEMLAY(void)                        {

    uint      loc = 0;
    uintptr_t ptr = mammi->jmpt[loc];

    while(ptr && (loc < mammi->jmpt_i)) {

        ptr          = mammi->jmpt[loc];
        if(*((MEMUNIT*) ptr)==FREE_BLOCK) { break; };

        CNTX*     cx = (CNTX*) ptr;
        if(!cx->elems) { break; }

        uchar* name  = mammi->jmpt_h[loc].id.key;
        for(int x=0;x<strlen(name);x++) {
            if(name[x]>=0x61 && name[x]<=0x7B) {
                name[x]-=0x20;

            };
        };

        CALOUT(E, "\n0x%" PRIXPTR " BLOCK_START Sx%016"  PRIX64 " %s \n\n"           ,
                                                  cx, cx->state, name                );

        CALOUT(E, "0x%"   PRIXPTR " ELEMS\t%u\n", &(cx->elems), cx->elems            );
        CALOUT(E, "0x%"   PRIXPTR " USIZE\t%u\n", &(cx->size ), cx->size             );

        for(uint x=0; x<cx->elems; x++) { loc++;

            ptr           = mammi->jmpt[loc];
            if(*((MEMUNIT*) ptr)==FREE_BLOCK) { break; };

            name  = mammi->jmpt_h[loc].id.key;
            int y=0;
            for(;y<strlen(name);y++) {
                if(name[y]>=0x61 && name[y]<=0x7B) {
                    name[y]-=0x20;

                };
            };

            MEMUNIT* data = (MEMUNIT*) ptr;

                                            // unpack and print addr
            TPADDR                          (0x06, GTARRSZ(loc)                             );
            if(y<4) {
            CALOUT                          (E, "\n0x%" PRIXPTR " %s\t\t0x", ptr, name      );

            } else {
            CALOUT                          (E, "\n0x%" PRIXPTR " %s\t0x", ptr, name        );
            };

            uint px=43;
            for(uint y=0; y<rd_units; y++) {

                CALOUT                      (E, "\r\e[%uC", px                              );
                CALOUT                      (E, "%016" PRIX64 " ", data[y]                  );
                px=26;

                if(!((y+1)%2) && ((y+1)<rd_units)) {
                    CALOUT(E, "\r\e[59C\n0x%" PRIXPTR "\t\t0x", data+y+1);
                    px=43;

                };
            };
        }; loc++; CALOUT(E, "\n");
    };

//   ---     ---     ---     ---     ---

    END: CALOUT(E, "\n\nLNAMES at {%u/%u} capacity | %u units remaining\n",                 \
                   mammi->lvaltop, NAMESZ, NAMESZ - mammi->lvaltop        );                };

//   ---     ---     ---     ---     ---

#define OP_FORCEBIN(op)                     \
    (*r)=((MEMUNIT)(*r))op((MEMUNIT)(*v))

#define OP_FORCEUNA(op)                     \
    (*r)=op(MEMUNIT)(*v)

//   ---     ---     ---     ---     ---

void CALCUS_OPSWITCH(void)                  {

    MEMUNIT* r = rd_lhand;
    MEMUNIT* v = rd_value;

    if(rd_flags&OP_COLON) {
        rd_flags&=~OP_COLON;
        MAMMIT_LVLB_NXT; return;

    }; switch(rd_flags&0xFFFFFFFFFFFF3FFFLL) {

    case OP_ESUBS:
        mammi->state|=MAMMIT_SF_PFET;
        break;

    case OP_BSUBS: {
        mammi->state&=~MAMMIT_SF_PFET;
        uintptr_t addr=(*rd_value);
        *(v)=*((MEMUNIT*) addr);
        *(r)=(*(v))&szmask_a;
        break;
    }

    case OP_EMUL:
    case OP_MUL:
        (*r)*=(*v); goto OPSWITCH_MINUSX;

    case OP_EDIV:
    case OP_DIV:
        (*r)/=(*v); goto OPSWITCH_MINUSX;

    case OP_EMODU:
    case OP_MODUS:
        OP_FORCEBIN(%); goto OPSWITCH_MINUSX;

//   ---     ---     ---     ---     ---

    case OP_RSHFT:
        OP_FORCEBIN(>>); goto OPSWITCH_MINUSX;

    case OP_GT:
        (*r)=(*r)>(*v);  goto OPSWITCH_MINUSX;

    case OP_EGT:
        (*r)=(*r)>=(*v); goto OPSWITCH_MINUSX;

    case OP_LSHFT:
        OP_FORCEBIN(<<); goto OPSWITCH_MINUSX;

    case OP_LT:
        (*r)=(*r)<(*v);  goto OPSWITCH_MINUSX;

    case OP_ELT:
        (*r)=(*r)<=(*v); goto OPSWITCH_MINUSX;

//   ---     ---     ---     ---     ---

    case OP_BANG:
        (*r)=!(*v); goto OPSWITCH_MINUSX;

    case OP_EBANG:
        (*r)=(*r)!=(*v); goto OPSWITCH_MINUSX;

    case OP_QUEST:
        (*r)=(*v)!=0; goto OPSWITCH_MINUSX;

    case OP_EQUAL:
        (*r)=(*v); rd_flags&=~OP_EQUAL;
        goto OPSWITCH_MINUSX;

    case OP_ECOOL:
        (*r)=(*r)==(*v); goto OPSWITCH_MINUSX;

    case OP_KUSH: {
        MEMUNIT tmp=(*r);
        (*r)=(*v); (*v)=(tmp);
        goto OPSWITCH_MINUSX;
    }

//   ---     ---     ---     ---     ---

    case OP_DAMPR:
        OP_FORCEBIN(&&); goto OPSWITCH_MINUSX;

    case OP_EAMPR:
    case OP_AMPER:
        OP_FORCEBIN(&); goto OPSWITCH_MINUSX;

    case OP_DPIPE:
        OP_FORCEBIN(||); goto OPSWITCH_MINUSX;

    case OP_EPIPE:
    case OP_PIPE:
        OP_FORCEBIN(|); goto OPSWITCH_MINUSX;

//   ---     ---     ---     ---     ---

    case OP_EXOR:
    case OP_XORUS:
        OP_FORCEBIN(^); goto OPSWITCH_MINUSX;

    case OP_TILDE:
        OP_FORCEUNA(~); goto OPSWITCH_MINUSX;

//   ---     ---     ---     ---     ---

    case OP_AT: {
        MAFETCH((MEMUNIT*) r, (MEMUNIT*) v);
        goto OPSWITCH_MINUSX;
    };

//   ---     ---     ---     ---     ---

    case OP_PPLUS:
        (*r)++; (*r)=(*r)&szmask_a; break;

    case OP_MMINU:
        (*r)--; (*r)=(*r)&szmask_a; break;

//   ---     ---     ---     ---     ---

    case OP_MINUS:
        (*r)-=(*v); break;

    case OP_EPLUS:
    case OP_PLUS:
    default:
        (*r)+=(*v); break;

    }; END:

    (*v)=0; rd_flags^=rd_flags;
    if(!(mammi->state&MAMMIT_SF_PFET)) {
        ((*r)&szmask_a)<<(rd_cbyte*8);

    }; return;

//   ---     ---     ---     ---     ---

    OPSWITCH_MINUSX:
    if(rd_flags&OP_MINUS) {
        (*r)=-(*r);

    }; goto END;                                                                            };

//   ---     ---     ---     ---     ---

void CALCUS_COLLAPSE(void)                  {

    switch(rd_cast) {

        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06: {

            //CALOUT(E, "lvlb %u | 0x%016" PRIX64 " %016" PRIX64 " %016" PRIX64 " -> ",
            //          mammi->lvlb, *rd_lhand, rd_flags, *rd_value                   );

            *rd_value  = (*rd_value)<<(rd_cbyte*8);
            CALCUS_OPSWITCH();

            //CALOUT(E, "%016" PRIX64 "\n", *rd_lhand);

            break;
        }

//   ---     ---     ---     ---     ---

        default: {
            float* r = (float*) rd_lhand;
            float* v = (float*) rd_value;
            // CALCUS_OPSWITCH;

        };
    };                                                                                      };

//   ---     ---     ---     ---     ---

void SEPOPS(ulong fset,
            ulong fchk,
            uint  gnxt)                     {

    while(mammi->lvlb) {

        if(rd_flags&fchk) {
            break;

        };

        CALCUS_COLLAPSE();
        MAMMIT_LVLB_PRV;

    }; if(gnxt&2) { CALCUS_COLLAPSE(); }
    rd_flags |= fset;

    if(gnxt&1) { MAMMIT_LVLB_NXT; };                                                        };

//   ---     ---     ---     ---     ---

uint POPOPS(void)                           {

    uint  ctok_cnt = 0;
    uint  gnxt     = 0;

    ulong SEPS_A   = OP_COLON;
    ulong SEPS_B   = OP_COLON | OP_DAMPR | OP_DPIPE;
    ulong SEPS_C   = OP_COLON | OP_ECOOL | OP_EBANG | OP_DAMPR | OP_DPIPE;

    TOP: gnxt ^= gnxt;                      // if operator chars in token, eval and pop them

    switch(rd_rawv[0]) {

        default: break;

//   ---     ---     ---     ---     ---

        case 0x26: rd_flags |= OP_AMPER;
            gnxt=1; goto POP_OPSTOP;

        case 0x80: SEPOPS(OP_DAMPR | OP_COLON, SEPS_B, 0b11);
            gnxt=0; goto POP_OPSTOP;

        case 0x8A: rd_flags |= OP_EAMPR;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x7C: rd_flags |= OP_PIPE;
            gnxt=1; goto POP_OPSTOP;

        case 0x86: SEPOPS(OP_DPIPE | OP_COLON, SEPS_B, 0b11);
            gnxt=0; goto POP_OPSTOP;

        case 0x92: rd_flags |= OP_EPIPE;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x24: rd_flags |= OP_MONEY;
            gnxt=1; goto POP_OPSTOP;

        case 0x88: rd_flags |= OP_EMONY;
            gnxt=1; goto POP_OPSTOP;

        case 0x25: rd_flags |= OP_MODUS;
            gnxt=1; goto POP_OPSTOP;

        case 0x89: rd_flags |= OP_EMODU;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x5E: rd_flags |= OP_XORUS;
            gnxt=1; goto POP_OPSTOP;

        case 0x91: rd_flags |= OP_EXOR;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2B: rd_flags |= OP_PLUS;
            gnxt=1; goto POP_OPSTOP;

        case 0x81: rd_flags |= OP_PPLUS;
            gnxt=1; goto POP_OPSTOP;

        case 0x8C: rd_flags |= OP_EPLUS;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2D: rd_flags |= OP_MINUS;
            gnxt=1; goto POP_OPSTOP;

        case 0x82: rd_flags |= OP_MMINU;
            gnxt=1; goto POP_OPSTOP;

        case 0x8D: rd_flags |= OP_EMINU;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2A: rd_flags |= OP_MUL;
            rd_flags &=~OP_DIV;

            gnxt=1; goto POP_OPSTOP;

        case 0x8B: rd_flags |= OP_EMUL;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2F: rd_flags &=~OP_MUL;
            rd_flags |= OP_DIV;

            gnxt=1; goto POP_OPSTOP;

        case 0x8E: rd_flags |= OP_EDIV;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x21: MAMMIT_LVLB_NXT;
            rd_flags  = mammi->lvlb_stack[mammi->lvlb-1]&OP_MINUS;
            mammi->lvlb_stack[mammi->lvlb-1] &=~OP_MINUS;
            rd_flags |= OP_BANG;

            gnxt=1; goto POP_OPSTOP;

        case 0x87: SEPOPS(OP_EBANG, SEPS_C, 0b11);
            gnxt=0; goto POP_OPSTOP;

        case 0x3F: MAMMIT_LVLB_NXT;
            rd_flags  = mammi->lvlb_stack[mammi->lvlb-1]&OP_MINUS;
            mammi->lvlb_stack[mammi->lvlb-1] &=~OP_MINUS;
            rd_flags |= OP_QUEST;

            gnxt=1; goto POP_OPSTOP;

        case 0x7E: MAMMIT_LVLB_NXT;
            rd_flags  = mammi->lvlb_stack[mammi->lvlb-1]&OP_MINUS;
            mammi->lvlb_stack[mammi->lvlb-1] &=~OP_MINUS;
            rd_flags |= OP_TILDE;

            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x23: rd_flags |= OP_KUSH;
            gnxt=1; goto POP_OPSTOP;

        case 0x3D: rd_flags |= OP_EQUAL;
            gnxt=1; goto POP_OPSTOP;

        case 0x84: SEPOPS(OP_ECOOL, SEPS_C, 0b11);
            gnxt=0; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x28: MAMMIT_LVLB_NXT;
            gnxt=1; goto POP_OPSTOP;

        case 0x5B: MAMMIT_LVLB_NXT;
            rd_flags |= OP_BSUBS;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x40:
            CALOUT(E, "'@' operator is deprecated (for now); results unpredictable\n");

            if(!(mammi->state&MAMMIT_SF_PFET)) {
                CALOUT(E, "Using '@' operator without fetch-from\n");
                return 0;

                goto POP_OPSTOP;

            };

            rd_flags |= OP_AT;
            MAMMIT_LVLB_NXT;

            gnxt=1; goto POP_OPSTOP;goto POP_OPSTOP;

        case 0x3A: SEPOPS(OP_COLON, OP_COLON, 0b11);
            gnxt=0; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x3C: rd_flags |= OP_LT;
            gnxt=1; goto POP_OPSTOP;

        case 0x83: rd_flags |= OP_LSHFT;
            gnxt=1; goto POP_OPSTOP;

        case 0x8F: rd_flags |= OP_ELT;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x3E: rd_flags |= OP_GT;
            gnxt=1; goto POP_OPSTOP;

        case 0x85: rd_flags |= OP_RSHFT;
            gnxt=1; goto POP_OPSTOP;

        case 0x90: rd_flags |= OP_EGT;
            gnxt=1; goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x93:
        case 0x94:
            CALOUT(E, "Arrow/walkback not implemented (%s @%u)\n", __func__, __LINE__);

//   ---     ---     ---     ---     ---

        POP_OPSTOP:
            if(rd_ctok) {
                rd_ctok->lops[ctok_cnt]=*rd_rawv;
                ctok_cnt++;

            }; if(gnxt) { MAMMIT_LVLB_NXT; }
            rd_rawv++; goto TOP;

    };

//   ---     ---     ---     ---     ---

    ctok_cnt = 0;
    uint len = strlen(rd_rawv);

    if(!len) { goto END; }

    POP_TERMINATORS:                        // same as oppies, but at end of token
    switch(rd_rawv[len-1]) {

        default: break;

        case 0x5D: MAMMIT_LVLB_NXT;
            rd_flags |= OP_ESUBS;

            SEPOPS(0, OP_BSUBS, 0b10);

            goto POP_TESTOP;

        case 0x29:

            if(mammi->lvlb) {
                MAMMIT_LVLB_PRV;            // lonely parens >;

            }; mammi->state &=~MAMMIT_SF_PSEC;

        POP_TESTOP:

            if(rd_ctok) {
                rd_ctok->rops[ctok_cnt]=rd_rawv[len-1];
                ctok_cnt++;

            };

            rd_rawv[len-1]=0x00;
            len--; if(!len) { break; }
            goto POP_TERMINATORS;

    }; END: return len;                                                                     };

//   ---     ---     ---     ---     ---

void JMPT_INSERT(void*  x   ,
                 uint   size,
                 uchar* meta,
                 uchar* name)               {

    LABEL* l = (mammi->jmpt_h+mammi->jmpt_i);
    l->id    = IDNEW(meta, name);

    l->loc   = mammi->jmpt_i;
    l->p_loc = 0;

    l->meta  = typedata;

//   ---     ---     ---     ---     ---    find parent

    if(cur_cntx && mammi->jmpt_i) {
        uintptr_t paddr=(uintptr_t) cur_cntx;
        for(int y=(int) mammi->jmpt_i;
            y > -1; y--              ) {

            LABEL* pl = (mammi->jmpt_h+y);
            if(mammi->jmpt[pl->loc]==paddr) {
                l->p_loc = pl->loc; break;

            };
        };
    };

//   ---     ---     ---     ---     ---    insertion

    HASHSET(LNAMES_HASH, byref(l->id));

    mammi->jmpt[mammi->jmpt_i+0] = (uintptr_t) x;
    mammi->jmpt[mammi->jmpt_i+1] = (uintptr_t) (x+(size*UNITSZ));

    uchar     fam                = (((uintptr_t) x)&0xF0)>>4;
    uintptr_t idex               = addr_table[fam][(NAMESZ/16)]++;

    addr_table[fam][idex]        = (uintptr_t) x;

    mammi->jmpt_i++;                                                                        };

//   ---     ---     ---     ---     ---