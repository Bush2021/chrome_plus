using System;
using System.Collections.Generic;
using System.IO;

using KNSoft.C4Lib.CodeHelper;

namespace KNSoft.NDK.SDK;

public class SyscallResolver
{
    public enum SyscallType
    {
        Nt = 0,
        NtUser = 1,
        Zw = 2,
        ZwManual = 3
    };

    /* Have no Zw version exported in ntdll.dll */
    static public readonly String[] UserModeImplSyscalls = ["NtGetTickCount"];

    static public List<Cpp.Function> GetSyscallsFromFile(String FilePath)
    {
        String[] Content = File.ReadAllLines(FilePath);
        List<Cpp.Function> Functions = [];

        for (Int32 i = 0; i < Content.Length; i++)
        {
            if (Content[i].Trim() != "NTSYSCALLAPI")
            {
                continue;
            }
            Int32 iStart;
            for (iStart = i - 1; iStart >= 0; iStart--)
            {
                if (Cpp.CodeResolver.IsFunctionDeclarationStart(Content[iStart]))
                {
                    break;
                }
            }
            if (iStart < 0)
            {
                continue;
            }
            do
            {
                if (Cpp.CodeResolver.IsFunctionDeclarationEnd(Content[i]))
                {
                    break;
                }
            } while (++i < Content.Length);
            if (i < Content.Length)
            {
                Functions.AddRange(Cpp.CodeResolver.GetFunctionsFromContent(Content[(iStart + 1)..(i + 1)]));
            }
        }

        return Functions;
    }

    static public List<Cpp.Function> GetSyscalls(String NtDir /* KNSoft.NDK\Source\Include\KNSoft\NDK\NT */, SyscallType Type)
    {
        if (Type == SyscallType.Nt)
        {
            List<Cpp.Function> Syscalls = [];

            String[] Headers = Directory.GetFiles(NtDir, @"*.h", SearchOption.AllDirectories);
            foreach (String Header in Headers)
            {
                if (Header.StartsWith(NtDir + @"\Rtl\") ||
                    Header.StartsWith(NtDir + @"\Win32K\") ||
                    Header.StartsWith(NtDir + @"\Extension\") ||
                    Header == NtDir + @"\Win32K\Win32KApi.h" ||
                    Header == NtDir + @"\ZwApi.h" ||
                    Header == NtDir + @"\ZwApi.Manual.h")
                {
                    continue;
                }
                Syscalls.AddRange(GetSyscallsFromFile(Header));
            }

            return Syscalls;
        } else if (Type == SyscallType.NtUser)
        {
            return GetSyscallsFromFile(NtDir + @"\Win32K\Win32KApi.h");
        } else if (Type == SyscallType.Zw)
        {
            return GetSyscallsFromFile(NtDir + @"\ZwApi.h");
        } else if (Type == SyscallType.ZwManual)
        {
            return GetSyscallsFromFile(NtDir + @"\ZwApi.Manual.h");
        }

        return [];
    }
}
