#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Bin.cpp"

int main(void) {

  std::unique_ptr<char[]> buff=
    Bin::orc("./fmat");

  std::cout << (buff.get()==NULL) << std::endl;

  buff.reset();

  std::cout << (buff.get()==NULL) << std::endl;

};
