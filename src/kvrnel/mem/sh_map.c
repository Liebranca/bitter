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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <inttypes.h>

#include <sys/mman.h>

//   ---     ---     ---     ---     ---
// #:0x0;> globals

static int map_fd;
static void* map;

static size_t page_sz=0;

//   ---     ---     ---     ---     ---

typedef struct {

  void* ptr;

  int fd;
  size_t sz;

  char fpath[256];
  char* cne;

} MAPHED;

//   ---     ---     ---     ---     ---

void qitos(char* dst,uint64_t v,int sz) {

  for(int x=0;x<(sz*2);x++) {
    char c='0';
    uint64_t masked=(v&(0xFLL<<(x*4)))>>(x*4);
    if(masked<10) {
      c=0x30+masked;

    } else {
      c=0x41+(masked-10);

    };dst[x]=c;

  };
};

uint64_t nqstoi(char* src,int sz) {

  uint64_t v=0;
  for(int x=0;x<(sz*2);x++) {
    uint64_t c=src[x];
    if(c<0x3A) {
      c=0x00+(c-0x30);

    } else {
      c=0x0A+(c-0x41);

    };v|=c<<(x*4);
  };return v;
};

//   ---     ---     ---     ---     ---

char* encodemh(void** mh_b) {

  MAPHED* mh=(MAPHED*) mh_b;

  char* dst=(mh->cne+0);

  int x=0;int y=0;
  qitos(dst+x,(uintptr_t) mh->ptr,sizeof(mh->ptr));
  x+=sizeof(mh->ptr)*2;

  qitos(dst+x,mh->fd,sizeof(mh->fd));
  x+=sizeof(mh->fd)*2;

  qitos(dst+x,mh->sz,sizeof(mh->sz));
  x+=sizeof(mh->sz)*2;

  for(;y<sizeof(mh->fpath);x++,y++) {
    dst[x]=mh->fpath[y];

  };

  /* if i want to debug the encode/decode

  printf("%s\n",mh->cne);

  printf(
    "%"PRIXPTR"%s%"PRIXPTR"%"PRIXPTR"\n",
    mh->ptr,mh->fpath,mh->fd,mh->sz

  );*/

  return mh->cne;

};

//   ---     ---     ---     ---     ---

void decodemh(
  MAPHED* mh,
  char* key) {

  int x=0;int y=0;

  mh->ptr=(void*) nqstoi(key+x,sizeof(mh->ptr));
  x+=sizeof(mh->ptr)*2;

  mh->fd=nqstoi(key+x,sizeof(mh->fd));
  x+=sizeof(mh->fd)*2;

  mh->sz=nqstoi(key+x,sizeof(mh->sz));
  x+=sizeof(mh->sz)*2;

  for(;y<sizeof(mh->fpath);x++,y++) {
    mh->fpath[y]=key[x];

  };

  mh->cne=NULL;

  /* if i want to debug the encode/decode

  printf("%s\n",key);

  printf(
    "%"PRIXPTR"%s%"PRIXPTR"%"PRIXPTR"\n",
    mh->ptr,mh->fpath,mh->fd,mh->sz

  );*/

};

//   ---     ---     ---     ---     ---

void gtsysd(void) {
  page_sz=sysconf(_SC_PAGE_SIZE)-1;

};

void mkmap(MAPHED* mh) {

  mh->fd=open(mh->fpath,O_RDWR);

  mh->ptr=mmap(
    mh->ptr,
    (mh->sz)-1,
    PROT_READ|PROT_WRITE,
    MAP_SHARED,
    mh->fd,
    0

  );

};

void dlmap(void** mh_b) {

  MAPHED* mh=(MAPHED*) mh_b;

  munmap(mh->ptr,(mh->sz)-1);
  close(mh->fd);

  if(mh->cne) {
    free(mh->cne);

  };free(mh);

};

//   ---     ---     ---     ---     ---

void** ntmap_solo(
  const char* fpath,
  size_t fsz

) {

  MAPHED* mh=(MAPHED*) malloc(sizeof(MAPHED));
  mh->cne=NULL;

  strncpy(
    mh->fpath,
    fpath,
    255

  );mh->ptr=NULL;

  mh->sz=fsz;
  mkmap(mh);

  if(!mh->ptr) {
    printf("Map failed @%s\n",__func__);
    close(mh->fd);
    return NULL;

  };return (void**) mh;

};

//   ---     ---     ---     ---     ---

void** ntmap_origin(
  const char* fpath,
  int pages) {

  MAPHED* mh=(MAPHED*) malloc(sizeof(MAPHED));
  mh->cne=(char*) malloc(sizeof(MAPHED)*2);

  if(!page_sz) {
    gtsysd();

  };mh->sz=page_sz*pages;

  FILE *fp = fopen(fpath, "w+");
  fseek(fp, page_sz*pages, SEEK_SET);
  fputc(0x00, fp);fclose(fp);

//   ---     ---     ---     ---     ---

  strncpy(
    mh->fpath,
    fpath,
    255

  );mh->ptr=NULL;

  mkmap(mh);

  if(!mh->ptr) {
    printf("Map failed @%s\n",__func__);
    close(mh->fd);
    return NULL;

  };return (void**) mh;

};

int mapfd(void** mh_a) {
  MAPHED* mh=(MAPHED*) mh_a;
  return mh->fd;

};

//   ---     ---     ---     ---     ---

void** ntmap_heir(char* key) {

  MAPHED* mh=(MAPHED*) malloc(sizeof(MAPHED));

  decodemh(mh,key);
  mkmap(mh);

  if(!mh->ptr) {
    printf("Map failed @%s\n",__func__);
    close(mh->fd);
    return NULL;

  };return (void**) (mh);

};

//   ---     ---     ---     ---     ---
// #:0x1;>
