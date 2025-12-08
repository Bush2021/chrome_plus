#pragma once

#include <KNSoft/NDK/NDK.h>
#include <KNSoft/NDK/NDK.inl>
#include <KNSoft/NDK/Package/UnitTest.h>
#include <KNSoft/NDK/Win32/API/Ntdll.Hash.h>

/* For ARM64EC, link x64 DLL import libraries */
#if defined(_M_ARM64EC)
#define LIB_DIR "../x64/"
#else
#define LIB_DIR
#endif

#pragma comment(lib, LIB_DIR"KNSoft.NDK.Ntdll.Hash.lib")
#pragma comment(lib, LIB_DIR"KNSoft.NDK.WinAPI.lib")
