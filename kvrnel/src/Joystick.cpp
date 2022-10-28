// ---   *   ---   *   ---
// JOYSTICK
// PSX happens
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <fcntl.h>
  #include <stdio.h>
  #include <unistd.h>

  #include "bitter/kvrnel/Joystick.hpp"

// ---   *   ---   *   ---
// nit

Joystick::Joystick(int id) {

  // TODO: mod path by id
  std::string path="/dev/input/js0";

  m_fd=open(path.c_str(),O_RDONLY);

  // ^non-blocking reads
  int flags=fcntl(m_fd,F_GETFL,0);
  flags|=O_NONBLOCK;
  fcntl(m_fd,F_SETFL,flags);

  // get button and axis count
  ioctl(m_fd,JSIOCGAXES,&m_ax_cnt);
  ioctl(m_fd,JSIOCGBUTTONS,&m_bt_cnt);

  // ^make state arrays
  m_ax.resize(m_ax_cnt);

};

// ---   *   ---   *   ---
// ^del

Joystick::~Joystick(void) {
  close(m_fd);

};

// ---   *   ---   *   ---
// raw unbuffered read from device

int Joystick::next_event(void) {

  int out=AR_DONE;
  uint64_t rb;

  rb=read(m_fd,&m_ev,sizeof(m_ev));
  out-=rb!=sizeof(m_ev);

  return out;

};

// ---   *   ---   *   ---

void Joystick::bt_event(void) {

  if(m_ev.value) {
    m_bt|=1<<m_ev.number;

  } else {
    m_bt&=~ (1<<m_ev.number);

  };

  printf(
    "BT %1X : %b\n",
    m_ev.number,(m_bt>>m_ev.number)&1

  );

};

void Joystick::ax_event(void) {
  m_ax[m_ev.number]=float(m_ev.value)/AXIS_MAX;

  printf(
    "AX %1X : %f\n",
    m_ev.number,m_ax[m_ev.number]

  );

};

// ---   *   ---   *   ---
// reads in next event from device

void Joystick::get_input(void) {

  while(this->next_event() >= 0) {
    m_ev.type&= ~JS_EVENT_INIT;

    switch(m_ev.type) {

    case JS_EVENT_BUTTON:
      bt_event();
      break;

    case JS_EVENT_AXIS:
      ax_event();
      break;

    };

  };

};

// ---   *   ---   *   ---
