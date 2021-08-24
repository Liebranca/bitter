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

//   ---     ---     ---     ---     ---

static uint   canvasVAO       = 0;
static uint   canvasVBO       = 0;
static uint   canvasProgram   = 0;
static uint   canvasShader[2] = { 0, 0 };

//   ---     ---     ---     ---     ---

void NTCANVAS(void)                         {

                                            // just a shorthand
    const SHDP* shader =            &SIN_CanvasShader;

                                            // draw rect
    float quadVertices[]       =            { -1.0f,  1.0f,  0.0f,  1.0f,                \
                                              -1.0f, -1.0f,  0.0f,  0.0f,                \
                                               1.0f, -1.0f,  1.0f,  0.0f,                \
                                              -1.0f,  1.0f,  0.0f,  1.0f,                \
                                               1.0f, -1.0f,  1.0f,  0.0f,                \
                                               1.0f,  1.0f,  1.0f,  1.0f                 };

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
                                             4 * sizeof(float), (void*) 0                );
    glEnableVertexAttribArray               (1                                           );
    glVertexAttribPointer                   (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float) ,
                                            (void*) (2 * sizeof(float))                  );

//   ---     ---     ---     ---     ---

                                            // make shaders and attach
    canvasProgram = glCreateProgram         (                                            );
    canvasShader[0] = CMPSHD                (shader->source_v, shader->num_vsources      ,
                                             GL_VERTEX_SHADER                            );
    canvasShader[1] = CMPSHD                (shader->source_f, shader->num_fsources      ,
                                             GL_FRAGMENT_SHADER                          );

    for (uint i=0; i<2; i++) {
        glAttachShader                      (canvasProgram, canvasShader[i]              );

    };

    for(uchar attribLoc = 0            ;
        attribLoc < shader->num_attribs;
        attribLoc++                    ) {  // fix attrib locations
            glBindAttribLocation            (canvasProgram, 0, shader->attribs[attribLoc]);

    };

//   ---     ---     ---     ---     ---

                                            // errcatch boiler

    glLinkProgram                           (canvasProgram                               );
    CHKSHDERR                               (canvasProgram, GL_LINK_STATUS               ,
                                             1, "Shader linking failed"                  );

    glValidateProgram                       (canvasProgram                               );
    CHKSHDERR                               (canvasProgram, GL_VALIDATE_STATUS           ,
                                             1, "Shader validation failed"               );

                                            // locate tex uniform
    int surfLoc = glGetUniformLocation      (canvasProgram, shader->samplers[0]          );
    glUniform1i                             (surfLoc, 0                                  ); };

//   ---     ---     ---     ---     ---

void DRCANVAS(uint texloc)                  {

    glUseProgram                            (canvasProgram                               );
    glActiveTexture                         (GL_TEXTURE0                                 );
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, texloc                 );

    glBindVertexArray                       (canvasVAO                                   );
    glDrawArrays                            (GL_TRIANGLES, 0, 6                          );

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