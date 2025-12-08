/*
 * Test for the BCD definitions
 */

#include "../UnitTest.h"

TEST_FUNC(Bcd)
{
    TEST_OK(IsEqualGUID(&GUID_BAD_MEMORY_GROUP, &_Inline_GUID_BAD_MEMORY_GROUP));
}
