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

#include "__sin.h"
#include "sin_globals.h"
#include "sin_shader.h"

#include "SHADERS/sin_CanvasShader.h"

#include <stdlib.h>

#include "sin_mesh.h"

//   ---     ---     ---     ---     ---
// #:0x0;>

static uint canvasVAO=0;
static uint canvasVBO=0;
static uint canvasUBO=0;
static uint canvasSSBO=0;

static uint canvasProgram=0;
static uint canvasShader[2]={0,0};
static uint canvasUniforms[2]={0,0};

static uint VertCount=0;
static uint CharCount=0;

//   ---     ---     ---     ---     ---
// #:0x1;>

void NTCANVAS(void) {

  mk_meshbat();

                            // just a shorthand
  const SHDP* params=&SIN_CanvasShader;

  float sc[2];GTCHRSZ(sc);
  uint ws[2];GTSCRSZ(ws);

  CharCount=ws[0]*ws[1];

  uint vsz=(CharCount)*(4*6)*sizeof(float);
  float* quadVerts=(float*) malloc(vsz);

  uint idex=0;

  float px=-1.0f;
  float py=1.0f;

  for(uint y=0;y<ws[1];y++) {
    for(uint x=0;x<ws[0];x++) {

      if(px>=1.0f) {
        px=-1.0f;
        py-=sc[1]*1.10;

      };

      uint chidex=(x+(y*ws[0]))<<1;
      uint videx=0;

      quadVerts[idex+0x00]=0|chidex;
      quadVerts[idex+0x01]=1|videx;
      quadVerts[idex+0x02]=px;
      quadVerts[idex+0x03]=py;

      quadVerts[idex+0x04]=0|chidex;
      quadVerts[idex+0x05]=0|videx;
      quadVerts[idex+0x06]=px;
      quadVerts[idex+0x07]=py;

      quadVerts[idex+0x08]=1|chidex;
      quadVerts[idex+0x09]=0|videx;
      quadVerts[idex+0x0A]=px;
      quadVerts[idex+0x0B]=py;

      quadVerts[idex+0x0C]=0|chidex;
      quadVerts[idex+0x0D]=1|videx;
      quadVerts[idex+0x0E]=px;
      quadVerts[idex+0x0F]=py;

      quadVerts[idex+0x10]=1|chidex;
      quadVerts[idex+0x11]=0|videx;
      quadVerts[idex+0x12]=px;
      quadVerts[idex+0x13]=py;

      quadVerts[idex+0x14]=1|chidex;
      quadVerts[idex+0x15]=1|videx;
      quadVerts[idex+0x16]=px;
      quadVerts[idex+0x17]=py;

      idex+=0x18;px+=(sc[0]);
      VertCount+=6;

    };
  };

//   ---     ---     ---     ---     ---
// #:0x2;>

  // gl alloc
  glGenVertexArrays(1,&canvasVAO);
  glGenBuffers(1,&canvasVBO);
  glBindVertexArray(canvasVAO);

  // load data
  glBindBuffer(GL_ARRAY_BUFFER,canvasVBO);
  glBufferData(GL_ARRAY_BUFFER,vsz,

  quadVerts,GL_STATIC_DRAW);

  // set attrib pointers
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,
  4 * sizeof(float),(void*) 0);

  free(quadVerts);

//   ---     ---     ---     ---     --- UBO as palette

  glGenBuffers(1,&canvasUBO);
  glBindBuffer(GL_UNIFORM_BUFFER,canvasUBO);

  glBufferData(
    GL_UNIFORM_BUFFER,

    64*sizeof(float),
    GTPAL(0), GL_DYNAMIC_DRAW

  );glBindBufferBase(
    GL_UNIFORM_BUFFER,
    0,canvasUBO

  );

//   ---     ---     ---     ---     --- SSBO as char array

  glGenBuffers(1,&canvasSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER,canvasSSBO);

  glBufferData(
    GL_SHADER_STORAGE_BUFFER,

    CharCount*sizeof(uint),
    NULL, GL_DYNAMIC_DRAW

  );glBindBufferBase(
    GL_SHADER_STORAGE_BUFFER,
    1,canvasSSBO

  );

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

  /*uint block_index=glGetUniformBlockIndex(
    canvasProgram,
    params->ubos[0]

  );glUniformBlockBinding(
    canvasProgram,
    block_index,
    0

  );*/

};

//   ---     ---     ---     ---     ---
// #:0x5;>


void BEGPSH(void) {

  glDisable(GL_DEPTH_TEST);
  glUseProgram(canvasProgram);

  float sc[4];GTCHRSZ(sc);
  glUniform4fv(canvasUniforms[0],1,sc);

};

void PSHCHR(uint* d, int update) {

  dr_meshbat(update);
  return;

  if(update) {
    glBindBuffer(GL_UNIFORM_BUFFER,canvasUBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,canvasSSBO);

    glBufferSubData(
      GL_SHADER_STORAGE_BUFFER,0,
      CharCount*sizeof(uint),(void*) d

    );

  };glDrawArrays(GL_TRIANGLES,0,VertCount);
};

void ENDPSH(void) {
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
  glDeleteBuffers(1,&canvasSSBO);
  glDeleteBuffers(1,&canvasUBO);
  glDeleteBuffers(1,&canvasVBO);
  glDeleteBuffers(1,&canvasVAO);

  dl_meshbat();

};

//   ---     ---     ---     ---     ---
// #:0x7;>
