#include "zjc_id.h"

//   ---     ---     ---     ---     ---

ID IDNEW(char* type, char* key)            {

    ID id={0}; int i=0;

    do {
        id.type[i]=*type; i++;
        if(i==3) { break; }

    } while(*type++);

    while(i<4) { id.type[i]='_'; i++; }; i=0;

    do {
        id.key[i]=*key; i++;
        if(i==19) { id.key[19]='\0'; break; }

    } while(*key++);

    if(i!=19) { id.key[i]='\0'; };

    id.x = -1;
    id.y = -1;

    return id;

};

//   ---     ---     ---     ---     ---