/*
 * Test for the assembly stuff
 */

#include "UnitTest.h"

#if defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))
EXTERN_C LOGICAL CDECL AsmMacro(VOID);
#endif

TEST_FUNC(Assembly)
{
#if defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))
    TEST_OK(AsmMacro());
#endif
}
