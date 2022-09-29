// ---   *   ---   *   ---
// BYTES
// common binary ops
//
// LIBRE SOFTWARE
// Licenced under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,
//

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Bytes.hpp"
  #include <cmath>
  #include <cstdio>

// ---   *   ---   *   ---
// get nth bit is set

template <typename T>
inline bool nthbit(T b,T n) {
  return (b&(1<<n))!=0;

};

// ---   *   ---   *   ---
// count N bits needed for value

template <typename T>
T bitsize(T x) {

  T bsize=0;
  T trail=0;

  for(T b=0;b<8;b++) {

    T bitset=nthbit<T>(x,b);

    if(bitset) {

      bsize+=trail+1;
      trail=0;

    } else {trail++;};

  };

  return bsize;

};

// ---   *   ---   *   ---
// give portion of a value's bits

template <typename T>
T bitslice(

  T b,

  T iStart,
  T iEnd

) {

  int x=0b0;
  int bit;

  for(T i=iStart;i<iEnd+1;i++) {

    bit=nthbit<T>(b,i);

    if(bit) {
      x|=bit<<(i-iStart);

    };

  };

  return x;

};

// ---   *   ---   *   ---
// clamps floats along a linear range
// encodes them as integers

template <typename T>
T frac(float x,float step,int nbits) {

  long b=roundf(x/step);

  size_t hibit=1<<nbits;
  size_t max=hibit-1;

  hibit=(hibit>>1)-1;
  b+=hibit;

  return (T) (b&max);

};

// ---   *   ---   *   ---
// ^decodes frac'd floats

template <typename T>
float unfrac(
  T b,

  float step,
  int nbits,

  int unsig

) {

  size_t hibit=1<<nbits;
  size_t max=hibit-1;

  hibit=(hibit>>1)-1;
  b-=hibit;

  return (b*step);

};

//// ---   *   ---   *   ---
//// move this to lymath later
//
//float clampf(float v,
//float start,
//float end) {if(v<start) {v
// =start;}
//else if(v>end) {v=end;};
//
//return v;};
//
//// ---   *   ---   *   ---
//
//float FRACTOFL(
//  uint frac,
//  uint maxval,
//  float fac,
//  uint shift) {
//
//  if(frac==maxval-1) {frac=maxval;};
//  return ((int) (frac-shift))* fac;
//
//};
//
//uchar FLTOFRAC(
//  float v,
//  float maxval,
//  float fraction,
//  uint fac,
//  uint shift) {
//
//  v=clampf(v,-maxval,maxval-fraction);
//  return (uchar) roundf(v * fac)+shift;
//
//};
//
//// ---   *   ---   *   ---
//
//static uint CFRAC_L=6;      // default FRAC level
//                            // for compression
//
//static uint CFRAC_D0=2;     // degrade step,
//                            // reduces precision
//
//static uint CFRAC_D1=4;     // further degrade
//                            // step, destroys
//                            // precision
//
//void STCFRACL(uint level) {
//  CFRAC_L=clampui(level,0,7);
//
//};
//
//// ---   *   ---   *   ---
//
//void ENCVRT(float* v,CRKVRT* c) {
//
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//                            // i dont care what
//                            // anybody says, Y is
//                            // UP
//  c->co_x=FLTOFRAC(
//    v[0],4,ff*CFRAC_D1,
//    fi/CFRAC_D1,fi
//
//  );c->co_y=FLTOFRAC(
//    v[2],4,ff*CFRAC_D1,
//    fi/CFRAC_D1,fi
//
//  );c->co_z=FLTOFRAC(
//    -v[1],4,ff*CFRAC_D1,
//    fi/CFRAC_D1,fi
//
//  );
//
//                            // same for normals
//  c->nr_x=FLTOFRAC(
//    v[3],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->nr_y=FLTOFRAC(
//    v[5],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->nr_z=-v[4]<0;
//
//                            // ... and tangents
//  c->tn_x=FLTOFRAC(
//    v[6],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->tn_y=FLTOFRAC(
//    v[8],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->tn_z=-v[7]<0;
//
//  c->bhand=v[9]<0;
//
//  c->u=FLTOFRAC(v[10],1,ff,fi,0);
//  c->v=FLTOFRAC(v[11],1,ff,fi,0);
//
//};
//
//// ---   *   ---   *   ---
//
//void ENCNVEC(
//  float* n,
//  JOJPIX* j) {
//
//
//  JOJVEC* v=(JOJVEC*) j;
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  v->x=FLTOFRAC((n[0]*2.0)-1.0,1,ff,fi,fi);
//  v->y=FLTOFRAC((n[1]*2.0)-1.0,1,ff,fi,fi);
//  v->w=FLTOFRAC(n[3],1,ff,fi,0);
//
//  v->sign=n[2]<0;
//
//};
//
//// ---   *   ---   *   ---
//
//void DECNVEC(float* n,JOJPIX* j) {
//
//
//  JOJVEC* v=(JOJVEC*) j;
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  float x=FRACTOFL(v->x,fi,ff,fi);
//  float y=FRACTOFL(v->y,fi,ff,fi);
//  float w=FRACTOFL(v->w,fi,ff,0);
//
//  int sign=(v->sign)?-1 :
//  1;
//  float z=0;
//
//  float d=1-(pow(x,2)+pow(y,2));
//  if(d>0) {z
//   =sqrt(d)* sign;};
//
//  n[0]=(x+1)*0.5;
//  n[1]=(y+1)*0.5;
//  n[2]=(z+1)*0.5;
//  n[3]=w;
//
//};
//
//// ---   *   ---   *   ---
//
//uchar GREY4STP(float v) {
//  return((v>0.25&&v<=0.75)||v>=0.95)|((v>0.75)<<1);
//
//};
//
//void ENCGREY(float* p,JOJPIX* j) {
//
//
//  JOJGREY* o=(JOJGREY*) j;
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  o->a=GREY4STP(p[0]);
//  o->b=GREY4STP(p[1]);
//  o->c=GREY4STP(p[2]);
//  o->d=GREY4STP(p[3]);
//
//};
//
//// ---   *   ---   *   ---
//
//void DECGREY(float* p,JOJPIX* j) {
//
//
//  JOJGREY* o=(JOJGREY*) j;
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  p[0]=o->a*0.35f;
//  p[1]=o->b*0.35f;
//  p[2]=o->c*0.35f;
//  p[3]=o->d*0.35f;
//
//};
//
//// ---   *   ---   *   ---
//
//void ZJC_pack_rawvert(
//  VP3D* vert,
//  CRKVRT* data) {
//
//  ;
//
//  /* old fun. rewrite.
//  uint co=((FLTOFRAC(data->co [0],4,FRAC5,32,128))
//  |(FLTOFRAC(data->co [1],4,FRAC5,32,128)<<8)
//  |(FLTOFRAC(data->co [2],4,FRAC5,32,128)<<16));
//
//  uint nnn=((FLTOFRAC(data->normal
//    [0],1,FRAC5,32,32))
//  |(FLTOFRAC(data->normal [1],1,FRAC4,16,16)<<6)
//  |((data->normal [2]<0)<<11)
//  |(((int)data->bhand)<<12));
//
//  uint
//    ttt=((FLTOFRAC(data->tangent[0],1,FRAC5,32,32))
//  |(FLTOFRAC(data->tangent[1],1,FRAC5,32,32)<<6)
//  |((data->tangent[2]<0)<<12));
//
//  uint uvs=((FLTOFRAC(data->uv [0],1,FRAC7,64,64))
//  |(FLTOFRAC(data->uv [0],1,FRAC7,64,64)<<7));
//
//  vert->frac1=((co)|((nnn&0xFF)<<24));
//  vert->frac2=(((nnn&(31
//    <<8))>>8)|(ttt<<5)|(uvs<<18));*/
//}
//
//// ---   *   ---   *   ---
//// try encoding four pixels at a time someday
//
//void ENCRGBA(float* p,JOJPIX* j) {
//
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  float r=p[0];
//  float g=p[1];
//  float b=p[2];
//  float a=p[3];
//
//  j->luma=FLTOFRAC(
//    (0.257f * r)
//   +(0.504f * g)
//   +(0.098f * b),
//    1,ff,fi,0
//
//  );
//
//  // this is wrong: it creates color artifacts
//  // i just so happen to __LOVE__ the glitchy look
//
//  j->chroma_u=FLTOFRAC(
//    (-0.148f * r)
//   -(0.291f * g)
//   +(0.439f * b),
//    1,ff,fi,fi
//
//  );j->chroma_v=FLTOFRAC(
//    (0.439f * r)
//   -(0.368f * g)
//   -(0.071f * b),
//    1,ff,fi,fi
//
//  );
//
////  // the 'right way', not nearly as cool
////
////  j->chroma_u=FLTOFRAC(
////    (-0.148f* r)
////   -(0.291f *g)
////   +(0.439f * b),
////    1,ff*CFRAC_D0,
////    fi/CFRAC_D0,
////    fi/CFRAC_D0
////
////  );j->chroma_v=FLTOFRAC(
////    (0.439f* r)
////   -(0.368f *g)
////   -(0.071f * b),
////    1,ff*CFRAC_D0,
////    fi/CFRAC_D0,
////    fi/CFRAC_D0
////
////  );
//
//  j->alpha=FLTOFRAC(a,1,ff*CFRAC_D1,fi/CFRAC_D1,0);
//
//};
//
//// ---   *   ---   *   ---
//
//void DECRGBA(float* p,JOJPIX* j) {
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  float luma=FRACTOFL(j->luma,fi,ff,0)* 1.164000f;
//
//  // color artifacts 10/10
//  float chr_u=FRACTOFL(
//    j->chroma_u,fi,ff,fi
//
//  );float chr_v=FRACTOFL(
//    j->chroma_v,fi,ff,fi
//
//  );
//
////  // the 'right way'
////  float chr_u=FRACTOFL(
////    j->chroma_u,
////    fi,ff*CFRAC_D0,
////    fi/CFRAC_D0
////
////  );float chr_v=FRACTOFL(
////    j->chroma_v,
////    fi,ff*CFRAC_D0,
////    fi/CFRAC_D0
////
////  );
//
//  float alpha=FRACTOFL(
//    j->alpha,fi/CFRAC_D1,ff*CFRAC_D1,0
//
//  );p[0]=(luma)+(1.596f * chr_v);
//  p[1]=(luma)-(0.392f * chr_u)-(0.813f * chr_v);
//  p[2]=(luma)+(2.017f * chr_u);
//  p[3]=alpha;
//
//};
//
////   ---     ---     ---     ---     ---
//// #:0xF;>

