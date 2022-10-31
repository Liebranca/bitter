// ---   *   ---   *   ---
// boring details that'd make the header
// too long for the casual reader
// meant to be pasted in!

#ifndef __24_JOJ_TILES_H__
#define __24_JOJ_TILES_H__

public:

// ---   *   ---   *   ---

typedef struct {

  uint64_t x;
  uint64_t y;

  uint64_t sz;
  uint64_t sz_sq;
  uint64_t sz_i;

  uint64_t cnt;
  uint64_t cnt_sq;

  std::unique_ptr<JOJ::Pixel> data;

// ---   *   ---   *   ---
// true if chunks are identical

  bool cmp(
    uint64_t idex_a,
    uint64_t idex_b

  ) {

    bool out=true;

    idex_a*=this->sz_sq;
    idex_b*=this->sz_sq;

    JOJ::Pixel* pixel=this->data.get();

    for(uint64_t i=0;i<this->sz_sq;i++) {

      if(pixel[idex_a++]!=pixel[idex_b++]) {
        out=false;
        break;

      };

    };

    return out;

  };

// ---   *   ---   *   ---
// return ptr to tile at [x,y]

  JOJ::Pixel* get(uint64_t x,uint64_t y) {

    return this->data.get()+sq_idex(

      x*this->sz_sq,
      y,

      this->sz_sq*this->sz

    );

  };

// ---   *   ---   *   ---
// send duplicate of tile to buffer

  std::unique_ptr<JOJ::Pixel> copy(
    JOJ::Pixel* pixel

  ) {

    std::unique_ptr<JOJ::Pixel> out(
      new JOJ::Pixel[this->sz_sq]

    );

    JOJ::Pixel* out_p=out.get();

    memcpy(
      out_p,pixel,

      this->sz_sq
    * sizeof(JOJ::Pixel)

    );

    return out;

  };

// ---   *   ---   *   ---
// rotates tile 90 degrees clockwise

  void ror(uint64_t off_x,uint64_t off_y) {

    auto pixel = this->get(off_x,off_y);
    auto buff  = this->copy(pixel);

    JOJ::Pixel* buff_p=buff.get();

    for(uint64_t y=0;y<this->sz;y++) {
    for(uint64_t x=0;x<this->sz;x++) {
      pixel[sq_idex(this->sz_i-y,x,this->sz)]=
        buff_p[sq_idex(x,y,this->sz)];

    }};

  };

// ---   *   ---   *   ---
// ^anti-clockwise

  void rol(uint64_t off_x,uint64_t off_y) {

    auto pixel = this->get(off_x,off_y);
    auto buff  = this->copy(pixel);

    JOJ::Pixel* buff_p=buff.get();

    for(uint64_t y=0;y<this->sz;y++) {
    for(uint64_t x=0;x<this->sz;x++) {
      pixel[sq_idex(y,this->sz_i-x,this->sz)]=
        buff_p[sq_idex(x,y,this->sz)];


    }};

  };

// ---   *   ---   *   ---
// invert on X

  void x_mirror(uint64_t off_x,uint64_t off_y) {

    auto pixel = this->get(off_x,off_y);
    auto buff  = this->copy(pixel);

    JOJ::Pixel* buff_p=buff.get();

    for(uint64_t y=0;y<this->sz;y++) {
    for(uint64_t x=0;x<this->sz;x++) {
      pixel[sq_idex(this->sz_i-x,y,this->sz)]=
        buff_p[sq_idex(x,y,this->sz)];

    }};

  };

// ---   *   ---   *   ---
// ^on Y

  void y_mirror(uint64_t off_x,uint64_t off_y) {

    auto pixel = this->get(off_x,off_y);
    auto buff  = this->copy(pixel);

    JOJ::Pixel* buff_p=buff.get();

    for(uint64_t y=0;y<this->sz;y++) {
    for(uint64_t x=0;x<this->sz;x++) {
      pixel[sq_idex(x,this->sz_i-y,this->sz)]=
        buff_p[sq_idex(x,y,this->sz)];

    }};

  };

// ---   *   ---   *   ---

} Tiles;

// ---   *   ---   *   ---

#endif // __24_JOJ_TILES_H__
