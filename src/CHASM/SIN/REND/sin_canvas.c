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

#include "SIN/sin_globals.h"
#include "SIN/sin_shader.h"

#include "SIN/SHADERS/sin_CanvasShader.h"

#include <string.h>

//   ---     ---     ---     ---     ---

static uint   canvasVAO         = 0;
static uint   canvasVBO         = 0;
static uint   canvasProgram     = 0;
static uint   canvasShader  [2] = { 0, 0 };
static uint   canvasUniforms[2] = { 0, 0 };

//   ---     ---     ---     ---     ---

void NTCANVAS(void)                         {

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

static int anim_i=0;

void DRCANVAS()                             {

    glUseProgram                            (canvasProgram                               );
    glBindVertexArray                       (canvasVAO                                   );

    char* st   = "Hello world!";
    int   i    = 0;

    float t[4] = { 0.1, -0.75, 0.75, 0.0 };
    if(anim_i==(strlen(st)+1)) { anim_i=0; };

    do {

        t[3]=(*st);
        glUniform4f(canvasUniforms[0], t[0], t[1], t[2], t[3]);

        glDrawArrays                            (GL_TRIANGLES, 0, 6                      );

        t[1]+=t[0];
        if(i==anim_i) { anim_i++; break; } i++;

    } while(*st++);

    RSTSHDLOC                               (                                            ); };

//   ---     ---     ---     ---     ---

void DLCANVAS(void)                         {

    for (uint i=0; i<2; i++) {
        glDetachShader                      (canvasProgram, canvasShader[i]);
        glDeleteShader                      (canvasShader[i]               );

    };

    glDeleteProgram                         (canvasProgram                 );
    glDeleteBuffers                         (1, &canvasVBO                 );
    glDeleteBuffers                         (1, &canvasVAO                 );               };

//   ---     ---     ---     ---     ---