// ---   *   ---   *   ---
// JOJ
// Image degrader
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,
// ---   *   ---   *   ---

// deps
  #include "zjc_joj.h"
  #include "kvr_zwrap.h"

  #include "../KVRNEL/kvr_paths.h"
  #include "../KVRNEL/MEM/kvr_str.h"
  #include "../KVRNEL/TYPES/zjc_hash.h"

  #include <string.h>

// ---   *   ---   *   ---
// fun pointer for swapping out encoders
static void(*JOJENC_F) (
  float* pix,
  JOJPIX * j

);

// dummy format for hashing colors
typedef struct JOJ_PALETTE_COLOR {

  ID id;      // lookup identifier
  JOJPIX pix; // the actual color values

} JOJPC;

static JOJPC JOJ_PALETTE[ZJC_DAFSIZE];

// addr table
static HASH* JOJ_PALHASH;

// ---   *   ---   *   ---

uint CALCJOJHSIZ (
  uint entries,
  uint colors

) {

  // size of header
  return (

    (sizeof (uint) * 4)
    + (entries * ZJC_IDK_WIDTH)
    + (colors * sizeof (JOJPIX)

  );
};

// ---   *   ---   *   ---

static uint JOJ_CVALUES[4] = { 4, 1, 1, 1 };
void STJOJDIM (uint dim) {
  JOJ_CVALUES[1] = dim;
  JOJ_CVALUES[2] = dim * dim;

}; void GTJOJLST (void) {return JOJ_CVALUES[0];};

// ---   *   ---   *   ---
// pixels read each run

#define JOJ_READCOUNT         \
  xELEMS(ZJC_DAFPAGE, JOJPIX) \
  /ZJC_RAWCOL_ELEMS

// ---   *   ---   *   ---

int NTJOJENG (int mode) {

  jpalette=MKHASH(6,"JOJ_PALETTE");
  int evilstate;

  // make a convenience string
  MEM* m=MKSTR(GTPECWD(),6,0);
  char* path=MEMBUFF(m,char,0);

// ---   *   ---   *   ---
  BIN** joj;           // fetch adeq bin slot
  // for current op

  if (mode == JOJ_ENCODE) {
  joj = GTBINTO ();}
  else {
  joj = GTBINFR ();};
  // ensure joj is wiped when we *write* to it
  int mate_rmode =
  (mode ==
   JOJ_ENCODE) ? KVR_FMODE_WB : KVR_FMODE_RBP;
  // set fname and open
  RPSTR (&m, "\\JOJ", strlen (path));
  BINOPEN ((*joj), path, mate_rmode,
           KVR_FTYPE_JOJ, 0, evilstate);
// ---   *   ---   *   ---
// op-dependent nit

  if (mode == JOJ_ENCODE) {
  NTDEFL ();};
  else {
  NTINFL ();};
// ---   *   ---   *   ---
  // free convenience string
  DLMEM (m); return DONE;};

// ---   *   ---   *   ---
int DLJOJENG (int mode) {

  CALOUT ('E', "Palette: %u/%u colors used",
          jpalette->nitems,
          jpalette->nslots * 8);
  DLMEM (jpalette);
// ---   *   ---   *   ---

  BIN ** joj;           // fetch adeq bin slot
  // for current op
  if (mode == JOJ_ENCODE) {
  joj = GTBINTO (); DLDEFL ();}
  else {
  joj = GTBINFR (); DLINFL ();};
  // close
  BINCLOSE ((*joj)); DLMEM (*joj);
  return DONE;};

// ---   *   ---   *   ---
int ENCJOJ (
  float* src,
  HRW * h

) {

  int wb; // fetch
  BIN ** to = GTBINTO (); MEM * wt = GTLNG ();
  // get buff from static mem
  uchar * buff = MEMBUFF (wt, uchar, 0); JOJPIX jpix = { 0 }; // empty, 
                                                              // 
  // 
  // intermediate 
  // write-to
//   ---     ---     ---     ---     ---
  // set encoder
  if (JOJ_CVALUES[0] == 4) {
  JOJ_CVALUES[0] = 0;}

  if (!JOJ_CVALUES[0]) {
  JOJENC_F = &ENCRGBA; STCFRACL (4);}
  elif (JOJ_CVALUES[0] == 1) {
  JOJENC_F = &ENCVEC; STCFRACL (1);}
  else {
  JOJENC_F = &ENCGREY;}; JOJ_CVALUES[0]++;
//   ---     ---     ---     ---     ---
  JOJPC * jpc = NULL;   // hash fetch target
  void *p_ptr = NULL;   // voidstar dummy for
  // jpc-get
  char idname[ZJC_IDK_WIDTH]; // buff for color 
                              // 
  // 
  // id key
//   ---     ---     ---     ---     ---
  for (uint x = 0, y = h->size_i;
       x < (JOJ_CVALUES[2] * ZJC_RAWCOL_ELEMS);
       x += ZJC_RAWCOL_ELEMS, y++) {

  // get the color
  JOJENC_F (src + x, &jpix);
  // make an id
  snprintf (idname, ZJC_IDK_WIDTH - 1,
            "%u.%u%.%u.%u", jpix.luma,
            jpix.chroma_u, jpix.chroma_v,
            jpix.alpha);
  // check id against palette
  STR_HASHGET (JOJ_PALHASH, idname, p_ptr, 0);
//   ---     ---     ---     ---     ---
  if (!p_ptr) {         // on new color, save
  // to palette

  if (JOJ_PALHASH->nitems == ZJC_DAFSIZE) {

  // this errcheck isnt trivial
  // failing it means a full stop, redo
  // the image list, then run blkmgk again
  // but hopefully you'll never see it
  CALOUT ('E', "BAD JOJ: using over %u colors",
          ZJC_DAFSIZE); return ERROR;};

//   ---     ---     ---     ---     ---
  // set values on new entry
  jpc = JOJ_PALETTE + (JOJ_PALHASH->nitems);
  jpc->id = IDNEW ("COL*", idname);
  jpc->pix = jpix;
  // insert on table
  HASHSET (JOJ_PALHASH, jpc);}
  // on repeat color, fetch from palette
  else {
  jpc = (JOJPC *) p_ptr;}; buff[y] = jpc->n; // save 
                                             // 
  // color index to buff

  };
//   ---     ---     ---     ---     ---
  h->size_i += JOJ_CVALUES[2] // add to
  // archive's
  // total
  return DONE;

};

//   ---     ---     ---     ---     ---
          int DECJOJ (JOJH * jojh) {

          int wb; uint dim = jojh->dim;
          uint sq_dim = dim * dim;
          // set color compression level
          STCFRACL (jojh->fracl);
          // get the joj
          JOJPIX * buff = GTJOJ (JOJ_TO);
//   ---     ---     ---     ---     ---
          JOJENC_F = &DECRGBA;  // set encoder 
          int len = strlen (jojh->name);
          char imtype = jojh->name[len - 1];
          if (imtype == 'n') {
          JOJENC_F = &DECNVEC;}
          elif (imtype == 'o' || imtype == 'c') {
          JOJENC_F = &DECGREY;};
//   ---     ---     ---     ---     ---
          // sizing ints we need later
          uint page = JOJ_READCOUNT;
          uint remain = sq_dim;
          // get recycle pixel buffer
          float *pixels = MEMBUFF (JOJ_PIXELS, float,
                                   0);

          // rewind target and write header
          rewind (JOJ_PIXDUMP->file);
          fseek (JOJ_PIXDUMP->file, 0, SEEK_CUR);
          BINWRITE (JOJ_PIXDUMP, wb, JOJH, 1, jojh);
          fseek (JOJ_PIXDUMP->file, 0, SEEK_CUR);
//   ---     ---     ---     ---     ---
          while (remain) {      // read one joj-wide
          // page at a time
          // each joj being 8(fracl'd)-bit Y'UV+A

          uint readsize =
          (remain < page) ? remain : page;
          // load buffer from dump
          BINREAD (JOJ_TO, wb, JOJPIX, readsize, buff); remain -= readsize; // discount 
                                                                            // 
          // 
          // read 
          // from 
          // pending
          for (uint x = 0, y = 0; y < (readsize); // go 
                                                  // 
               // 
               // through 
               // page 
               // and 
               // joj 
               // decode
               x += ZJC_RAWCOL_ELEMS, y++) {
          JOJENC_F (pixels + x, buff + y);};
          // save decoded pixels so caller can read
          BINWRITE (JOJ_PIXDUMP, wb, float,
                    readsize * ZJC_RAWCOL_ELEMS,
                    pixels);}; return DONE;};

//   ---     ---     ---     ---     ---
          int ZPJOJ (HRW * h) {

          // get header size
          h->offs_d = CALCJOJHSIZ (h->total / 4, JOJ_PALHASH->nitems); int wb; // add 
                                                                               // 
          // 
          // sizings 
          // to 
          // header
          uint sizes[4] =
          { h->size_i, h->size_d, h->total,
          JOJ_PALHASH->nitems
          };
          // fetch
          BIN ** to = GTBINTO (); MEM * wt = GTLNG ();
          // get buff from static mem
          uchar * buff = MEMBUFF (wt, uchar, 0);
          // rewind TO and skip sig
          rewind ((*to)->file);
          fseek ((*to)->file, 0, SEEK_CUR);
          fseek ((*to)->file, sizeof (SIG), SEEK_CUR);
          fseek ((*to)->file, 0, SEEK_CUR);
//   ---     ---     ---     ---     ---
          // quick blank mem for a flood fill
          MEM * m = MKSTR ("ZPBLNK", offs_d, 1);
          // add padding so we can jump without
          // crashing
          fseek ((*to)->file, 0, SEEK_CUR);
          BINWRITE ((*to), wb, uchar, h->offs_d,
                    m->buff);
          fseek ((*to)->file, 0, SEEK_CUR);
          // free blank mem
          DLMEM (m);
//   ---     ---     ---     ---     ---
          // zip the buff
          DEFLBUF (buff, (*to), h->size_i, &(h->size_d),
                   0, 0);
//   ---     ---     ---     ---     ---
          // write size data
          fseek ((*to)->file, 0, SEEK_CUR);
          BINWRITE ((*to), wb, uint, ARRSIZE (sizes),
                    sizes);
          fseek ((*to)->file, 0, SEEK_CUR);
//   ---     ---     ---     ---     ---
          // make temp for palette write values
          MEM * pal = MKSTR ("JOJPAL", sizeof (JOJPIX) * JOJ_PALHASH->nitems, 1); for (uint x = 0; x < h->total; x++) { // copy 
                                                                                                                        // 
          // 
          // palette 
          // data 
          // to 
          // buff

          JOJPC jpc = JOJ_PALETTE[x];
          pal[x] = jpc->pix;};
//   ---     ---     ---     ---     ---
          // write palette data
          fseek ((*to)->file, 0, SEEK_CUR);
          BINWRITE ((*to), wb, JOJPIX,
                    JOJ_PALHASH->nitems, pal);
          fseek ((*to)->file, 0, SEEK_CUR); DLMEM (pal);
          return DONE;};
//   ---     ---     ---     ---     ---
          int UZPJOJ (JOJH * jojh, uint * sizes) {

          // fetch bin & buff
          BIN ** from = GTBINFR (); MEM * rd = GTBINRD (); uchar * buff = MEMBUFF (rd, uchar, 0); int wb; // read 
                                                                                                          // 
          // 
          // file 
          // header
          fseek ((*from)->file, 0, SEEK_CUR);
          BINREAD ((*from), wb, uint, 4, sizes);
          BINREAD ((*from), wb, JOJH, sizes[2], jojh);
          fseek ((*from)->file, 0, SEEK_CUR);
//   ---     ---     ---     ---     ---
          // unzip
          INFLBUF ((*from), buff, sizes[0], sizes[1], 0,
                   PKD_JOJH_SIZE);
          // rewind & skip sig
          rewind (JOJ_TO->file);
          fseek (JOJ_TO->file, 0, SEEK_CUR);
          fseek (JOJ_TO->file, sizeof (SIG), SEEK_CUR);
          fseek (JOJ_TO->file, 0, SEEK_CUR);
          return DONE;};
//   ---     ---     ---     ---     ---
