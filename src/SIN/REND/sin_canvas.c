//**************************
// SIN_CANVAS              *
// quad by quad drawing    *
// used for chars          *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "sin_canvas.h"
#include "KVRNEL/MEM/kvr_mem.h"

#include "sin_globals.h"
#include "sin_shader.h"

#include "SHADERS/sin_CanvasShader.h"

//   ---     ---     ---     ---     ---
// #:0x0;>

static uint canvasVAO=0;
static uint canvasVBO=0;
static uint canvasProgram=0;
static uint canvasShader [2]= {0,0};
static uint canvasUniforms[2]= {0,0};

//   ---     ---     ---     ---     ---
// #:0x1;>


void NTCANVAS(void) {

                            // just a shorthand
  const SHDP* params=&SIN_CanvasShader;

  float quadVertices[]= {   // draw rect
    0.0f,1.0f,
    0.0f,0.0f,
    1.0f,0.0f,
    0.0f,1.0f,
    1.0f,0.0f,
    1.0f,1.0f

  };

//   ---     ---     ---     ---     ---
// #:0x2;>

  // gl alloc
  glGenVertexArrays(1,&canvasVAO);
  glGenBuffers(1,&canvasVBO);
  glBindVertexArray(canvasVAO);

  // load data
  glBindBuffer(GL_ARRAY_BUFFER,canvasVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),

  &quadVertices,GL_STATIC_DRAW);

  // set attrib pointers
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,
  2 * sizeof(float),(void*) 0);

//   ---     ---     ---     ---     ---
// #:0x3;>


  // make shaders and attach
  canvasProgram=glCreateProgram();

  canvasShader[0]=CMPSHD(

    params->source_v,
    params->num_vsources,

    GL_VERTEX_SHADER

  );canvasShader[1]=CMPSHD(

    params->source_f,
    params->num_fsources,

    GL_FRAGMENT_SHADER

  );

  for(uint i=0;i<2;i++) {

    glAttachShader(canvasProgram,canvasShader[i]);

  };

  // fix attrib locations
  for(uchar attribLoc=0;
  attribLoc<params->num_attribs;
  attribLoc++) {

    glBindAttribLocation(
      canvasProgram,
       0,params->attribs[attribLoc]);

  };

//   ---     ---     ---     ---     ---
// #:0x4;>


  // errcatch boiler
  glLinkProgram(canvasProgram);

  CHKSHDERR(
    canvasProgram,
    GL_LINK_STATUS,
    1,"Shader linking failed"

  );glValidateProgram(canvasProgram);

  CHKSHDERR(
    canvasProgram,
    GL_VALIDATE_STATUS,
    1,"Shader validation failed"

  );

  // locate uniforms
  for(uchar i=0;i<params->num_uniforms;i++) {

    canvasUniforms[i]=glGetUniformLocation(
      canvasProgram,
       params->uniforms[i]);

  };
};

//   ---     ---     ---     ---     ---
// #:0x5;>


void BEGPSH(void) {

  glDisable(GL_DEPTH_TEST);
  glUseProgram(canvasProgram);
  glBindVertexArray(canvasVAO);

};

void PSHCHR(float* t,ustr8* d) {

  glUniform4fv(canvasUniforms[0],1,t);
  glUniform4uiv(canvasUniforms[1],1,(uint*) d);
  glDrawArrays(GL_TRIANGLES,0,6);

};

void ENDPSH(float* t,ustr8* d) {

  glUniform4fv(canvasUniforms[0],1,t);
  glUniform4uiv(canvasUniforms[1],1,(uint*) d);
  glDrawArrays(GL_TRIANGLES,0,6);
  glEnable(GL_DEPTH_TEST);
  RSTSHDLOC();

};

//   ---     ---     ---     ---     ---
// #:0x6;>


void DLCANVAS(void) {


  for(uint i=0;i<2;i++) {

    glDetachShader(canvasProgram,canvasShader[i]);
    glDeleteShader(canvasShader[i]);

  };

  glDeleteProgram(canvasProgram);
  glDeleteBuffers(1,&canvasVBO);
  glDeleteBuffers(1,&canvasVAO);

};

//   ---     ---     ---     ---     ---
// #:0x7;>

