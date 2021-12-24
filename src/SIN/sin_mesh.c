#include "sin_mesh.h"

#include "sin_globals.h"
#include "sin_shader.h"

#include "SHADERS/sin_SketchShader.h"

#include "KVRNEL/LYMATH/mat4.h"

#include <stdio.h>
#include <stddef.h>

//   ---     ---     ---     ---     ---

typedef struct {

  union {
    struct {

      char x;// plane coordinates
      char y;
      char z;
      char w;

      char c;// color
      char n;// normal
      char t;// tangent

      char pad;

    };char mem[8];

  };

} vertex;

// get your brain around this tri
static vertex mesh[]={

  0x80,0x80,0x00,0xFF,0x0F,0x00,0x00,0x00,

  0x00,0x7F,0x00,0xFF,0x0C,0x00,0x00,0x00,

  0x7F,0x80,0x00,0xFF,0x0C,0x00,0x00,0x00

};

#define vertex_count \
  ((sizeof(mesh)/sizeof(vertex))-1)

//   ---     ---     ---     ---     ---

static int batch;
static int batch_vao;
static int batch_ubo;

static int batch_program;

enum shader_types {shd_vert,shd_frag,shd_num};
static int batch_shaders[shd_num]={0,0};

static vertex camera={
  0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00

};

//   ---     ---     ---     ---     ---

void mk_meshbat(void) {

  // gl alloc
  glGenVertexArrays(1,&batch_vao);
  glGenBuffers(1,&batch);
  glBindVertexArray(batch_vao);

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
  glVertexAttribPointer(
    0,
    4,
    GL_BYTE,GL_FALSE,
    sizeof(vertex),
    (void*) offsetof(vertex,x)

  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,
    4,
    GL_BYTE,GL_FALSE,
    sizeof(vertex),
    (void*) offsetof(vertex,c)

  );

//   ---     ---     ---     ---     ---

  glGenBuffers(1,&batch_ubo);
  glBindBuffer(GL_UNIFORM_BUFFER,batch_ubo);

  glBufferData(
    GL_UNIFORM_BUFFER,

    sizeof(mat4),
    NULL,

    GL_DYNAMIC_DRAW

  );glBindBufferBase(
    GL_UNIFORM_BUFFER,
    1,batch_ubo

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
       attribLoc,params->attribs[attribLoc]);

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

static size_t sel_vertex=0;

void dr_meshbat(int keys) {

  vertex* v=mesh+sel_vertex;
  //v->c=0xC;

  /*sel_vertex+=
   -(keys&0x10 && sel_vertex>0)
   +(keys&0x20 && sel_vertex<vertex_count);

  v=mesh+sel_vertex;
  v->c=0xF;

  v->x+=0x1*(
   -(keys&0x1 && v->x>-128)
   +(keys&0x2 && v->x<127)

  );v->y+=1*(
   -(keys&0x4 && v->y>-128)
   +(keys&0x8 && v->y<127)

  );*/

//   ---     ---     ---     ---     ---

  char cx=camera.x;
  char cy=camera.y;

  camera.x+=
   -((keys&0x10) && camera.x>-128)
   +((keys&0x20) && camera.x<127);

  camera.y+=
   -((keys&0x40) && camera.y>-128)
   +((keys&0x80) && camera.y<127);

  vec4 pos={0,0,48,ftoc(1)};
  vec4 at={camera.x,camera.y,0,ftoc(1)};
  vec4 up={0,32,0,ftoc(1)};

  mat4 view=lookat(pos,at,up);
  /*mat4 view={0};

  view.a=(vec4) {32,0,0,0};
  view.b=(vec4) {0,32,0,0};
  view.c=(vec4) {0,0,32,0};
  view.d=(vec4) {0,0,0,32};*/

  if(cx!=camera.x || cy!=camera.y) {
    prm(&view);

  };

  // upload modified geometry
  glBindBuffer(GL_ARRAY_BUFFER,batch);
  glBufferSubData(
    GL_ARRAY_BUFFER,0,
    3*sizeof(vertex),
    (void*) mesh

  );

  glBindBuffer(GL_UNIFORM_BUFFER,batch_ubo);
  glBufferSubData(
    GL_UNIFORM_BUFFER,0,
    sizeof(view),
    (void*) &view

  );

  // draw
  glUseProgram(batch_program);
  glBindVertexArray(batch_vao);
  glDrawArrays(GL_TRIANGLES,0,3);

};

void dl_meshbat(void) {

  for(int i=0;i<shd_num;i++) {

    glDetachShader(batch_program,batch_shaders[i]);
    glDeleteShader(batch_shaders[i]);

  };

  glDeleteBuffers(1,&batch_ubo);
  glDeleteBuffers(1,&batch);

  glDeleteVertexArrays(1,&batch_vao);

};

//   ---     ---     ---     ---     ---