/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    chRAT                                 *
*                                           *
*     -sdl mouse stuff                      *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "chRAT.h"

//  - --- - --- - --- - --- -

#define CH_RATS_ACTIVE  0x01
#define CH_RATS_IGNORED 0x02

//  - --- - --- - --- - --- -

void RESWINRAT(RAT* rat, ustr2 dim)         { rat->state=3;                                 \
                                              rat->abs.x=dim.x; rat->abs.y=dim.y;           \
                                              rat->rel.x=0;  rat->rel.y=0;                  };

void STOPWINRAT(RAT* rat)                   { USTWINRATACT(rat); rat->rel.x=0;              \
                                                                 rat->rel.y=0;              };

//   ---     ---     ---     ---     ---

void RUNWINRAT(RAT* rat,
               SDL_MouseMotionEvent* motion){ rat->abs.x=motion->x;                         \
                                              rat->abs.y=motion->y;                         \
                                              rat->rel.x=(motion->xrel * rat->sens);        \
                                              rat->rel.y=(motion->yrel * rat->sens);        \
                                                                                            \
                                              STWINRATACT(rat);                             };

//   ---     ---     ---     ---     ---

int  WALWINRAT(RAT* rat, ustr16 dim)        { return (   rat->abs.x == 0        )     \
                                                     | ((rat->abs.y == 0        ) << 1)
                                                     | ((rat->abs.x == dim.x - 1) << 2)
                                                     | ((rat->abs.y == dim.y - 1) << 3);    };

//   ---     ---     ---     ---     ---

ustr2 GTWINRATMOTION(RAT* rat, ustr2 dim,
                     float x            )   { SCUSTR16(&rat->rel, dim, x);                  };

//   ---     ---     ---     ---     ---

int  GTWINRATIG  (RAT* rat)                 { return rat->state & CH_RATS_IGNORED;          };
void USTWINRATIG (RAT* rat)                 { rat->state &= ~CH_RATS_IGNORED;               };
void STWINRATIG  (RAT* rat)                 { rat->state |=  CH_RATS_IGNORED;               };
int  GTWINRATACT (RAT* rat)                 { return rat->state & CH_RATS_ACTIVE;           };
void USTWINRATACT(RAT* rat)                 { rat->state &= ~CH_RATS_ACTIVE;                };
void STWINRATACT (RAT* rat)                 { rat->state |=  CH_RATS_ACTIVE;                };

//   ---     ---     ---     ---     ---