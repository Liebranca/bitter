#include "bitter/kvrnel/Clock.hpp"
#include "bitter/kvrnel/Joystick.hpp"

int main(void) {

  Clock clk(10);
  Joystick joy(0);

  if(joy.miss()) {
    printf("No deal!\n");
    goto END;

  };

// ---   *   ---   *   ---

  {int panic=600;

  while(panic--) {

    clk.tick(0);
    joy.get_input();

    if(joy.axis(PSX::MOVE_Y)>0.5) {
      break;

    };

  }};

// ---   *   ---   *   ---

END:

  return 0;

};

// ---   *   ---   *   ---
