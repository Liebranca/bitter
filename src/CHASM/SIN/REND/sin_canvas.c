/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SIN_CANVAS                            *
*                                           *
*     -fullscreen rect                      *
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

#include "KVRNEL/MEM/kvr_mem.h"
#include "KVRNEL/lymath.h"

#include "SIN/sin_globals.h"
#include "SIN/sin_shader.h"

#include "SIN/SHADERS/sin_CanvasShader.h"

#include <string.h>
#include <inttypes.h>

//   ---     ---     ---     ---     ---

#define PESO_RUNNING 0x01
#define PESO_ESCAPED 0x02
#define PESO_ESCREAD 0x04

#define PESO_OPWIDTH 64
#define PESO_STKSIZE 256

//   ---     ---     ---     ---     ---

static char* st_src = "$:%gb;>Hello world$:/gb;>!";
static char* tstr;

void NOPE(void* nope) {
    CALOUT('P', "\e[67;0HYou nope!");

};

void GBPE(void* buff) {
    ustr8 data = *((ustr8*) buff);
    CALOUT('P', "\e[67;0HYou gb 0x%08X", data.F);

};

void WTPE(void* buff) {
    CALOUT('P', "\e[67;0HYou wt!");

};

const STARK PEMAC_OPCODES[] = {

    &NOPE,                                  // np; do nothing
    &GBPE,                                  // gb;  back and paste over
    &WTPE,                                  // wt;  write

};

//   ---     ---     ---     ---     ---

typedef struct PESO_OPERATION {

    ID    id;
    ustr8 code;

} PEOP; static PEOP PESO_OPDUMMY = {0};

typedef struct PESO_MACHINE {

    MEM*  m;                                // mem header

    char  rdop[PESO_OPWIDTH];
    ustr8 args;

    uint  state;                            // state bitflags
    STK   stack;                            // loaded codes; push on lvlup pop on lvldw

} PEMAC;

static PEMAC* pemac;
static HASH*  GNAMES;

static PEOP   PE_NP = {0};
static PEOP   PE_GB = {0};
static PEOP   PE_WT = {0};

//   ---     ---     ---     ---     ---

static uint   canvasVAO         = 0;
static uint   canvasVBO         = 0;
static uint   canvasProgram     = 0;
static uint   canvasShader  [2] = { 0, 0 };
static uint   canvasUniforms[2] = { 0, 0 };

//   ---     ---     ---     ---     ---

void NTCANVAS(void)                         {

                                            // belongs in intprtr, actually
    GNAMES        = MKHASH                  (5, "GNAMES"                                 );

                                            // for opcode-to-funptr fetch
    PE_NP.id      = IDNEW                   ("FUN*", "np"                                );
    PE_GB.id      = IDNEW                   ("FUN*", "gb"                                );
    PE_WT.id      = IDNEW                   ("FUN*", "wt"                                );

    PE_NP.code.F  = 0x00;
    PE_GB.code.F  = 0x01;
    PE_WT.code.F  = 0x02;

                                            // upload opcodes to gnames
    HASHSET                                 (GNAMES, byref(PE_NP.id)                     );
    HASHSET                                 (GNAMES, byref(PE_GB.id)                     );
    HASHSET                                 (GNAMES, byref(PE_WT.id)                     );

    ID id    = IDNEW                        ("PE$O", "_MACHINE"                          );
    MEMGET                                  (PEMAC, pemac                                ,
                                             xBYTES(PESO_STKSIZE, uint), &id             );

                                            // init the callstack
    MKSTK                                   (byref(pemac->stack), PESO_STKSIZE           );

//   ---     ---     ---     ---     ---

                                            // just a shorthand
    const SHDP* params         =            &SIN_CanvasShader;

                                            // draw rect
    float quadVertices[]       =            {  0.0f,  1.0f,                              \
                                               0.0f,  0.0f,                              \
                                               1.0f,  0.0f,                              \
                                               0.0f,  1.0f,                              \
                                               1.0f,  0.0f,                              \
                                               1.0f,  1.0f                               };

//   ---     ---     ---     ---     ---

                                            // gl alloc
    glGenVertexArrays                       (1, &canvasVAO                               );
    glGenBuffers                            (1, &canvasVBO                               );
    glBindVertexArray                       (canvasVAO                                   );

                                            // load data
    glBindBuffer                            (GL_ARRAY_BUFFER, canvasVBO                  );
    glBufferData                            (GL_ARRAY_BUFFER, sizeof(quadVertices        ),
                                             &quadVertices, GL_STATIC_DRAW               );

                                            // set attrib pointers
    glEnableVertexAttribArray               (0                                           );
    glVertexAttribPointer                   (0, 2, GL_FLOAT, GL_FALSE                    ,
                                             2 * sizeof(float), (void*) 0                );

//   ---     ---     ---     ---     ---

                                            // make shaders and attach
    canvasProgram = glCreateProgram         (                                            );
    canvasShader[0] = CMPSHD                (params->source_v, params->num_vsources      ,
                                             GL_VERTEX_SHADER                            );
    canvasShader[1] = CMPSHD                (params->source_f, params->num_fsources      ,
                                             GL_FRAGMENT_SHADER                          );

    for (uint i=0; i<2; i++) {
        glAttachShader                      (canvasProgram, canvasShader[i]              );

    };

    for(uchar attribLoc = 0            ;
        attribLoc < params->num_attribs;
        attribLoc++                    ) {  // fix attrib locations
            glBindAttribLocation            (canvasProgram, 0, params->attribs[attribLoc]);

    };

//   ---     ---     ---     ---     ---

                                            // errcatch boiler

    glLinkProgram                           (canvasProgram                               );
    CHKSHDERR                               (canvasProgram, GL_LINK_STATUS               ,
                                             1, "Shader linking failed"                  );

    glValidateProgram                       (canvasProgram                               );
    CHKSHDERR                               (canvasProgram, GL_VALIDATE_STATUS           ,
                                             1, "Shader validation failed"               );

                                            // locate uniforms
    for(uchar i = 0; i < params->num_uniforms; i++) {
        canvasUniforms[i] = glGetUniformLocation(canvasProgram, params->uniforms[i]      );

    };                                                                                      };

//   ---     ---     ---     ---     ---

void RDCANVAS(char** st  ,
              char   last,
              ustr8* d   ,
              float* t   )                  {

    int lstate=0;

    VERIFY_PE_STATE:
    d[0].x=(**st);
    switch(d[0].x) {                        // check current state

        case 0x00: pemac->state &=~PESO_RUNNING; break;
        case 0x24: pemac->state |= PESO_ESCAPED * ((*((*st)+1))==0x3A); break;
        case 0x3B: pemac->state &=~PESO_ESCAPED * ((*((*st)+1))==0x3E); break;

        default  : break;

    };

    if(lstate && !(pemac->state & PESO_ESCAPED)) {
        lstate=0; (*st)+=2; d[0].x=(**st);

    };

//   ---     ---     ---     ---     ---

    CALOUT('P', "\e[56;0HPE_RUNNING %d\
                 \e[57;0HPE_ESCAPED %d\
                 \e[58;0HPE_ESCREAD %d",

                 pemac->state & PESO_RUNNING,
                 pemac->state & PESO_ESCAPED,
                 pemac->state & PESO_ESCREAD);

    if(pemac->state & PESO_ESCREAD) {
        INCSTK(&(pemac->stack));

    };

//   ---     ---     ---     ---     ---

    if(pemac->state & PESO_ESCAPED) {       // process current state

        (*st)+=2; d[0].x=(**st); lstate=1;

        switch(d[0].x) {
            case 0x00: return;
            case 0x25: pemac->state |= PESO_ESCREAD; (*st)++; break;
            case 0x2F: pemac->state &=~PESO_ESCREAD; (*st)++; break;

            default  : break;

        };

//   ---     ---     ---     ---     ---

        uint   i = 0;
        PEOP* op = &PESO_OPDUMMY;

        CLMEM2(pemac->rdop, PESO_OPWIDTH);

        do {

            if(**st && **st != 0x3B) {
                pemac->rdop[i]=(**st);
                i++; (*st)++; continue;

            }; break;


        } while(**st && **st != 0x3B);

//   ---     ---     ---     ---     ---

        pemac->rdop[i]='\0';

        STR_HASHGET(GNAMES, pemac->rdop, op, 0       );
        CALOUT('P', "\e[60;0H0x%" PRIXPTR "\e[60;10H:\
                     \e[60;12HPE_OPCODE %s"          ,
                     (uintptr_t) op, pemac->rdop     );
        if(op) {
            CALOUT('P', " -> INS 0x%02X", op->code.x);

            if(pemac->state & PESO_ESCREAD) {
                STACKPUSH(byref(pemac->stack), 0);

            } else {
                STACKPOP(byref(pemac->stack), pemac->args.F);

            }; PEMAC_OPCODES[op->code.x](&(pemac->args));

        } else { CALOUT('P', " -> CANNOT FETCH"); }
        goto VERIFY_PE_STATE;

    }

//   ---     ---     ---     ---     ---

    else {
        CALOUT('P', "\e[68;0H\e[2KWRITING %c", d[0].x);
        switch(last) {                      // printing

            case 0x00: return;
            case 0x0A: t[1]=(-1)-t[0]; t[3]-=(t[2]*1.5); break;
            case 0x08: t[1]-=t[0]; break;

            default  : t[1]+=t[0]; break;

        };

    };                                                                                      };

//   ---     ---     ---     ---     ---

static int   anim_i = 0;

void DRCANVAS(void)                         {

    glDisable                               (GL_DEPTH_TEST                               );
    glUseProgram                            (canvasProgram                               );
    glBindVertexArray                       (canvasVAO                                   );

    tstr         = st_src;
    char  last   = '\0';

    int   i      = 0;

    float sc[2]; GTCHRSZ(sc, 16);
    float t [4]  = { sc[0], -1.0, sc[1], 1.0 };
    ustr8 d [4]  = { 0,0,0,0                 };

    pemac->state = PESO_RUNNING;

    if(anim_i==12) { anim_i=0; };

    do {

        RDCANVAS                            (&tstr, last, d, t                        );
        glUniform4f                         (canvasUniforms[0], t[0], t[1], t[2], t[3]);
        glUniform4ui                        (canvasUniforms[1], d[0].F, d[1].F        ,
                                                                d[2].F, d[3].F        );

        glDrawArrays                        (GL_TRIANGLES, 0, 6                       );

        last=d[0].x;

        // if(i==anim_i) { anim_i++; break; } i++;

    } while(*tstr++);

    d[0].x=226;                             // draw cursor
    glUniform4ui                            (canvasUniforms[1], d[0].F, d[1].F        ,
                                                                d[2].F, d[3].F        );

    glDrawArrays                            (GL_TRIANGLES, 0, 6                       );

    glEnable                                (GL_DEPTH_TEST                            );
    RSTSHDLOC                               (                                         );    };

//   ---     ---     ---     ---     ---

void DLCANVAS(void)                         {

    for (uint i=0; i<2; i++) {
        glDetachShader                      (canvasProgram, canvasShader[i]);
        glDeleteShader                      (canvasShader[i]               );

    };

    glDeleteProgram                         (canvasProgram                 );
    glDeleteBuffers                         (1, &canvasVBO                 );
    glDeleteBuffers                         (1, &canvasVAO                 );

    DLMEM(pemac); DLMEM(GNAMES);                                                            };

//   ---     ---     ---     ---     ---