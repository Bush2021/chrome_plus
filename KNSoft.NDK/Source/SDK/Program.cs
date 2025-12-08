/* Usage: SDK.exe (KNSoft.NDK\Source Path) */

using System;
using System.Diagnostics;
using System.IO;
using System.Collections.Generic;
using System.Linq;

using KNSoft.C4Lib.CodeHelper;

using KNSoft.NDK.SDK;

/* Pass the test, make sure KNSoft.C4Lib.CodeHelper.Cpp works well for us */
TestCodeHelper();

/* Get Syscalls */
String NtDir = args[0] + @"\Include\KNSoft\NDK\NT";
List<Cpp.Function> NtSyscalls = [.. SyscallResolver.GetSyscalls(NtDir, SyscallResolver.SyscallType.Nt).OrderBy(x => x.Name)];
List<Cpp.Function> ZwSyscalls = SyscallResolver.GetSyscalls(NtDir, SyscallResolver.SyscallType.Zw);
List<Cpp.Function> ZwManualSyscalls = SyscallResolver.GetSyscalls(NtDir, SyscallResolver.SyscallType.ZwManual);

List<Cpp.Function> Diff = [.. NtSyscalls.Where(x => ZwSyscalls.Find(y => y.Name[2..] == x.Name[2..]) == null &&
                                                    ZwManualSyscalls.Find(y => y.Name[2..] == x.Name[2..]) == null &&
                                                    Array.Find(SyscallResolver.UserModeImplSyscalls, y => y == x.Name) == null)];

/* Report warning if some Nt* have no corresponding Zw* */
if (Diff.Count > 0)
{
    Console.WriteLine(@"Warning: the following Nt* have no corresponding Zw*:");
    foreach (Cpp.Function Func in Diff)
    {
        Console.WriteLine("    " + Func.Name);
    }
}

/* Generate ZwApi.h */
StreamWriter ZwApiHeader = Cpp.CreateOutputFile(NtDir + @"\ZwApi.h");
String ZwApiStart = """
#include "NT.h"

#include "ZwApi.Manual.h"
""";
Cpp.OutputWithNewLine(ZwApiHeader, Cpp.CodeFragment.AutoGenerateFileComment);
Cpp.OutputWithNewLine(ZwApiHeader, Cpp.CodeFragment.PragmaOnce);
Cpp.OutputWithNewLine(ZwApiHeader, ZwApiStart);
Cpp.OutputWithNewLine(ZwApiHeader, Cpp.CodeFragment.ExternCStart);
for (Int32 i = 0; i < NtSyscalls.Count; i++)
{
    if (ZwManualSyscalls.Find(x => x.Name[2..] == NtSyscalls[i].Name[2..]) == null &&
        Array.Find(SyscallResolver.UserModeImplSyscalls, x => x == NtSyscalls[i].Name) == null)
    {
        NtSyscalls[i].Name = "Zw" + NtSyscalls[i].Name[2..];
        foreach (String Line in Cpp.CodeResolver.FunctionToDeclaration(NtSyscalls[i]))
        {
            ZwApiHeader.WriteLine(Line);
        }
        ZwApiHeader.WriteLine();
    }
}
ZwApiHeader.WriteLine(Cpp.CodeFragment.ExternCEnd);
ZwApiHeader.Dispose();

return 0;

/* ========== Tests ========== */

static void TestCodeHelper()
{
    List<Cpp.Function>? Functions;

    /* A standard syscall API in NDK */

    String[] C1T = """

NTSYSCALLAPI
NTSTATUS
NTAPI
ZwAcquireCrossVmMutant(
    _In_ HANDLE CrossVmMutant,
    _In_ PLARGE_INTEGER Timeout
    );
""".Split("\r\n");

    Functions = Cpp.CodeResolver.GetFunctionsFromContent(C1T);
    Debug.Assert(Functions.Count == 1);
    Debug.Assert(Functions[0].Prefixes.Count == 3);
    Debug.Assert(Functions[0].Prefixes[0] == "NTSYSCALLAPI");
    Debug.Assert(Functions[0].Prefixes[1] == "NTSTATUS");
    Debug.Assert(Functions[0].Prefixes[2] == "NTAPI");
    Debug.Assert(Functions[0].Name == "ZwAcquireCrossVmMutant");
    Debug.Assert(Functions[0].Parameters.Count == 2);

    Debug.Assert(Functions[0].Parameters[0].Prefixes.Count == 1);
    Debug.Assert(Functions[0].Parameters[0].Prefixes[0] == "_In_");
    Debug.Assert(Functions[0].Parameters[0].Type == "HANDLE");
    Debug.Assert(Functions[0].Parameters[0].Name == "CrossVmMutant");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[0].LineComment));

    Debug.Assert(Functions[0].Parameters[1].Prefixes.Count == 1);
    Debug.Assert(Functions[0].Parameters[1].Prefixes[0] == "_In_");
    Debug.Assert(Functions[0].Parameters[1].Type == "PLARGE_INTEGER");
    Debug.Assert(Functions[0].Parameters[1].Name == "Timeout");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[1].LineComment));

    /* Complex SAL */

    String[] C2T = """

_When_(Status < 0, _Out_range_(>, 0))
_When_(Status >= 0, _Out_range_(==, 0))
ULONG
C2TFunc(
    _In_ _Post_ptr_invalid_ _When_(XXX, != 0) ULONG Status);
""".Split("\r\n");

    Functions = Cpp.CodeResolver.GetFunctionsFromContent(C2T);
    Debug.Assert(Functions.Count == 1);
    Debug.Assert(Functions[0].Prefixes.Count == 3);
    Debug.Assert(Functions[0].Prefixes[0] == "_When_(Status < 0, _Out_range_(>, 0))");
    Debug.Assert(Functions[0].Prefixes[1] == "_When_(Status >= 0, _Out_range_(==, 0))");
    Debug.Assert(Functions[0].Prefixes[2] == "ULONG");
    Debug.Assert(Functions[0].Name == "C2TFunc");
    Debug.Assert(Functions[0].Parameters.Count == 1);

    Debug.Assert(Functions[0].Parameters[0].Prefixes.Count == 3);
    Debug.Assert(Functions[0].Parameters[0].Prefixes[0] == "_In_");
    Debug.Assert(Functions[0].Parameters[0].Prefixes[1] == "_Post_ptr_invalid_");
    Debug.Assert(Functions[0].Parameters[0].Prefixes[2] == "_When_(XXX, != 0)");
    Debug.Assert(Functions[0].Parameters[0].Type == "ULONG");
    Debug.Assert(Functions[0].Parameters[0].Name == "Status");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[0].LineComment));

    /* With comments */

    String[] C3T = """

ULONG
C3TFunc(
    /* Comment1 */ _In_ _Post_ptr_invalid_ _When_(XXX, != 0) ULONG Status, // Comment2 // Comment3
    INT p2 // xxx
)
;
""".Split("\r\n");

    Functions = Cpp.CodeResolver.GetFunctionsFromContent(C3T);
    Debug.Assert(Functions.Count == 1);
    Debug.Assert(Functions[0].Prefixes.Count == 1);
    Debug.Assert(Functions[0].Prefixes[0] == "ULONG");
    Debug.Assert(Functions[0].Name == "C3TFunc");
    Debug.Assert(Functions[0].Parameters.Count == 2);

    Debug.Assert(Functions[0].Parameters[0].Prefixes.Count == 3);
    Debug.Assert(Functions[0].Parameters[0].Prefixes[0] == "_In_");
    Debug.Assert(Functions[0].Parameters[0].Prefixes[1] == "_Post_ptr_invalid_");
    Debug.Assert(Functions[0].Parameters[0].Prefixes[2] == "_When_(XXX, != 0)");
    Debug.Assert(Functions[0].Parameters[0].Type == "ULONG");
    Debug.Assert(Functions[0].Parameters[0].Name == "Status");
    Debug.Assert(Functions[0].Parameters[0].Comments.Count == 1);
    Debug.Assert(Functions[0].Parameters[0].Comments[0] == "/* Comment1 */");
    Debug.Assert(Functions[0].Parameters[0].LineComment == "// Comment2 // Comment3");

    Debug.Assert(Functions[0].Parameters[1].Prefixes.Count == 0);
    Debug.Assert(Functions[0].Parameters[1].Type == "INT");
    Debug.Assert(Functions[0].Parameters[1].Name == "p2");
    Debug.Assert(Functions[0].Parameters[1].LineComment == "// xxx");

    /* VOID parameter */

    String[] C4T = """
void
C4TFunc(
    VOID);
""".Split("\r\n");

    Functions = Cpp.CodeResolver.GetFunctionsFromContent(C4T);
    Debug.Assert(Functions.Count == 1);
    Debug.Assert(Functions[0].Prefixes.Count == 1);
    Debug.Assert(Functions[0].Prefixes[0] == "void");
    Debug.Assert(Functions[0].Name == "C4TFunc");
    Debug.Assert(Functions[0].Parameters.Count == 1);

    Debug.Assert(Functions[0].Parameters[0].Prefixes.Count == 0);
    Debug.Assert(Functions[0].Parameters[0].Type == "VOID");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[0].Name));
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[0].LineComment));

    /* A single line, a parameter comment in tail, parameter type has "*" */

    String[] C5T = """
int C5TFunc(int * p1, /* comment */ int p2, int* /* No name */, int *);
""".Split("\r\n");

    Functions = Cpp.CodeResolver.GetFunctionsFromContent(C5T);
    Debug.Assert(Functions.Count == 1);
    Debug.Assert(Functions[0].Prefixes.Count == 1);
    Debug.Assert(Functions[0].Prefixes[0] == "int");
    Debug.Assert(Functions[0].Name == "C5TFunc");
    Debug.Assert(Functions[0].Parameters.Count == 4);

    Debug.Assert(Functions[0].Parameters[0].Prefixes.Count == 0);
    Debug.Assert(Functions[0].Parameters[0].Type == "int*");
    Debug.Assert(Functions[0].Parameters[0].Name == "p1");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[0].LineComment));

    Debug.Assert(Functions[0].Parameters[1].Prefixes.Count == 0);
    Debug.Assert(Functions[0].Parameters[1].Type == "int");
    Debug.Assert(Functions[0].Parameters[1].Name == "p2");
    Debug.Assert(Functions[0].Parameters[1].Comments.Count == 1);
    Debug.Assert(Functions[0].Parameters[1].Comments[0] == "/* comment */");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[1].LineComment));

    Debug.Assert(Functions[0].Parameters[2].Prefixes.Count == 0);
    Debug.Assert(Functions[0].Parameters[2].Type == "int*");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[2].Name));
    Debug.Assert(Functions[0].Parameters[2].Comments.Count == 1);
    Debug.Assert(Functions[0].Parameters[2].Comments[0] == "/* No name */");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[2].LineComment));

    Debug.Assert(Functions[0].Parameters[3].Prefixes.Count == 0);
    Debug.Assert(Functions[0].Parameters[3].Type == "int*");
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[3].Name));
    Debug.Assert(Functions[0].Parameters[3].Comments.Count == 0);
    Debug.Assert(String.IsNullOrEmpty(Functions[0].Parameters[3].LineComment));
}
