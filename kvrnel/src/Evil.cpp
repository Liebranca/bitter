// ---   *   ---   *   ---
// EVIL
// hoh sys
//
// LIBRE SOFTWARE
// Licenced under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <cstring>
  #include <iostream>

  #include "kvrnel/Evil.hpp"

// ---   *   ---   *   ---

cx8 HEXCHARS[]={

  '0','1','2','3',
  '4','5','6','7',
  '8','9','A','B',
  'C','D','E','F'

};

std::string hexstr(
  uint64_t x,
  uint8_t  sz

) {

  std::string out(sz,'0');

  for(int8_t i=0;i<sz;i++) {
    out[(sz-1)-i]=char(HEXCHARS[x&0xF]);
    x>>=4;

  };

  return out;

};

// ---   *   ---   *   ---

static Evil _evil=Evil();

Evil& Evil::get(void) {
  return _evil;

};

Evil::Errme& Evil::top_err(void) {
  return Evil::m_log.back();

};

void Evil::set_err(
  const std::string& info

) {

  top_err().info=info;

};

// ---   *   ---   *   ---
// shortens path

char* Evil::shpath(const char* path) {

  char* file = (char*) path;
  char  c    = '\0';

  // iter through string
  while (*path++){

    // is forward slash
    c=*path;
    if(c== '/') {

      // cut here
      if(strlen(path)<=FLEN_TGT)  {
        file=(char*) ++path;
        break;

      };

    };

  };

  return file;

};

// ---   *   ---   *   ---
// avoid recursion or you'll see this one

void Evil::on_max_depth(Evil::Errme& me) {

  if(m_depth > CSTK_LEN) {

    std::cerr
    << "Call depth exceeded\n"

    << "#:!;> " << me.file << ' '
    << "at " << me.func << ' '

    << "(line " << me.line << ')'
    << std::endl
    ;

  };

};

// ---   *   ---   *   ---
// fill callbranch so that
//
// depth 0
// \--> depth 1
// .  \--> depth 2
// .
// \--> depth 1
//
// etc.

std::string Evil::get_callbranch(
  Evil::Errme& me

) {

  std::string callbranch(66,'\0');

  callbranch[0]='\b';
  callbranch[1]='\0';

  if(m_depth>1) {
    on_max_depth(me);

    // branch start
    callbranch[0]='\\';

    // branch end
    callbranch[m_depth+1] = '>';
    callbranch[m_depth+2] = '\0';

    // fill between
    for(int x=1;x<m_depth+1;x++) {
      callbranch[x]='-';

    };

  } else {
    callbranch[0]='\n';
    callbranch[1]='\0';

  };

  return callbranch;

};

// ---   *   ---   *   ---

void Evil::geterrloc(

  const char* file,
  const char* func,

  uint64_t    line

) {

  Evil::Errme me={

    .depth = m_depth++,
    .line  = line,

    .loc   = std::string(ERRS_LEN+1,'\0'),

    .file  = std::string(file),
    .func  = std::string(func)

  };

  auto callbranch=get_callbranch(me);

  // paste to logs
  me.loc+=

    std::string(PALETTE[1])

  + callbranch+std::to_string(m_ptr)

  + ":\e[0m "+std::string(PALETTE[0])
  +"<"+me.file+">"

  + "\e[0m on func "+std::string(PALETTE[2])
  + me.func

  + "\e[0m line "+std::string(PALETTE[3])
  + std::to_string(me.line)
  + "\e[0m\n"
  ;

  // move to next entry
  m_log.push_back(me);
  m_ptr++;

};

// ---   *   ---   *   ---
// spit out entries

void Evil::printlog(bool flush) {

  for(int i=0;i<m_ptr;i++) {
    std::cerr << PALETTE[4] << m_log[i].loc;

    // 'delete' this entry ;>
    if(flush) {
      m_log[i].loc[0]='\0';

    };

  };

  // reset counters if need
  if(flush) {m_ptr=0;};

};

// ---   *   ---   *   ---
// 'delete' last entry

void Evil::poplog(void) {

  if(m_ptr) {
    m_ptr--;

    m_log[m_ptr].loc[0]='\0';
    m_log[m_ptr].depth=0;

  };

  m_depth-=m_depth!=0;

};

// ---   *   ---   *   ---

uint8_t Evil::errtype(
  const Evil::Errcode& ee

) {

  uint8_t out=0;
  std::string mstart;

  // really bad error
  if(ee.type==AR_FATAL) {

    mstart+=PALETTE[6];

    m_state=AR_FATAL;
    out=6;

  // plain bad error
  } else {

    mstart+=PALETTE[7];

    m_state=AR_ERROR;
    out=7;

  };

  mstart+="#:!;> ";

  // let the printing begin
  std::cerr
  << "\n" << mstart

  << PALETTE[3]
  << "<ERR:" << hexstr(ee.code,4) << "> "

  << PALETTE[4]
  ;

  return out;

};

// ---   *   ---   *   ---
// ill be back

void Evil::terminator(
  const Evil::Errcode& ee

) {

  auto info  = top_err().info;

  auto color = errtype(ee);
  auto mbody = std::string(ee.mess)+" ";

  std::cerr

  << mbody << PALETTE[0]
  << '<' << info << '>'

  << "\n"
  << PALETTE[color] << "#:!;> "
  << PALETTE[4] << "TRACE:"
  << std::endl
  ;

  printlog(false);

  if(m_state==AR_FATAL) {
    abort();

  } else {
    exit(m_state);

  };

};

// ---   *   ---   *   ---
