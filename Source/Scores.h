#ifndef __SCORES__
#define __SCORES__

#include "Defines.h"
#include "Types.h"

struct ScoreType
{
    uint ClientId;
    char ClientName[SCORE_NAME_LEN];
    int  Value;
};

#endif // __SCORES__ //
