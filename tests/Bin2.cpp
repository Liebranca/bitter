#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Bin.cpp"

class Test: public Bin {

public:

  typedef struct {

    char sig[4];

    uint64_t pad0;
    uint64_t pad1;

  } Header;

// ---   *   ---   *   ---

private:

  vicstr m_fsig(void) {
    return "TEST";

  };

  vic64 m_header_sz(void) {
    return sizeof(Header);

  };

// ---   *   ---   *   ---

  Header m_hed;

// ---   *   ---   *   ---

public:

  Test(std::string fpath,char mode=Bin::READ) {
    this->open(fpath,mode);

  };

};

int main(void) {

  Test b("./hedtest",Bin::WRITE|Bin::TRUNC);

  char buff[32]={0x24,0x21,0x3B};
  b.write(buff,32);

  Test::Header hed;
  hed.pad0=0x2424;
  hed.pad1=0x2424;

  b.write_header(&hed);

  return 0;

};
