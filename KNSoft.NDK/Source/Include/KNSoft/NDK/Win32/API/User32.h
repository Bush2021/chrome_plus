#pragma once

#include "../../NT/MinDef.h"

#include <apisetcconv.h>

EXTERN_C_START

WINUSERAPI
INT
WINAPI
MessageBoxTimeoutA(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType,
    _In_ WORD wLanguageId,
    _In_ DWORD dwMilliseconds
    );

WINUSERAPI
INT
WINAPI
MessageBoxTimeoutW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType,
    _In_ WORD wLanguageId,
    _In_ DWORD dwMilliseconds
    );

WINUSERAPI
HWND
WINAPI
GetShellChangeNotifyWindow(VOID);

WINUSERAPI
HWND
WINAPI
GetTaskmanWindow(VOID);

WINUSERAPI
BOOL
WINAPI
IsTopLevelWindow(
    _In_ HWND hWnd
    );

WINUSERAPI
BOOL
WINAPI
IsServerSideWindow(
    _In_ HWND hWnd
    );

WINUSERAPI
BOOL
WINAPI
EndTask(
    HWND hWnd,
    BOOL fShutDown,
    BOOL fForce
    );

/* See also NtUserConsoleControl */
WINUSERAPI
NTSTATUS
NTAPI
ConsoleControl(
    _In_ CONSOLECONTROL Command,
    _In_reads_bytes_(ConsoleInformationLength) PVOID ConsoleInformation,
    _In_ ULONG ConsoleInformationLength
    );

/* See also NtUserQuerySendMessage */
WINUSERAPI
BOOL
NTAPI
QuerySendMessage(
    _Inout_ PMSG Message
    );

/* See also NtUserSetWindowStationUser */
WINUSERAPI
BOOL
NTAPI
SetWindowStationUser(
    _In_ HWINSTA WindowStationHandle,
    _In_ PLUID UserLogonId,
    _In_ PSID UserSid,
    _In_ ULONG UserSidLength
    );

/* See also NtUserGhostWindowFromHungWindow */
WINUSERAPI
HWND
NTAPI
GhostWindowFromHungWindow(
    _In_ HWND WindowHandle
    );

/* See also NtUserHungWindowFromGhostWindow */
WINUSERAPI
HWND
NTAPI
HungWindowFromGhostWindow(
    _In_ HWND WindowHandle
    );

/* See also NtUserGetSendMessageReceiver */
WINUSERAPI
HWND
NTAPI
GetSendMessageReceiver(
    _In_ HANDLE ThreadId
    );

/* User32.dll ordinal 2005, see also: NtUserSetChildWindowNoActivate */
WINUSERAPI
BOOL
NTAPI
SetChildWindowNoActivate(
    _In_ HWND WindowHandle
    );

EXTERN_C_END
