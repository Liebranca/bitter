// ---   *   ---   *   ---
// BIN
// Talks to your disk
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

#include <cstring>
#include <filesystem>

#include "kvrnel/Evil.hpp"
#include "kvrnel/Bin.hpp"

//   ---     ---     ---     ---     ---
// reads mode flags

void Bin::set_mode(char mode) {

  m_mode^=m_mode;

  while(mode) {
  switch(mode) {

// ---   *   ---   *   ---

  case READ:
    m_mode|=std::ios::in;
    mode&=~READ;

    break;

  case WRITE:
    m_mode|=std::ios::out;
    mode&=~WRITE;

    break;

// ---   *   ---   *   ---

  case TRUNC:
    m_mode|=std::ios::trunc;
    mode&=~TRUNC;

    break;

  case APPEND:
    m_mode|=std::ios::app;
    mode&=~APPEND;

    break;

// ---   *   ---   *   ---
// we always open in binary mode
// read the name of the effn class

  }};

  m_mode|=std::ios::binary;

};

// ---   *   ---   *   ---
// opens a file

Bin::Bin(std::string fpath,char mode) {

  this->set_mode(mode);
  m_fh.open(fpath,m_mode);

  // errchk
  if(!m_fh.is_open()) {

    Evil::terminator(
      Error::OPEN_FAIL,
      fpath.c_str()

    );

  };

// ---   *   ---   *   ---
// signature check

  size_t sig_len=strlen(m_fsig);

  if(sig_len) {

    char sig[128]={0};
    m_fh.read(sig,SLIMIT);

    // errchk
    if(strcmp(m_fsig,sig)) {

      Evil::terminator(
        Error::BAD_SIG,
        fpath.c_str()

      );

    };

  };

// ---   *   ---   *   ---
// get size for later buffer alloc

  m_size=std::filesystem::file_size(fpath);

  m_size-=sig_len;
  m_size-=m_header_sz;

};

// ---   *   ---   *   ---

Bin::~Bin(void) {

  std::cout << "Closed!" << std::endl;

  if(m_fh.is_open()) {
    m_fh.close();

  };

};

// ---   *   ---   *   ---

std::unique_ptr<char[]> Bin::orc(
  std::string fpath

) {

  Bin b(fpath,READ);

  std::unique_ptr<char[]> buff(
    new char[b.m_size]

  );

  b.m_fh.read(buff.get(),b.m_header,);

  return buff;

};

//// ---   *   ---   *   ---
//
//int RDBIN(BIN*bin) {
//
//  // get readmode, then path from buff
//  char* rmode=GTRMODE(bin->mode);
//  char* path=MEMBUFF(byref(bin->m),char,0);
//
//  int isnew=(!strcmp(rmode,"wb+")    // just so we know if we created something
//|!strcmp(rmode,"wb"));
//
//
//  // we consider it a reading sesh if either
//  int r=(!strcmp(rmode,"rb+")        // -mode is read for update (input & output)
//|!strcmp(rmode,"rb"));     // -mode is read only       (just input    )
//
////   ---     ---     ---     ---     ---
//
//  // open and reopen (once) on a failed read
//OPEN:bin->file=fopen(path,rmode);
//  if (bin->file==NULL) {
//
//    if (isnew > 0) {
//      return ERROR;
//    }
//    elif(r) {
//      rmode="wb+";
//      isnew=1;
//      goto OPEN;
//    };
//  };
//
////   ---     ---     ---     ---     ---
//
//  if (!isnew) {                 // not new? then check it's a valid file
//
//    SIG sig;                    // read to this
//    int rb;                     // bytes read
//
//    // read & compare
//    BINREAD(bin,rb,SIG,1,&sig);
//    BINSIG(bin,sig);
//
//    if (BADSIG) {
//      return ERROR;
//    };
//  }
////   ---     ---     ---     ---     ---
//
//  else {                        // new? then MAKE IT valid
//    int wb;
//
//    BINWRITE(bin,wb,SIG,1,&(bin->sign));
//  };
//
////   ---     ---     ---     ---     ---
//
//  CALOUT(F,"Opened file <%s>\n\b",path);
//
//  if (!strcmp(rmode,"wb+")) {
//    isnew=2;
//  };
//  return isnew;
//};
//
////   ---     ---     ---     ---     ---    // just a convenience getter
//
//char* PTHBIN(BIN*bin) {
//  return MEMBUFF(byref(bin->m),char,0);
//};
//
////   ---     ---     ---     ---     ---
//
//int BIN2BIN(uint size) {
//
//  int wb;
//  uchar* buff=(uchar *) KVR_RD->buff;
//
//  CALOUT(F,"Writting from <%s> to <%s>\n\b",
//         PTHBIN(KVR_FR),PTHBIN(KVR_TO));
//
//  while (size) {
//    uint readsize=
//      (size < ZJC_DAFSIZE)?size:ZJC_DAFSIZE;
//
//    fseek(KVR_FR->file,0,SEEK_CUR);
//    BINREAD(KVR_FR,wb,uchar,readsize,buff);
//    fseek(KVR_FR->file,0,SEEK_CUR);
//
//    fseek(KVR_TO->file,0,SEEK_CUR);
//    BINWRITE(KVR_TO,wb,uchar,readsize,buff);
//    fseek(KVR_TO->file,0,SEEK_CUR);
//
//    size-=readsize;
//
//  };
//  return DONE;
//};
//
////   ---     ---     ---     ---     ---
//
//int DLBIN(BIN*bin) {
//
//  // errcatch before you print success
//  int failure=fclose(bin->file);
//
//  if (failure) {
//    return ERROR;
//  }
//
//  CALOUT(F,"File closed <%s>\n\b",PTHBIN(bin));
//
//  bin->file=NULL;
//  return DONE;
//};
//
////   ---     ---     ---     ---     ---
//
//int RMBIN(BIN*bin) {
//
//  // close before delete, then errcatch
//  if (bin->file) {
//    BINCLOSE(bin);
//  };
//  int retx=remove(PTHBIN(bin));
//
//  if (retx) {
//    return ERROR;
//  }
//
//  CALOUT(F,"Deleted file <%s>\n\b",PTHBIN(bin));
//
//
//  return DONE;
//};
//
////   ---     ---     ---     ---     ---
