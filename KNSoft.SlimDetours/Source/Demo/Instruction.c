/*
 * This demo compiled with platform specified assembly source to hook functions with customized instructions.
 * 
 * Run "Demo.exe -Run Instruction".
 */

#include "Demo.h"
#include "Instruction.inl"

typedef
_Function_class_(FN_INSTRUCTION)
ULONG_PTR
_cdecl
FN_INSTRUCTION(VOID);

#if defined(_M_X64) && !defined(_M_ARM64EC)

EXTERN_C FN_INSTRUCTION SimpleInstructionFunc1X64;

#elif defined(_M_IX86)

EXTERN_C FN_INSTRUCTION SimpleInstructionFunc1X86;

#endif

static
_Function_class_(FN_INSTRUCTION)
ULONG_PTR
_cdecl
Hooked_InstructionFunc(VOID)
{
    return (ULONG_PTR)PRESET_RETURN_VALUE * 2;
}

static FN_INSTRUCTION* g_apfnInstructionFunctions[] = {
#if defined(_M_X64) && !defined(_M_ARM64EC)
    &SimpleInstructionFunc1X64,
#elif defined(_M_IX86)
    &SimpleInstructionFunc1X86,
#endif
    NULL
};

TEST_FUNC(Instruction)
{
    HRESULT hr;
    FN_INSTRUCTION *pfn1, *pfn2;

    for (ULONG i = 0; i < ARRAYSIZE(g_apfnInstructionFunctions) && g_apfnInstructionFunctions[i] != NULL; i++)
    {
        pfn2 = pfn1 = g_apfnInstructionFunctions[i];
        if (pfn1() != (ULONG_PTR)PRESET_RETURN_VALUE)
        {
            TEST_SKIP("Instruction Function #%lu did not return the preset value\n", i);
            continue;
        }
        hr = SlimDetoursInlineHook(TRUE, (PVOID*)&pfn1, Hooked_InstructionFunc);
        TEST_OK(SUCCEEDED(hr));
        if (SUCCEEDED(hr))
        {
            TEST_OK(pfn1() == (ULONG_PTR)PRESET_RETURN_VALUE);
            TEST_OK(pfn2() == (ULONG_PTR)PRESET_RETURN_VALUE * 2);
        }
        hr = SlimDetoursInlineHook(FALSE, (PVOID*)&pfn1, Hooked_InstructionFunc);
        TEST_OK(SUCCEEDED(hr));
        if (SUCCEEDED(hr))
        {
            TEST_OK(pfn2() == (ULONG_PTR)PRESET_RETURN_VALUE);
        }
    }
}
