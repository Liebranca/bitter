#include "ZJC_CommonTypes.h"
#include <stdlib.h>

long hexstr_tolong (char* v)                { return strtol(v, NULL, 0);                    }
long chexstr_tolong(cchar* v)               { return strtol(v, NULL, 0);                    }
