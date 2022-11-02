#include "bitter/kvrnel/Clock.hpp"
#include "bitter/kvrnel/Joystick.hpp"

#include <iostream>

int main(void) {

  Clock clk(10);
  Joystick joy(0);

  if(joy.miss()) {
    printf("No deal!\n");
    goto END;

  };

// ---   *   ---   *   ---

  {int panic=600;

  uint8_t x=0;

  while(panic--) {

    clk.tick(0);
    joy.get_input();

    if(joy.axis(PSX::MOVE_X)>0.5) {
      x++;

    } else if(joy.axis(PSX::MOVE_X)<-0.5) {
      x--;

    };

    std::cout

    << "\e[22;1H\e[2K"

    << "\e[22;"
    << (x&63)
    << "Ho"

    << std::endl;

    if(joy.axis(PSX::MOVE_Y)>0.5) {
      break;

    };

  }};

// ---   *   ---   *   ---

END:

  return 0;

};

// ---   *   ---   *   ---
