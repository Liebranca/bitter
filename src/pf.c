// just a test file. remove later

#include "shb7/sh_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

// gcc shb7/sh_map.c pf.c -o pf.exe

int main(void) {


  char* bf=ntproc();
  if(!bf) {
    printf("Oh noes\n");
    kill(getppid(),SIGCONT);
    return -1;

  };

  strcpy(bf,"$#$$$$$$$$$$$$$$");
  dlmap();

  kill(getppid(),SIGCONT);

  return 0;

};