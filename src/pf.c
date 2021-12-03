// deleeeeeeeeeete this test file already!

#include "KVRNEL/MEM/sh_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

// make shb7 && gcc -L/cygdrive/d/lieb_git/kvr/bin/x64/ -lkvrnel pf.c -o ../bin/x64/pf.exe

int main(int argc,char* argv[]) {

  void** mh=ntmap_heir(argv[1]);

  if(!mh) {
    printf("Oh noes\n");
    kill(getppid(),SIGCONT);
    return -1;

  };char* bf=(char*) (*mh);

  strcpy(bf,"$#$$$$$$$$$$$$$$");
  dlmap(mh);

  kill(getppid(),SIGCONT);

  return 0;

};