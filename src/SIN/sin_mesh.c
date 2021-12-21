#include "sin_mesh.h"

#include "sin_globals.h"
#include "sin_shader.h"

#include "SHADERS/sin_SketchShader.h"

#include <stdio.h>

const float ffr=1.0f/32.0f;

//   ---     ---     ---     ---     ---

typedef struct {

  union {
    struct {
      char x;
      char y;
      char z;
      char w;

    };char mem[4];

  };

} vertex;

//   ---     ---     ---     ---     ---

static int batch;
static int batch_vao;

static int batch_program;

enum shader_types {shd_vert,shd_frag,shd_num};
static int batch_shaders[shd_num]={0,0};

void mk_meshbat(void) {

  // gl alloc
  glGenVertexArrays(1,&batch_vao);
  glGenBuffers(1,&batch);
  glBindVertexArray(batch_vao);

  // get your brain around this tri
  vertex mesh[]={

    0x80,0x80,0x00,0xFF,
    0x00,0x7F,0x00,0xFF,
    0x7F,0x80,0x00,0xFF

  };

  // load data
  glBindBuffer(GL_ARRAY_BUFFER,batch);
  glBufferData(
    GL_ARRAY_BUFFER,
    SIN_MSHBSZ*sizeof(vertex),

    (void*) mesh,
    GL_DYNAMIC_DRAW

  );

  // set attrib pointers
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(
    0,
    4,
    GL_BYTE,
    sizeof(vertex),
    NULL

  );

//   ---     ---     ---     ---     ---

  const SHDP* params=&SIN_SketchShader;

  // make shaders and attach
  batch_program=glCreateProgram();

  batch_shaders[shd_vert]=CMPSHD(

    params->source_v,
    params->num_vsources,

    GL_VERTEX_SHADER

  );batch_shaders[shd_frag]=CMPSHD(

    params->source_f,
    params->num_fsources,

    GL_FRAGMENT_SHADER

  );

  for(int i=0;i<shd_num;i++) {
    glAttachShader(batch_program,batch_shaders[i]);

  };

  // fix attrib locations
  for(uchar attribLoc=0;
  attribLoc<params->num_attribs;
  attribLoc++) {

    glBindAttribLocation(
      batch_program,
       0,params->attribs[attribLoc]);

  };

//   ---     ---     ---     ---     ---
// #:0x4;>


  // errcatch boiler
  glLinkProgram(batch_program);

  CHKSHDERR(
    batch_program,
    GL_LINK_STATUS,
    1,"Shader linking failed"

  );glValidateProgram(batch_program);

  CHKSHDERR(
    batch_program,
    GL_VALIDATE_STATUS,
    1,"Shader validation failed"

  );
};

//   ---     ---     ---     ---     ---

void dr_meshbat(void) {
  glUseProgram(batch_program);
  glBindVertexArray(batch_vao);
  glDrawArrays(GL_TRIANGLES,0,3);

};

void dl_meshbat(void) {

  for(int i=0;i<shd_num;i++) {

    glDetachShader(batch_program,batch_shaders[i]);
    glDeleteShader(batch_shaders[i]);

  };

  glDeleteBuffers(1,&batch);
  glDeleteVertexArrays(1,&batch_vao);

};

//   ---     ---     ---     ---     ---