#include "kvrnel/Vec.cpp"
#include "kvrnel/Sprite.cpp"
#include "kvrnel/Clock.cpp"

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
