/*
 * Test for the KNSoft.NDK.UnitTest unit test framework itself
 */

#include "UnitTest.h"

TEST_FUNC(UnitTestFramework)
{
    UNITTEST_RESULT Result;

    UnitTest_Print("The following tests verify the unit test framework itself, so fail and skip outputs are expected\n");

    /* Totally 5 pass, 6 fail, 3 skip */
    TEST_OK(TRUE);
    TEST_OK(1 == 1);
    TEST_OK(1 != 2);
    TEST_RESULT(Pass);
    TEST_RESULT(Pass);

    TEST_OK(FALSE);
    TEST_OK(1 != 1);
    TEST_OK(1 == 2);
    TEST_RESULT(Fail);
    TEST_FAIL("Fail");
    TEST_FAIL();

    TEST_SKIP("Skip");
    TEST_SKIP();
    TEST_RESULT(Skip);

    /* Move the result to local and verify */
    C_ASSERT(sizeof(Result) == sizeof(*TEST_PARAMETER_RESULT));
    RtlCopyMemory(&Result, TEST_PARAMETER_RESULT, sizeof(Result));
    RtlZeroMemory(TEST_PARAMETER_RESULT, sizeof(*TEST_PARAMETER_RESULT));

    TEST_OK(Result.Pass == 5);
    TEST_OK(Result.Fail == 6);
    TEST_OK(Result.Skip == 3);
}
