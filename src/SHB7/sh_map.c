//**************************
// SH_MAP                  *
// shared memory with mmap *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "sh_map.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>


//   ---     ---     ---     ---     ---
// #:0x0;> globals

static int map_fd;
static void* map;

//   ---     ---     ---     ---     ---

void ntmap(void) {

  struct stat statbuf;
  map_fd=open("./shb7wt",O_RDWR);

  map=mmap(
    NULL,
    0x4000-1,
    PROT_READ|PROT_WRITE,
    MAP_SHARED,
    map_fd,
    0

  );
};

void dlmap(void) {
  munmap(map,0x4000-1);
  close(map_fd);

};

//   ---     ---     ---     ---     ---

void* ntproc(void) {

  ntmap();

  if(!map) {
    printf("Map failed @%s\n",__func__);
    close(map_fd);
    return NULL;

  };return map;

};

void* ntshb7(void) {

  FILE *fp = fopen("./shb7wt", "w+");
  fseek(fp, 0x4000 , SEEK_SET);
  fputc('\0', fp);fclose(fp);

  ntmap();

  if(!map) {
    printf("Map failed @%s\n",__func__);
    close(map_fd);
    return NULL;

  };return map;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

