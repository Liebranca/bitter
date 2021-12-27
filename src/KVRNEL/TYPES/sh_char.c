//**************************
// SH_CHAR                 *
// chars plus meta         *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTIBUTORS             *
// lyeb,                   *
//**************************

#include "sh_char.h"

//   ---     ---     ---     ---     ---

static size_t line_width=0;
static size_t line_height=0;

static size_t cursor_x=0;
static size_t cursor_y=0;

static ichar* dr_buff=NULL;

static char gl_col=0x00;
static char gl_ctl=0x1F;

#define cursor_pos cursor_x+(cursor_y*line_width)
#define dr_at dr_buff[cursor_pos]

//   ---     ---     ---     ---     ---

void sticsc(
  ichar* buff,
  size_t x,
  size_t y

) {

  dr_buff=buff;

  line_width=x;
  line_height=y;

};ichar* rfdrbuf(void) {return dr_buff;};

//   ---     ---     ---     ---     ---

void sticpos(size_t* pos) {
  cursor_x=pos[0];
  cursor_y=pos[1];

};

void sticcol(char col) {gl_col=col;};
void sticctl(char ctl) {gl_ctl=ctl;};

void gticpos(size_t* pos) {
  pos[0]=cursor_x;pos[1]=cursor_y;

};size_t gticposr(void) {
  return cursor_pos;

};

//   ---     ---     ---     ---     ---

int wlenic(ichar* c) {

  int len=0;
  while(c->ch && c->ch!=' ') {
    len++;c++;

  };return len;

};

//   ---     ---     ---     ---     ---

void wric(char c) {

  dr_at=(ichar) {
    c,gl_col,gl_ctl

  };cursor_x++;

  if(cursor_x>=line_width || c=='\n') {

    cursor_x=0;
    cursor_y++;

  };

};

//   ---     ---     ---     ---     ---

void bkic(void) {

  dr_at=(ichar) {0x20};
  cursor_x-=1*(cursor_pos>=2);

  if(cursor_x>=line_width) {
    cursor_x=line_width-1;
    cursor_y--;

    cursor_y*=0+(cursor_y<line_height);

  };

};

//   ---     ---     ---     ---     ---

void prompt_cap(void) {
  cursor_x=(cursor_pos<2) ? 2 : cursor_x;
  dr_at=(ichar) {0xE2,0x06,gl_ctl};

};

//   ---     ---     ---     ---     ---
