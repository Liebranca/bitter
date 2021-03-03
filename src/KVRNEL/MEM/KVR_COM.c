#include "KVR_COM.h"
#include <string.h>

//   ---     ---     ---     ---     ---

void EXNEW(COM* c, RET* r)                  {

    ; //PRSKLS(c->val[0]);

};

//   ---     ---     ---     ---     ---

RET RDCOM(COM* c)                           {

    RET r = {0};

    if(c == NULL) { return r; };

    switch(c->ins)                          {

        case  0: EXNEW(c, &r); break;
        default:               break;

                                            };

    return r;

};

//   ---     ---     ---     ---     ---