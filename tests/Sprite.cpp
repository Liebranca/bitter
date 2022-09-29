#include "kvrnel/src/Vec.cpp"
#include "kvrnel/src/Sprite.cpp"
#include "kvrnel/src/Clock.cpp"

#include <locale.h>
#include <unistd.h>

int main(void) {

  setlocale(LC_ALL,"");

  long fps=12;
  Clock clk((float) fps);

  while(fps--) {

    clk.draw();
    std::wcout << std::endl;

    clk.tick(0);

  };

  return 0;

};
