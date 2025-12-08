/*
 * Test for the MSVC Specification
 */

#include "../UnitTest.h"

static volatile LONG g_lInit = 0;

static
MSVC_POST_CPP_USER_INITIALIZER(Init1)
{
    InterlockedAdd(&g_lInit, 234);
}

static
MSVC_POST_CPP_USER_INITIALIZER(Init2)
{
    InterlockedAdd(&g_lInit, 432);
}

TEST_FUNC(MSVC)
{
    /* Cpp initializer */
    TEST_OK(g_lInit == 666);

    /* Additional predefined macros by KNSoft/MSVC-Template */
#if !defined(_KNSOFT_) || !defined(MSBUILD_CONFIGURATION_TYPE_EXE)
#error
#endif
    TEST_OK(strcmp(MSBUILD_SOLUTION_NAME, "KNSoft.NDK") == 0);
    TEST_OK(strcmp(MSBUILD_PROJECT_NAME, "UnitTest") == 0);
#if defined(_DEBUG)
    TEST_OK(strcmp(MSBUILD_CONFIGURATION, "Debug") == 0);
#elif defined(NDEBUG)
    TEST_OK(strcmp(MSBUILD_CONFIGURATION, "Release") == 0);
#endif
#if defined(_M_IX86)
    TEST_OK(strcmp(MSBUILD_TARGET_PLATFORM, "x86") == 0);
#elif defined(_M_X64) && !defined(_M_ARM64EC)
    TEST_OK(strcmp(MSBUILD_TARGET_PLATFORM, "x64") == 0);
#elif defined(_M_ARM64)
    TEST_OK(strcmp(MSBUILD_TARGET_PLATFORM, "ARM64") == 0);
#elif defined(_M_ARM64EC)
    TEST_OK(strcmp(MSBUILD_TARGET_PLATFORM, "ARM64EC") == 0);
#endif
    UnitTest_PrintF("[MSBuild] Solution Name: %hs\n", MSBUILD_SOLUTION_NAME);
    UnitTest_PrintF("[MSBuild] Project Name: %hs\n", MSBUILD_PROJECT_NAME);
    UnitTest_PrintF("[MSBuild] Configuration: %hs\n", MSBUILD_CONFIGURATION);
    UnitTest_PrintF("[MSBuild] Platform: %hs\n", MSBUILD_PLATFORM);
    UnitTest_PrintF("[MSBuild] MSBuildVersion: %hs\n", MSBUILD_VERSION);
    UnitTest_PrintF("[MSBuild] VCToolsVersion: %hs\n", MSBUILD_VCTOOLS_VERSION);
    UnitTest_PrintF("[MSBuild] PlatformToolset: %hs\n", MSBUILD_PLATFORM_TOOLSET);
    UnitTest_PrintF("[MSBuild] PlatformToolsetVersion: %hs\n", MSBUILD_PLATFORM_TOOLSET_VERSION);
    UnitTest_PrintF("[MSBuild] TargetPlatform: %hs\n", MSBUILD_TARGET_PLATFORM);
    UnitTest_PrintF("[MSBuild] TargetPlatformVersion: %hs\n", MSBUILD_TARGET_PLATFORM_VERSION);
}

_STATIC_ASSERT(TRUE);
