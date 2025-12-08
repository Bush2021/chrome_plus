/* Minimum Windows NT definition */

#pragma once

#include <SdkDdkVer.h>

/* Patch undocumented NTDDI versions */
#define NTDDI_WIN10_GE NTDDI_WIN11_ZN /* Introduced in Windows SDK 10.0.26100.1 (Win11), later than all NTDDI_WIN10_* */
#define NTDDI_WIN11_DT NTDDI_WIN11_GE /* Introduced in Windows SDK 10.0.26100.3916, later than NTDDI_WIN11_GE */

/* Minimum support NT6.0 by default */
#ifndef _KNSOFT_NDK_NTDDI_MIN
#define _KNSOFT_NDK_NTDDI_MIN NTDDI_VISTA
#endif

// MS-Spec: Nonstandard extension used: zero-sized array in struct/union
#pragma warning(disable: 4200)

#if defined(_DEBUG) && !defined(DBG)
#define DBG 1
#endif

#pragma region Define architecture

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_IX86)
#define _X86_
#if !defined(_CHPE_X86_ARM64_) && defined(_M_HYBRID)
#define _CHPE_X86_ARM64_
#endif
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && (defined(_M_AMD64) || defined(_M_ARM64EC))
#define _AMD64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_ARM)
#define _ARM_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_ARM64)
#define _ARM64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_ARM64EC)
#define _ARM64EC_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_M68K)
#define _68K_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_MPPC)
#define _MPPC_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_M_IX86) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && !defined(_ARM64EC_) && defined(_M_IA64)
#if !defined(_IA64_)
#define _IA64_
#endif /* !_IA64_ */
#endif

#ifndef _MAC
#if defined(_68K_) || defined(_MPPC_)
#define _MAC
#endif
#endif

#pragma endregion Windows.h

#pragma region Include windef.h, ntdef.h, guiddef.h, wingdi.h, and winioctl.h

/* windef.h */

#ifndef _WINNT_

#define WIN32_NO_STATUS

#define PSID MS_PSID
#define _JOBOBJECTINFOCLASS _MS_JOBOBJECTINFOCLASS
#define JOBOBJECTINFOCLASS MS_JOBOBJECTINFOCLASS
#define JobObjectBasicAccountingInformation MS_JobObjectBasicAccountingInformation
#define JobObjectBasicLimitInformation MS_JobObjectBasicLimitInformation
#define JobObjectBasicProcessIdList MS_JobObjectBasicProcessIdList
#define JobObjectBasicUIRestrictions MS_JobObjectBasicUIRestrictions
#define JobObjectSecurityLimitInformation MS_JobObjectSecurityLimitInformation
#define JobObjectEndOfJobTimeInformation MS_JobObjectEndOfJobTimeInformation
#define JobObjectAssociateCompletionPortInformation MS_JobObjectAssociateCompletionPortInformation
#define JobObjectBasicAndIoAccountingInformation MS_JobObjectBasicAndIoAccountingInformation
#define JobObjectExtendedLimitInformation MS_JobObjectExtendedLimitInformation
#define JobObjectJobSetInformation MS_JobObjectJobSetInformation
#define JobObjectGroupInformation MS_JobObjectGroupInformation
#define JobObjectNotificationLimitInformation MS_JobObjectNotificationLimitInformation
#define JobObjectLimitViolationInformation MS_JobObjectLimitViolationInformation
#define JobObjectGroupInformationEx MS_JobObjectGroupInformationEx
#define JobObjectCpuRateControlInformation MS_JobObjectCpuRateControlInformation
#define JobObjectCompletionFilter MS_JobObjectCompletionFilter
#define JobObjectCompletionCounter MS_JobObjectCompletionCounter
#define JobObjectNetRateControlInformation MS_JobObjectNetRateControlInformation
#define JobObjectNotificationLimitInformation2 MS_JobObjectNotificationLimitInformation2
#define JobObjectLimitViolationInformation2 MS_JobObjectLimitViolationInformation2
#define JobObjectCreateSilo MS_JobObjectCreateSilo
#define JobObjectSiloBasicInformation MS_JobObjectSiloBasicInformation
#define JobObjectNetworkAccountingInformation MS_JobObjectNetworkAccountingInformation
#define MaxJobObjectInfoClass MS_MaxJobObjectInfoClass
#define _HEAP_INFORMATION_CLASS _MS_HEAP_INFORMATION_CLASS
#define HEAP_INFORMATION_CLASS MS_HEAP_INFORMATION_CLASS
#define HeapCompatibilityInformation MS_HeapCompatibilityInformation
#define HeapEnableTerminationOnCorruption MS_HeapEnableTerminationOnCorruption
#define HeapOptimizeResources MS_HeapOptimizeResources
#define HeapTag MS_HeapTag

#endif

#include <windef.h>

#ifdef _WINNT_

#undef WIN32_NO_STATUS

#undef PSID
typedef SID* PSID;
typedef const SID* PCSID;
#undef _JOBOBJECTINFOCLASS
#undef JOBOBJECTINFOCLASS
#undef JobObjectBasicAccountingInformation
#undef JobObjectBasicLimitInformation
#undef JobObjectBasicProcessIdList
#undef JobObjectBasicUIRestrictions
#undef JobObjectSecurityLimitInformation
#undef JobObjectEndOfJobTimeInformation
#undef JobObjectAssociateCompletionPortInformation
#undef JobObjectBasicAndIoAccountingInformation
#undef JobObjectExtendedLimitInformation
#undef JobObjectJobSetInformation
#undef JobObjectGroupInformation
#undef JobObjectNotificationLimitInformation
#undef JobObjectLimitViolationInformation
#undef JobObjectGroupInformationEx
#undef JobObjectCpuRateControlInformation
#undef JobObjectCompletionFilter
#undef JobObjectCompletionCounter
#undef JobObjectNetRateControlInformation
#undef JobObjectNotificationLimitInformation2
#undef JobObjectLimitViolationInformation2
#undef JobObjectCreateSilo
#undef JobObjectSiloBasicInformation
#undef JobObjectNetworkAccountingInformation
#undef MaxJobObjectInfoClass
#undef _HEAP_INFORMATION_CLASS
#undef HEAP_INFORMATION_CLASS
#undef HeapCompatibilityInformation
#undef HeapEnableTerminationOnCorruption
#undef HeapOptimizeResources
#undef HeapTag

#endif

/* ntdef.h */

#ifndef _NTDEF_

#undef LANGIDFROMLCID
#undef MAKELANGID
#undef MAKELCID
#undef MAKESORTLCID
#undef MAX_NATURAL_ALIGNMENT
#undef PRIMARYLANGID
#undef SUBLANGID
#undef PROBE_ALIGNMENT
#undef PROBE_ALIGNMENT32
#undef SORTIDFROMLCID
#undef SORTVERSIONFROMLCID
#undef UNICODE_STRING_MAX_BYTES

#define _LIST_ENTRY _MS_LIST_ENTRY
#define LIST_ENTRY MS_LIST_ENTRY
#define PLIST_ENTRY PMS_LIST_ENTRY
#define PRLIST_ENTRY PRMS_LIST_ENTRY
#define LIST_ENTRY32 MS_LIST_ENTRY32
#define PLIST_ENTRY32 PMS_LIST_ENTRY32
#define LIST_ENTRY64 MS_LIST_ENTRY64
#define PLIST_ENTRY64 PMS_LIST_ENTRY64
#define _SINGLE_LIST_ENTRY _MS_SINGLE_LIST_ENTRY
#define SINGLE_LIST_ENTRY MS_SINGLE_LIST_ENTRY
#define PSINGLE_LIST_ENTRY PMS_SINGLE_LIST_ENTRY
#define _LARGE_INTEGER _MS_LARGE_INTEGER
#define LARGE_INTEGER MS_LARGE_INTEGER
#define PLARGE_INTEGER PMS_LARGE_INTEGER
#define _ULARGE_INTEGER _MS_ULARGE_INTEGER
#define ULARGE_INTEGER MS_ULARGE_INTEGER
#define PULARGE_INTEGER PMS_ULARGE_INTEGER
#define _LUID _MS_LUID
#define LUID MS_LUID
#define PLUID PMS_LUID
#define _FLOAT128 _MS_FLOAT128
#define FLOAT128 MS_FLOAT128
#define PFLOAT128 PMS_FLOAT128
#define _PROCESSOR_NUMBER _MS_PROCESSOR_NUMBER
#define PROCESSOR_NUMBER MS_PROCESSOR_NUMBER
#define PPROCESSOR_NUMBER PMS_PROCESSOR_NUMBER
#define _GROUP_AFFINITY _MS_GROUP_AFFINITY
#define GROUP_AFFINITY MS_GROUP_AFFINITY
#define PGROUP_AFFINITY PMS_GROUP_AFFINITY
#define _GROUP_AFFINITY32 _MS_GROUP_AFFINITY32
#define GROUP_AFFINITY32 MS_GROUP_AFFINITY32
#define PGROUP_AFFINITY32 PMS_GROUP_AFFINITY32
#define _GROUP_AFFINITY64 _MS_GROUP_AFFINITY64
#define GROUP_AFFINITY64 MS_GROUP_AFFINITY64
#define PGROUP_AFFINITY64 PMS_GROUP_AFFINITY64

#if defined(_M_IX86)
#define Int64ShllMod32 MS_Int64ShllMod32
#define Int64ShraMod32 MS_Int64ShraMod32
#define Int64ShrlMod32 MS_Int64ShrlMod32
#endif

#define _ENUM_FLAG_SIZED_INTEGER _MS_ENUM_FLAG_SIZED_INTEGER
#define _ENUM_FLAG_INTEGER_FOR_SIZE _MS_ENUM_FLAG_INTEGER_FOR_SIZE

#endif

#include <ntdef.h>

#ifdef _NTDEF_

#undef _ENUM_FLAG_SIZED_INTEGER
#undef _ENUM_FLAG_INTEGER_FOR_SIZE

#undef Int64ShllMod32
#undef Int64ShraMod32
#undef Int64ShrlMod32

#undef _LIST_ENTRY
#undef LIST_ENTRY
#undef PLIST_ENTRY
#undef PRLIST_ENTRY
#undef LIST_ENTRY32
#undef PLIST_ENTRY32
#undef LIST_ENTRY64
#undef PLIST_ENTRY64
#undef _SINGLE_LIST_ENTRY
#undef SINGLE_LIST_ENTRY
#undef PSINGLE_LIST_ENTRY

#undef _LARGE_INTEGER
#undef LARGE_INTEGER
#undef PLARGE_INTEGER
#undef _ULARGE_INTEGER
#undef ULARGE_INTEGER
#undef PULARGE_INTEGER
#undef _LUID
#undef LUID
#undef PLUID
#undef _FLOAT128
#undef FLOAT128
#undef PFLOAT128
#undef _PROCESSOR_NUMBER
#undef PROCESSOR_NUMBER
#undef PPROCESSOR_NUMBER
#undef _GROUP_AFFINITY
#undef GROUP_AFFINITY
#undef PGROUP_AFFINITY
#undef _GROUP_AFFINITY32
#undef GROUP_AFFINITY32
#undef PGROUP_AFFINITY32
#undef _GROUP_AFFINITY64
#undef GROUP_AFFINITY64
#undef PGROUP_AFFINITY64

#endif

#include <wingdi.h>
#include <guiddef.h>
#include <winioctl.h>

#pragma endregion

#include <ntstatus.h>

#pragma region Addendum

#pragma region NtStatus

#define NT_CUSTOMER_SHIFT 29
#define NT_CUSTOMER(Status) ((((ULONG)(Status)) >> NT_CUSTOMER_SHIFT) & 1)

#define NT_SEVERITY_MASK 3
#define NT_SEVERITY_SHIFT 30
#define NT_SEVERITY(Status) ((((ULONG)(Status)) >> NT_SEVERITY_SHIFT) & NT_SEVERITY_MASK)

#define NT_FACILITY_MASK 0xfff
#define NT_FACILITY_SHIFT 16
#define NT_FACILITY(Status) ((((ULONG)(Status)) >> NT_FACILITY_SHIFT) & NT_FACILITY_MASK)

#define NT_CODE_MASK 0xffff
#define NT_CODE(Status) (((ULONG)(Status)) & NT_CODE_MASK)

#pragma endregion

#pragma region Basic Types

typedef unsigned __int64 QWORD, near* PQWORD, far* LPQWORD;
typedef DOUBLE *PDOUBLE;
typedef void* POINTER_32 PVOID32;

#define MAKEDWORD(l, h) ((DWORD)(((WORD)(((DWORD_PTR)(l)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(h)) & 0xffff))) << 16))
#define MAKEQWORD(l, h) ((QWORD)(((DWORD)(((DWORD_PTR)(l)) & 0xffffffff)) | ((QWORD)((DWORD)(((DWORD_PTR)(h)) & 0xffffffff))) << 32))
#define LODWORD(l) ((DWORD)(((QWORD)(l)) & 0xffffffff))
#define HIDWORD(l) ((DWORD)((((QWORD)(l)) >> 32) & 0xffffffff))

#define MAXUCHAR    0xff
#define MAXUSHORT   0xffff
#define MAXULONG    0xffffffff
#define MAXQWORD    0xffffffffffffffff

#define ANSI_STRING_MAX_BYTES ((USHORT)65535)
#define ANSI_STRING_MAX_CHARS (65535)

#pragma endregion

#pragma region Microsoft Specific

#define DECLSPEC_ALLOCATOR __declspec(allocator)
#define DECLSPEC_EXPORT __declspec(dllexport)
#define DECLSPEC_NOALIAS __declspec(noalias)

#if defined(_WIN64)
#define DECLSPEC_POINTERALIGN DECLSPEC_ALIGN(8)
#else
#define DECLSPEC_POINTERALIGN DECLSPEC_ALIGN(4)
#endif

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#if defined(__cplusplus)
#define typeof decltype
#elif __STDC_VERSION__ < 202311L && _MSC_FULL_VER >= 193933428
#define typeof __typeof__
#endif

#if __STDC_VERSION__ >= 202311L
#ifndef __cplusplus
#define nullptr ((void *)0)
#endif
typedef typeof(nullptr) nullptr_t;
#endif

#define FIELD_TYPE(type, field) typeof(((type*)NULL)->field)

/* Patch _STATIC_ASSERT to avoid confusion amount static_assert, _Static_assert, _STATIC_ASSERT and C_ASSERT */
#undef _STATIC_ASSERT
#ifdef __cplusplus
#define _STATIC_ASSERT(expr) static_assert((expr), #expr)
#else
#if __STDC_VERSION__ >= 201112L
#define _STATIC_ASSERT(expr) _Static_assert((expr), #expr)
#else
#define _STATIC_ASSERT C_ASSERT
#endif
#endif

#pragma endregion

#pragma region Types

/* KM types */
typedef struct _FILE_OBJECT *PFILE_OBJECT;
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT;
typedef struct _IRP *PIRP;

/* GUID */
typedef GUID *PGUID;
typedef const GUID* PCGUID;

/* WNF, ntdef.h */
#ifndef _DEFINED__WNF_STATE_NAME
#define _DEFINED__WNF_STATE_NAME
typedef struct _WNF_STATE_NAME
{
    ULONG Data[2];
} WNF_STATE_NAME;
typedef struct _WNF_STATE_NAME* PWNF_STATE_NAME;
typedef const struct _WNF_STATE_NAME* PCWNF_STATE_NAME;
#endif

/* Ps */

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _CLIENT_ID64
{
    VOID* POINTER_64 UniqueProcess;
    VOID* POINTER_64 UniqueThread;
} CLIENT_ID64, *PCLIENT_ID64;

typedef struct _CLIENT_ID32
{
    VOID* POINTER_32 UniqueProcess;
    VOID* POINTER_32 UniqueThread;
} CLIENT_ID32, *PCLIENT_ID32;

/* Io */

typedef struct _IO_STATUS_BLOCK
{
    union
    {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _IO_STATUS_BLOCK64
{
    union
    {
        NTSTATUS Status;
        VOID* POINTER_64 Pointer;
    };
    ULONG64 Information;
} IO_STATUS_BLOCK64, *PIO_STATUS_BLOCK64;

typedef struct _IO_STATUS_BLOCK32
{
    union
    {
        NTSTATUS Status;
        VOID* POINTER_32 Pointer;
    };
    ULONG Information;
} IO_STATUS_BLOCK32, *PIO_STATUS_BLOCK32;

typedef VOID(NTAPI *PIO_APC_ROUTINE)(
    _In_ PVOID ApcContext,
    _In_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG Reserved);

/* Ke */

typedef struct _KSYSTEM_TIME
{
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

/* Rtl */

typedef struct _RTL_BALANCED_NODE64
{
    union
    {
        struct _RTL_BALANCED_NODE64* POINTER_64 Children[2];
        struct
        {
            struct _RTL_BALANCED_NODE64* POINTER_64 Left;
            struct _RTL_BALANCED_NODE64* POINTER_64 Right;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    union
    {
        UCHAR Red : 1;
        UCHAR Balance : 2;
        ULONGLONG ParentValue;
    } DUMMYUNIONNAME2;
} RTL_BALANCED_NODE64, *PRTL_BALANCED_NODE64;

typedef struct _RTL_BALANCED_NODE32
{
    union
    {
        struct _RTL_BALANCED_NODE32* POINTER_32 Children[2];
        struct
        {
            struct _RTL_BALANCED_NODE32* POINTER_32 Left;
            struct _RTL_BALANCED_NODE32* POINTER_32 Right;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    union
    {
        UCHAR Red : 1;
        UCHAR Balance : 2;
        ULONG ParentValue;
    } DUMMYUNIONNAME2;
} RTL_BALANCED_NODE32, *PRTL_BALANCED_NODE32;

typedef struct _SINGLE_LIST_ENTRY64 SINGLE_LIST_ENTRY64, *PSINGLE_LIST_ENTRY64;
struct _SINGLE_LIST_ENTRY64
{
    SINGLE_LIST_ENTRY64* POINTER_64 Next;
};

#pragma endregion

#pragma region Macro

#pragma region Flags

//
//  These macros are used to test, set and clear flags respectivly
//

#ifndef FlagOn
#define FlagOn(_F,_SF)        ((_F) & (_SF))
#endif

#ifndef BooleanFlagOn
#define BooleanFlagOn(F,SF)   ((BOOLEAN)(((F) & (SF)) != 0))
#endif

#ifndef SetFlag
#define SetFlag(_F,_SF)       ((_F) |= (_SF))
#endif

#ifndef ClearFlag
#define ClearFlag(_F,_SF)     ((_F) &= ~(_SF))
#endif

#pragma endregion ntifs.h

#pragma region Assertion

#if defined(_PREFAST_)
#define NT_ANALYSIS_ASSUME(_exp) _Analysis_assume_(_exp)
#else
#if DBG
#define NT_ANALYSIS_ASSUME(_exp) ((void) 0)
#else
#define NT_ANALYSIS_ASSUME(_exp) __noop(_exp)
#endif
#endif

#define NT_ASSERT_ACTION(_exp) \
    ((!(_exp)) ? \
        (__annotation(L"Debug", L"AssertFail", L## #_exp), \
         DbgRaiseAssertionFailure(), FALSE) : \
        TRUE)

#define NT_ASSERTMSG_ACTION(_msg, _exp) \
    ((!(_exp)) ? \
        (__annotation(L"Debug", L"AssertFail", L##_msg), \
         DbgRaiseAssertionFailure(), FALSE) : \
        TRUE)

#define NT_ASSERTMSGW_ACTION(_msg, _exp) \
    ((!(_exp)) ? \
        (__annotation(L"Debug", L"AssertFail", _msg), \
         DbgRaiseAssertionFailure(), FALSE) : \
        TRUE)

#if DBG

#define NT_ASSERT_ASSUME(_exp) \
    (NT_ANALYSIS_ASSUME(_exp), NT_ASSERT_ACTION(_exp))
#define NT_ASSERTMSG_ASSUME(_msg, _exp) \
    (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSG_ACTION(_msg, _exp))
#define NT_ASSERTMSGW_ASSUME(_msg, _exp) \
    (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSGW_ACTION(_msg, _exp))

#define NT_ASSERT_NOASSUME     NT_ASSERT_ASSUME
#define NT_ASSERTMSG_NOASSUME  NT_ASSERTMSG_ASSUME
#define NT_ASSERTMSGW_NOASSUME NT_ASSERTMSGW_ASSUME

#define NT_VERIFY     NT_ASSERT
#define NT_VERIFYMSG  NT_ASSERTMSG
#define NT_VERIFYMSGW NT_ASSERTMSGW

#else // DBG

#define NT_ASSERT_ASSUME(_exp)           (NT_ANALYSIS_ASSUME(_exp), 0)
#define NT_ASSERTMSG_ASSUME(_msg, _exp)  (NT_ANALYSIS_ASSUME(_exp), 0)
#define NT_ASSERTMSGW_ASSUME(_msg, _exp) (NT_ANALYSIS_ASSUME(_exp), 0)

#define NT_ASSERT_NOASSUME(_exp)           ((void) 0)
#define NT_ASSERTMSG_NOASSUME(_msg, _exp)  ((void) 0)
#define NT_ASSERTMSGW_NOASSUME(_msg, _exp) ((void) 0)

#define NT_VERIFY(_exp)           (NT_ANALYSIS_ASSUME(_exp), ((_exp) ? TRUE : FALSE))
#define NT_VERIFYMSG(_msg, _exp ) (NT_ANALYSIS_ASSUME(_exp), ((_exp) ? TRUE : FALSE))
#define NT_VERIFYMSGW(_msg, _exp) (NT_ANALYSIS_ASSUME(_exp), ((_exp) ? TRUE : FALSE))

#endif // DBG

#define NT_FRE_ASSERT(_exp)           (NT_ANALYSIS_ASSUME(_exp), NT_ASSERT_ACTION(_exp))
#define NT_FRE_ASSERTMSG(_msg, _exp)  (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSG_ACTION(_msg, _exp))
#define NT_FRE_ASSERTMSGW(_msg, _exp) (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSGW_ACTION(_msg, _exp))

#ifdef NT_ASSERT_ALWAYS_ASSUMES
#define NT_ASSERT     NT_ASSERT_ASSUME
#define NT_ASSERTMSG  NT_ASSERTMSG_ASSUME
#define NT_ASSERTMSGW NT_ASSERTMSGW_ASSUME
#else
#define NT_ASSERT     NT_ASSERT_NOASSUME
#define NT_ASSERTMSG  NT_ASSERTMSG_NOASSUME
#define NT_ASSERTMSGW NT_ASSERTMSGW_NOASSUME
#endif

#pragma endregion wdm.h

#pragma region Pointer & Align & Size

/* ntifs.h */
#define RtlOffsetToPointer(B, O) ((PCHAR)(((PCHAR)(B)) + ((ULONG_PTR)(O))))
#define RtlPointerToOffset(B, P) ((ULONG)(((PCHAR)(P)) - ((PCHAR)(B))))

/* fltKernel.h */
#define Add2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))
#define PtrOffset(B,O) ((ULONG)((ULONG_PTR)(O) - (ULONG_PTR)(B)))
#define ROUND_TO_SIZE(_length, _alignment) ((((ULONG_PTR)(_length)) + ((_alignment) - 1)) & ~(ULONG_PTR)((_alignment) - 1))
#define IS_ALIGNED(_pointer, _alignment) ((((ULONG_PTR)(_pointer)) & ((_alignment) - 1)) == 0)

/* wdm.h */

#undef ALIGN_DOWN_BY
#undef ALIGN_UP_BY
#undef ALIGN_DOWN_POINTER_BY
#undef ALIGN_UP_POINTER_BY
#undef ALIGN_DOWN
#undef ALIGN_UP
#undef ALIGN_DOWN_POINTER
#undef ALIGN_UP_POINTER
#define ALIGN_DOWN_BY(length, alignment) ((ULONG_PTR)(length) & ~((ULONG_PTR)(alignment) - 1))
#define ALIGN_UP_BY(length, alignment) (ALIGN_DOWN_BY(((ULONG_PTR)(length) + (alignment) - 1), alignment))
#define ALIGN_DOWN_POINTER_BY(address, alignment) ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)(alignment) - 1)))
#define ALIGN_UP_POINTER_BY(address, alignment) (ALIGN_DOWN_POINTER_BY(((ULONG_PTR)(address) + (alignment) - 1), alignment))
#define ALIGN_DOWN(length, type) ALIGN_DOWN_BY(length, sizeof(type))
#define ALIGN_UP(length, type) ALIGN_UP_BY(length, sizeof(type))
#define ALIGN_DOWN_POINTER(address, type) ALIGN_DOWN_POINTER_BY(address, sizeof(type))
#define ALIGN_UP_POINTER(address, type) ALIGN_UP_POINTER_BY(address, sizeof(type))

#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field) (sizeof(((type*)0)->field))
#endif

/* ksxxx.inc */
#if defined(_WIN64)
#define SizeofPointer 8
#else
#define SizeofPointer 4
#endif

#pragma endregion

/* wdm.h */

#if defined(_WIN64)
#define BitScanReverseSizeT BitScanReverse64
#define BitScanForwardSizeT BitScanForward64
#else
#define BitScanReverseSizeT BitScanReverse
#define BitScanForwardSizeT BitScanForward
#endif

#if DBG
#define IF_DEBUG if (TRUE)
#else
#define IF_DEBUG if (FALSE)
#endif

#pragma endregion

#pragma endregion
