#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* wdm.h */

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
{
    StandardDesign,                 // None == 0 == standard design
    NEC98x86,                       // NEC PC98xx series on X86
    EndAlternatives                 // past end of known alternatives
} ALTERNATIVE_ARCHITECTURE_TYPE;

#define PROCESSOR_FEATURE_MAX 64

EXTERN_C_END
