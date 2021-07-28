#include "zjc_id.h"

//   ---     ---     ---     ---     ---

ID IDNEW(char* type, char* key)            {

    ID id={0}; int i=0;
    uint klimit=KVR_IDK_WIDTH-1;

    do {
        id.type[i]=*type; i++;
        if(i==4) { break; }

    } while(*type++); i=0;

    do {
        id.key[i]=*key; i++;
        if(i==klimit) { id.key[klimit]='\0'; break; }

    } while(*key++);

    if(i!=klimit) { id.key[i]='\0'; };

    id.x = -1;
    id.y = -1;

    return id;

};

//   ---     ---     ---     ---     ---