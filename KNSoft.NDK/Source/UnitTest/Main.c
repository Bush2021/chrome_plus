#include <KNSoft/NDK/NDK.h>
#include <KNSoft/NDK/Package/UnitTest.inl>

TEST_DECL_FUNC(UnitTestFramework);
TEST_DECL_FUNC(MSVC);

TEST_DECL_FUNC(Assembly);
TEST_DECL_FUNC(Bcd);
TEST_DECL_FUNC(KUSDStruct);
TEST_DECL_FUNC(PebLdrStruct);
TEST_DECL_FUNC(PebStruct);
TEST_DECL_FUNC(RandGen);
TEST_DECL_FUNC(StrSafeFunc);
TEST_DECL_FUNC(TebStruct);

CONST UNITTEST_ENTRY UnitTestList[] = {
    TEST_DECL_ENTRY(UnitTestFramework),
    TEST_DECL_ENTRY(MSVC),

    TEST_DECL_ENTRY(Assembly),
    TEST_DECL_ENTRY(Bcd),
    TEST_DECL_ENTRY(KUSDStruct),
    TEST_DECL_ENTRY(PebLdrStruct),
    TEST_DECL_ENTRY(PebStruct),
    TEST_DECL_ENTRY(RandGen),
    TEST_DECL_ENTRY(StrSafeFunc),
    TEST_DECL_ENTRY(TebStruct),
    { 0 }
};

ULONG flagcounts[32] = { 0 };

int
_cdecl
wmain(
    _In_ int argc,
    _In_reads_(argc) _Pre_z_ wchar_t** argv)
{
    return UnitTest_Main(argc, argv);
}
