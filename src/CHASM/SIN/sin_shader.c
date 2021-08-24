/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SIN_SHADER                            *
*                                           *
*     -redundant shader mang                *
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

#include "sin_shader.h"
#include "SIN/sin_globals.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

//   ---     ---     ---     ---     ---

typedef struct SIN_SHADER_PROGRAM {

    ID     id;

    uint   loc;
    uint   shaders [SIN_SHDLAYS];
    uint   uniforms[SIN_NUM_U  ];

    ushort flags;

} PRGM;

typedef struct SIN_SHDMANG {                // array of shader descriptors

    MEM  m;                                 // mem header

    uint used;                              // occupied slot counter
    PRGM slots[SIN_SHDNIDS];                // id array
    STK  stack;                             // avail @stktop

} SHDMNG;

//   ---     ---     ---     ---     ---

static SHDMNG* SIN_SHADERS  = NULL;
static HASH*   SIN_SHDHASH  = NULL;
static PRGM*   __program    = NULL;         // active program
static sint    __curShdLoc  = -1;           // idex of active program+1

//   ---     ---     ---     ---     ---

void RSTSHDLOC(void     )                   { __program=NULL; __curShdLoc=-1;               };

void STPRGM   (uint idex)                   { __program=(SIN_SHADERS->slots)+idex;          \
                                              __curShdLoc=idex+1;                           }

void BNDPRGM  (void     )                   { glUseProgram(__program->loc);                 }

int  CHKPRGRM (uint idex)                   {
    if(idex+1 != __curShdLoc)               { STPRGM(idex); BNDPRGM(); return 1; } return 0;}

//   ---     ---     ---     ---     ---

void NTSINSHD()                             {

                                            // set name and alloc to SIN_SHADERS
    ID id=IDNEW                             ("DATA", "_SIN_SHDIDS"                    );    \
    MEMGET                                  (SHDMNG, SIN_SHADERS                      ,     \
                                             xBYTES(SIN_SHDNIDS, uint), &id           );    \

                                            // init the slotstack
    MKSTK                                   (byref(SIN_SHADERS->stack), SIN_SHDNIDS   );
    for(uint y=SIN_SHDNIDS; y>0; y--)       { STACKPUSH(byref(SIN_SHADERS->stack), y-1);    };
                                                                                            };

void DLSINSHD()                             {
    for(uint idex=0     ;
        idex<SIN_SHDNIDS;
        idex++          )                   { if(SIN_SHADERS->slots[idex].loc)
                                            { DLSHD(idex); } }; DLMEM(SIN_SHADERS);    };

//  - --- - --- - --- - --- -

void CHKSHDERR(uint shader, uint flag  ,
               int isProgram           ,
               const char* errorMessage)    {

    char error[1024] = {0};
    int success      = 0;

    if(isProgram)                           { glGetProgramiv(shader, flag, &success);       }
    else                                    { glGetShaderiv (shader, flag, &success);       };

    if (success == GL_FALSE) {
        if (isProgram) {
            glGetProgramInfoLog             (shader, sizeof(error), NULL, error    );

        }

        else {
            glGetShaderInfoLog              (shader, sizeof(error), NULL, error    );

        }; CALOUT                           ('E', "%s: %s\n", errorMessage, error  );
           SIN_EVILSTATE++;

    };                                                                                      };

//   ---     ---     ---     ---     ---

uint CMPSHD(cchar** source  ,
            uint num_sources,
            uint shaderType )               {

    uint shader = glCreateShader            (shaderType                         );

    if (!shader) {                          // prelm duh errcatch
        CALOUT                              ('E', "Shader couldn't be created\n");

    };

                                            // compile and validate
    glShaderSource                          (shader, num_sources,(const char* const*) source, NULL  );
    glCompileShader                         (shader                             );
    CHKSHDERR                               (shader, GL_COMPILE_STATUS, 0       ,
                                             "Shader couldn't compile"          );

    return shader;                                                                          };

//  - --- - --- - --- - --- -

uint MKSHD(const SHDP* params)              {

    uint idex; STACKPOP                     (byref(SIN_SHADERS->stack), idex);

    PRGM program = SIN_SHADERS->slots[idex];// fill out the forms
    program.loc  = glCreateProgram          (                               );

//   ---     ---     ---     ---     ---

                                            // compile vert and frag shaders
    program.shaders[0] = CMPSHD             (params->source_v, params->num_vsources,
                                             GL_VERTEX_SHADER                      );
    program.shaders[1] = CMPSHD             (params->source_f, params->num_fsources,
                                             GL_FRAGMENT_SHADER                    );

    for (uint i=0; i<2; i++) {              // attach both to gl loc
        glAttachShader                      (program.loc, program.shaders[i]       );

    };

    for(uchar attrib_loc = 0            ;
        attrib_loc < params->num_attribs;
        attrib_loc++                    ) { // look for attrs
        glBindAttribLocation                (program.loc, attrib_loc               ,
                                             params->attribs[attrib_loc]           );

    };

                                            // link/erratch boiler
    glLinkProgram                           (program.loc                           );
    CHKSHDERR                               (program.loc, GL_LINK_STATUS, 1        ,
                                             "Shader linking failed"               );

                                            // bind so we can configure
    glUseProgram                            (program.loc                           );

//   ---     ---     ---     ---     ---    // find unis and set fixed locations

    for(uchar i = 0; i < params->num_uniforms; i++)
    {
        int uniform_loc = -1;

        if  (!strcmp(params->uniforms[i], "Model"                )) {
            uniform_loc = SIN_MODEL_U;

        }
        elif(!strcmp(params->uniforms[i], "ModelInverseTranspose")) {
            uniform_loc = SIN_NMAT_U;

        };

        if(uniform_loc >= 0) {
            program.uniforms[uniform_loc] = glGetUniformLocation(program.loc,
                                                                 params->uniforms[i]);

        };
    };

//   ---     ---     ---     ---     ---    // ^same, for ubos

    for(uchar i = 0; i < params->num_ubos; i++)
    {
        int block_binding = -1;

        if     (!strcmp(params->ubos[i], "Ambient")) { block_binding = 0; }
        else if(!strcmp(params->ubos[i], "Lights" )) { block_binding = 1; }

        if(block_binding >= 0)
        {
            uint block_index = glGetUniformBlockIndex(program.loc, params->ubos[i]);
            glUniformBlockBinding                    (program.loc, block_index    ,
                                                      block_binding               );

        };
    };

//   ---     ---     ---     ---     ---

    for(uchar i = 0; i < params-> num_samplers; i++)
    {
        int sampler_loc = glGetUniformLocation(program.loc, params->samplers[i]);
        int texslot     = 0;

        if     (!strcmp(params->samplers[i], "ShadowMap")) { texslot = 0;       }
        else if(!strcmp(params->samplers[i], "ENV"      )) { texslot = 1;       }
        else if(!strcmp(params->samplers[i], "Surface"  )) { texslot = 2;       }

        glUniform1i                                        (sampler_loc, texslot);

    };

    glValidateProgram(program.loc                                                   );
    CHKSHDERR        (program.loc, GL_VALIDATE_STATUS, 1, "Shader validation failed");

    SIN_SHADERS->used++; return idex;                                                       };

//   ---     ---     ---     ---     ---

void DLSHD(uint idex)                       {

    PRGM program = SIN_SHADERS->slots[idex];

    void** tmp;                             // dummy hash get-pop
    GTSINHASH                               (&(program.id), tmp, 1          );

    for (uint i=0; i<2; i++) {              // gl clean
        glDetachShader                      (program.loc, program.shaders[i]);
        glDeleteShader                      (program.shaders[i]             );

    }; glDeleteProgram                      (program.loc                    );

                                            // push free to slotstack
    STACKPUSH                               (byref(SIN_SHADERS->stack), idex);
    CLMEM2                                  (&program, sizeof(PRGM)         );

    SIN_SHADERS->used--;                                                                    };

//   ---     ---     ---     ---     ---
