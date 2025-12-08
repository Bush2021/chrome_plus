#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

/**
 * The NtQuerySystemTime routine obtains the current system time.
 *
 * @param SystemTime A pointer to a LARGE_INTEGER structure that receives the system time. This is a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysystemtime
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemTime(
    _Out_ PLARGE_INTEGER SystemTime);

/**
 * The NtSetSystemTime routine sets the current system time and date. The system time is expressed in Coordinated Universal Time (UTC).
 *
 * @param SystemTime A pointer to a LARGE_INTEGER structure that that contains the new system date and time.
 * @param PreviousTime A pointer to a LARGE_INTEGER structure that that contains the previous system time.
 * @return NTSTATUS Successful or errant status.
 * @remarks The calling process must have the SE_SYSTEMTIME_NAME privilege.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-setsystemtime
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemTime(
    _In_opt_ PLARGE_INTEGER SystemTime,
    _Out_opt_ PLARGE_INTEGER PreviousTime);

/**
 * The NtQueryTimerResolution routine retrieves the range and current value of the system interrupt timer.
 *
 * @param MaximumTime The maximum timer resolution, in 100-nanosecond units.
 * @param MinimumTime The minimum timer resolution, in 100-nanosecond units.
 * @param CurrentTime The current timer resolution, in 100-nanosecond units.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryTimerResolution(
    _Out_ PULONG MaximumTime,
    _Out_ PULONG MinimumTime,
    _Out_ PULONG CurrentTime);

/**
 * The NtSetTimerResolution routine sets the system interrupt timer resolution to the specified value.
 *
 * @param DesiredTime The desired timer resolution, in 100-nanosecond units.
 * @param SetResolution If TRUE, the timer resolution is set to the value specified by DesiredTime. If FALSE, the timer resolution is reset to the default value.
 * @param ActualTime The actual timer resolution, in 100-nanosecond units.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimerResolution(
    _In_ ULONG DesiredTime,
    _In_ BOOLEAN SetResolution,
    _Out_ PULONG ActualTime);

/**
 * The NtQueryPerformanceCounter routine retrieves the current value of the performance counter,
 * which is a high resolution (<1us) time stamp that can be used for time-interval measurements.
 *
 * @param PerformanceCounter A pointer to a variable that receives the current performance-counter value, in 100-nanosecond units.
 * @param PerformanceFrequency A pointer to a variable that receives the current performance-frequency value, in 100-nanosecond units.
 * @return NTSTATUS Successful or errant status.
 * @remarks On systems that run Windows XP or later, the function will always succeed and will thus never return zero. Use RtlQueryPerformanceCounter instead since no system calls are required.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryPerformanceCounter(
    _Out_ PLARGE_INTEGER PerformanceCounter,
    _Out_opt_ PLARGE_INTEGER PerformanceFrequency);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryAuxiliaryCounterFrequency(
    _Out_ PULONG64 AuxiliaryCounterFrequency);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtConvertBetweenAuxiliaryCounterAndPerformanceCounter(
    _In_ BOOLEAN ConvertAuxiliaryToPerformanceCounter,
    _In_ PULONG64 PerformanceOrAuxiliaryCounterValue,
    _Out_ PULONG64 ConvertedValue,
    _Out_opt_ PULONG64 ConversionError);

#endif

NTSYSCALLAPI
ULONG
NtGetTickCount(VOID);

EXTERN_C_END
