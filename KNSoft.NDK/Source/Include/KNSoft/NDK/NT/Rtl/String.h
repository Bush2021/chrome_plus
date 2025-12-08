#pragma once

#include "../MinDef.h"
#include "../Nls.h"
#include "DataStructures/GenericTable.h"

EXTERN_C_START

/* phnt & wdm.h */

NTSYSAPI
VOID
NTAPI
RtlInitString(
    _Out_ PSTRING DestinationString,
    _In_opt_z_ PCSTR SourceString
);

#if (NTDDI_VERSION >= NTDDI_WIN10)
NTSYSAPI
NTSTATUS
NTAPI
RtlInitStringEx(
    _Out_ PSTRING DestinationString,
    _In_opt_z_ PCSZ SourceString
);
#endif

NTSYSAPI
VOID
NTAPI
RtlInitAnsiString(
    _Out_ PANSI_STRING DestinationString,
    _In_opt_z_ PCSTR SourceString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlInitAnsiStringEx(
    _Out_ PANSI_STRING DestinationString,
    _In_opt_z_ PCSZ SourceString
);

_At_(UnicodeString->Buffer, _Post_equal_to_(Buffer))
_At_(UnicodeString->Length, _Post_equal_to_(0))
_At_(UnicodeString->MaximumLength, _Post_equal_to_(BufferSize))
FORCEINLINE
VOID
RtlInitEmptyUnicodeString(
    _Out_ PUNICODE_STRING UnicodeString,
    _Writable_bytes_(BufferSize)
    _When_(BufferSize != 0, _Notnull_)
    __drv_aliasesMem PWCHAR Buffer,
    _In_ USHORT BufferSize
    )
{
    memset(UnicodeString, 0, sizeof(*UnicodeString));
    UnicodeString->MaximumLength = BufferSize;
    UnicodeString->Buffer = Buffer;
}

_At_(AnsiString->Buffer, _Post_equal_to_(Buffer))
_At_(AnsiString->Length, _Post_equal_to_(0))
_At_(AnsiString->MaximumLength, _Post_equal_to_(BufferSize))
FORCEINLINE
VOID
RtlInitEmptyAnsiString(
    _Out_ PANSI_STRING AnsiString,
    _Pre_maybenull_ _Pre_readable_size_(BufferSize) __drv_aliasesMem PCHAR Buffer,
    _In_ USHORT BufferSize
    )
{
    memset(AnsiString, 0, sizeof(*AnsiString));
    AnsiString->MaximumLength = BufferSize;
    AnsiString->Buffer = Buffer;
}

#pragma prefast(push)
#pragma prefast(disable : 6101, "Out parameter is not written fully or at all.")

FORCEINLINE
VOID
RtlSanitizeUnicodeStringPadding(
    _Out_ PUNICODE_STRING String
    )
{
#if defined(_WIN64)

    ULONG PaddingSize;
    ULONG PaddingStart;

    PaddingStart = FIELD_OFFSET(UNICODE_STRING, MaximumLength) +
                   sizeof(String->MaximumLength);

    PaddingSize = FIELD_OFFSET(UNICODE_STRING, Buffer) - PaddingStart;

    memset((PCH)String + PaddingStart, 0, PaddingSize);

#else

    UNREFERENCED_PARAMETER(String);

#endif

}

#pragma prefast(pop)

NTSYSAPI
VOID
NTAPI
RtlFreeAnsiString(
    _Inout_ _At_(AnsiString->Buffer, _Frees_ptr_opt_) PANSI_STRING AnsiString
);

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)
NTSYSAPI
VOID
NTAPI
RtlInitUTF8String(
    _Out_ PUTF8_STRING DestinationString,
    _In_opt_z_ PCSZ SourceString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlInitUTF8StringEx(
    _Out_ PUTF8_STRING DestinationString,
    _In_opt_z_ PCSZ SourceString
);

NTSYSAPI
VOID
NTAPI
RtlFreeUTF8String(
    _Inout_ _At_(Utf8String->Buffer, _Frees_ptr_opt_) PUTF8_STRING Utf8String
);
#endif

NTSYSAPI
VOID
NTAPI
RtlFreeOemString(
    _Inout_ POEM_STRING OemString
);

NTSYSAPI
VOID
NTAPI
RtlCopyString(
    _In_ PSTRING DestinationString,
    _In_opt_ PSTRING SourceString
);

NTSYSAPI
CHAR
NTAPI
RtlUpperChar(
    _In_ CHAR Character
);

_Must_inspect_result_
NTSYSAPI
LONG
NTAPI
RtlCompareString(
    _In_ PSTRING String1,
    _In_ PSTRING String2,
    _In_ BOOLEAN CaseInSensitive
);

_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlEqualString(
    _In_ PSTRING String1,
    _In_ PSTRING String2,
    _In_ BOOLEAN CaseInSensitive
);

_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixString(
    _In_ PSTRING String1,
    _In_ PSTRING String2,
    _In_ BOOLEAN CaseInSensitive
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendStringToString(
    _Inout_ PSTRING Destination,
    _In_ PSTRING Source
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendAsciizToString(
    _Inout_ PSTRING Destination,
    _In_opt_z_ PCSTR Source
);

NTSYSAPI
VOID
NTAPI
RtlUpperString(
    _Inout_ PSTRING DestinationString,
    _In_ const STRING* SourceString
);

FORCEINLINE
BOOLEAN
RtlIsNullOrEmptyUnicodeString(
    _In_opt_ PUNICODE_STRING String
)
{
    return !String || String->Length == 0;
}

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_z_ PCWSTR SourceString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlInitUnicodeStringEx(
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_z_ PCWSTR SourceString
);

/**
 * The RtlCreateUnicodeString routine creates a new counted Unicode string.
 *
 * \param DestinationString Pointer to the newly allocated and initialized Unicode string.
 * \param SourceString Pointer to a null-terminated Unicode string with which to initialize the new string.
 * \return TRUE if the Unicode string was successfully created, FALSE otherwise.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlcreateunicodestring
 */
_Success_(return != 0)
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeString(
    _Out_ PUNICODE_STRING DestinationString,
    _In_z_ PCWSTR SourceString
);

NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeStringFromAsciiz(
    _Out_ PUNICODE_STRING DestinationString,
    _In_z_ PCSTR SourceString
);

/**
 * The RtlFreeUnicodeString routine releases storage that was allocated by RtlAnsiStringToUnicodeString or RtlUpcaseUnicodeString.
 *
 * \param UnicodeString A pointer to the string buffer.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlfreeunicodestring
 */
NTSYSAPI
VOID
NTAPI
RtlFreeUnicodeString(
    _Inout_ _At_(UnicodeString->Buffer, _Frees_ptr_opt_) PUNICODE_STRING UnicodeString
);

#define RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE (0x00000001)
#define RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING (0x00000002)

NTSYSAPI
NTSTATUS
NTAPI
RtlDuplicateUnicodeString(
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING StringIn,
    _Out_ PUNICODE_STRING StringOut
);

/**
 * The RtlCopyUnicodeString routine copies a source string to a destination string.
 *
 * \param[in] DestinationString A pointer to the destination string buffer.
 * \param[in] SourceString A pointer to the source string buffer.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopyunicodestring
 */
NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    _In_ PUNICODE_STRING DestinationString,
    _In_opt_ PCUNICODE_STRING SourceString
);

/**
 * The RtlUpcaseUnicodeChar routine converts the specified Unicode character to uppercase.
 *
 * \param[in] SourceCharacter Specifies the character to convert.
 * \return The uppercase version of the specified Unicode character.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlupcaseunicodechar
 */
NTSYSAPI
WCHAR
NTAPI
RtlUpcaseUnicodeChar(
    _In_ WCHAR SourceCharacter
);

/**
 * The RtlDowncaseUnicodeChar routine converts the specified Unicode character to lowercase.
 *
 * \param[in] SourceCharacter Specifies the character to convert.
 * \return The lowercase version of the specified Unicode character.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtldowncaseunicodechar
 */
NTSYSAPI
WCHAR
NTAPI
RtlDowncaseUnicodeChar(
    _In_ WCHAR SourceCharacter
);

/**
 * The RtlCompareUnicodeString routine compares two Unicode strings.
 * 
 * \param[in] String1 Pointer to the first string.
 * \param[in] String2 Pointer to the second string.
 * \param[in] CaseInSensitive If TRUE, case should be ignored when doing the comparison.
 * \return A signed value that gives the results of the comparison.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcompareunicodestring
 */
_Must_inspect_result_
NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeString(
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
);

/**
 * The RtlCompareUnicodeStrings routine compares two Unicode strings.
 * 
 * \param[in] String1 Pointer to the first string.
 * \param[in] String1Length The length, in bytes, of the first string.
 * \param[in] String2 Pointer to the second string.
 * \param[in] String2Length The length, in bytes, of the second string.
 * \param[in] CaseInSensitive If TRUE, case should be ignored when doing the comparison.
 * \return A signed value that gives the results of the comparison.
 */
_Must_inspect_result_
NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeStrings(
    _In_reads_(String1Length) PCWCH String1,
    _In_ SIZE_T String1Length,
    _In_reads_(String2Length) PCWCH String2,
    _In_ SIZE_T String2Length,
    _In_ BOOLEAN CaseInSensitive
);

/**
 * The RtlEqualUnicodeString routine compares two Unicode strings to determine whether they are equal.
 *
 * \param[in] String1 Pointer to the first Unicode string.
 * \param[in] String2 Pointer to the second Unicode string.
 * \param[in] CaseInSensitive If TRUE, case should be ignored when doing the comparison.
 * \return TRUE if the two Unicode strings are equal; otherwise, it returns FALSE.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlequalunicodestring
 */
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
);

#define HASH_STRING_ALGORITHM_DEFAULT 0
#define HASH_STRING_ALGORITHM_X65599 1
#define HASH_STRING_ALGORITHM_INVALID 0xffffffff

/**
 * The RtlHashUnicodeString routine creates a hash value from a given Unicode string and hash algorithm.
 *
 * \param[in] String A pointer to a UNICODE_STRING structure that contains the Unicode string to be converted to a hash value.
 * \param[in] CaseInSensitive Specifies whether to treat the Unicode string as case sensitive when computing the hash value. If CaseInSensitive is TRUE, a lowercase and uppercase string hash to the same value.
 * \param[in] HashAlgorithm The hash algorithm to use.
 * \param[out] HashValue A pointer to a ULONG variable that receives the hash value.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlhashunicodestring
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlHashUnicodeString(
    _In_ PUNICODE_STRING String,
    _In_ BOOLEAN CaseInSensitive,
    _In_ ULONG HashAlgorithm,
    _Out_ PULONG HashValue
);

NTSYSAPI
NTSTATUS
NTAPI
RtlValidateUnicodeString(
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING String
);

/**
 * The RtlPrefixUnicodeString routine compares two Unicode strings to determine whether one string is a prefix of the other.
 *
 * \param[in] String1 Pointer to the first string, which might be a prefix of the buffered Unicode string at String2.
 * \param[in] String2 Pointer to the second string.
 * \param[in] CaseInSensitive TRUE, case should be ignored when doing the comparison.
 * \return TRUE if String1 is a prefix of String2.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-rtlprefixunicodestring
 */
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixUnicodeString(
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
);

#if defined(_KERNEL_MODE) && NTDDI_VERSION >= NTDDI_WIN10
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlSuffixUnicodeString(
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10)
_Must_inspect_result_
NTSYSAPI
PWCHAR
NTAPI
RtlFindUnicodeSubstring(
    _In_ PUNICODE_STRING FullString,
    _In_ PUNICODE_STRING SearchString,
    _In_ BOOLEAN CaseInSensitive
);
#endif

#define RTL_FIND_CHAR_IN_UNICODE_STRING_START_AT_END 0x00000001
#define RTL_FIND_CHAR_IN_UNICODE_STRING_COMPLEMENT_CHAR_SET 0x00000002
#define RTL_FIND_CHAR_IN_UNICODE_STRING_CASE_INSENSITIVE 0x00000004

NTSYSAPI
NTSTATUS
NTAPI
RtlFindCharInUnicodeString(
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING StringToSearch,
    _In_ PUNICODE_STRING CharSet,
    _Out_ PUSHORT NonInclusivePrefixLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString(
    _Inout_ PUNICODE_STRING Destination,
    _In_ PCUNICODE_STRING Source
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString(
    _Inout_ PUNICODE_STRING Destination,
    _In_opt_z_ PCWSTR Source
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDowncaseUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
VOID
NTAPI
RtlEraseUnicodeString(
    _Inout_ PUNICODE_STRING String
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PCANSI_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
ULONG
NTAPI
RtlxAnsiStringToUnicodeSize(
    _In_ PCANSI_STRING AnsiString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    _Inout_ PANSI_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

// rev
NTSYSAPI
ULONG
NTAPI
RtlUnicodeStringToAnsiSize(
    _In_ PUNICODE_STRING SourceString
);

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToUTF8String(
    _Inout_ PUTF8_STRING DestinationString,
    _In_ PCUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUTF8StringToUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PUTF8_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);
#endif

NTSYSAPI
WCHAR
NTAPI
RtlAnsiCharToUnicodeChar(
    _Inout_ PUCHAR * SourceCharacter
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToAnsiString(
    _Inout_ PANSI_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlOemStringToUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ POEM_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToOemString(
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToOemString(
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlOemStringToCountedUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PCOEM_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToCountedOemString(
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToCountedOemString(
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
);

/**
 * The RtlMultiByteToUnicodeN routine translates the specified source string into a Unicode string, using the current system ANSI code page (ACP).
 * The source string is not necessarily from a multibyte character set.
 *
 * \param UnicodeString Pointer to a caller-allocated buffer that receives the translated string. UnicodeString buffer must not overlap with MultiByteString buffer.
 * \param MaxBytesInUnicodeString Maximum number of bytes to be written at UnicodeString. If this value causes the translated string to be truncated, RtlMultiByteToUnicodeN does not return an error status.
 * \param BytesInUnicodeString Pointer to a caller-allocated variable that receives the length, in bytes, of the translated string. This parameter can be NULL.
 * \param MultiByteString Pointer to the string to be translated.
 * \param BytesInMultiByteString Size, in bytes, of the string at MultiByteString.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlmultibytetounicoden
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeN(
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInMultiByteString) PCSTR MultiByteString,
    _In_ ULONG BytesInMultiByteString
);

/**
 * The RtlMultiByteToUnicodeSize routine determines the number of bytes that are required to store the Unicode translation for the specified source string.
 * The translation is assumed to use the current system ANSI code page (ACP). The source string is not necessarily from a multibyte character set.
 *
 * \param BytesInUnicodeString Pointer to a caller-allocated variable that receives the number of bytes that are required to store the translated string.
 * \param MultiByteString Pointer to the source string for which the Unicode length is to be calculated.
 * \param BytesInMultiByteString Length, in bytes, of the source string.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlmultibytetounicodesize
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeSize(
    _Out_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInMultiByteString) PCSTR MultiByteString,
    _In_ ULONG BytesInMultiByteString
);

/**
 * The RtlUnicodeToMultiByteN routine translates the specified Unicode string into a new character string, using the current system ANSI code page (ACP).
 * The source string is not necessarily from a multibyte character set.
 *
 * \param MultiByteString Pointer to a caller-allocated buffer to receive the translated string. MultiByteString buffer must not overlap with UnicodeString buffer.
 * \param MaxBytesInMultiByteString Maximum number of bytes to be written to MultiByteString. If this value causes the translated string to be truncated, RtlUnicodeToMultiByteN does not return an error status.
 * \param BytesInMultiByteString Pointer to a caller-allocated variable that receives the length, in bytes, of the translated string. This parameter is optional and can be NULL.
 * \param UnicodeString Pointer to the Unicode source string to be translated.
 * \param BytesInUnicodeString Size, in bytes, of the string at UnicodeString.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlunicodetomultibyten
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    _Out_writes_bytes_to_(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    _In_ ULONG MaxBytesInMultiByteString,
    _Out_opt_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PCWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

/**
 * The RtlUnicodeToMultiByteSize routine determines the number of bytes that are required to store the multibyte translation for the specified Unicode string.
 * The translation is assumed to use the current system ANSI code page (ACP). The source string is not necessarily from a multibyte character set.
 *
 * \param BytesInMultiByteString Pointer to a caller-allocated variable that receives the number of bytes required to store the translated string.
 * \param UnicodeString Pointer to the Unicode string for which the multibyte length is to be calculated.
 * \param BytesInUnicodeString Length, in bytes, of the source string.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlunicodetomultibytesize
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteSize(
    _Out_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PCWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

/**
 * The RtlUpcaseUnicodeToMultiByteN routine translates the specified Unicode string into a new uppercase character string, using the current system ANSI code page (ACP).
 * The translated string is not necessarily from a multibyte character set.
 *
 * \param MultiByteString Pointer to a caller-allocated buffer to receive the translated string.
 * \param MaxBytesInMultiByteString Maximum number of bytes to be written at MultiByteString. If this value causes the translated string to be truncated, RtlUpcaseUnicodeToMultiByteN does not return an error status.
 * \param BytesInMultiByteString Pointer to a caller-allocated variable that receives the length, in bytes, of the translated string. This parameter can be NULL.
 * \param UnicodeString Pointer to the Unicode source string to be translated.
 * \param BytesInUnicodeString Size, in bytes, of the string at UnicodeString.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlupcaseunicodetomultibyten
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToMultiByteN(
    _Out_writes_bytes_to_(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    _In_ ULONG MaxBytesInMultiByteString,
    _Out_opt_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PCWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlOemToUnicodeN(
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWSTR UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInOemString) PCCH OemString,
    _In_ ULONG BytesInOemString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToOemN(
    _Out_writes_bytes_to_(MaxBytesInOemString, *BytesInOemString) PCHAR OemString,
    _In_ ULONG MaxBytesInOemString,
    _Out_opt_ PULONG BytesInOemString,
    _In_reads_bytes_(BytesInUnicodeString) PCWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToOemN(
    _Out_writes_bytes_to_(MaxBytesInOemString, *BytesInOemString) PCHAR OemString,
    _In_ ULONG MaxBytesInOemString,
    _Out_opt_ PULONG BytesInOemString,
    _In_reads_bytes_(BytesInUnicodeString) PCWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlConsoleMultiByteToUnicodeN(
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInMultiByteString) PCCH MultiByteString,
    _In_ ULONG BytesInMultiByteString,
    _Out_ PULONG pdwSpecialChar
);

/**
 * The RtlUTF8ToUnicodeN routine translates the specified source string into a Unicode string, using the 8-bit Unicode Transformation Format (UTF-8) code page.
 *
 * \param UnicodeStringDestination Pointer to a caller-allocated buffer to receive the translated string.
 * \param UnicodeStringMaxByteCount Maximum number of bytes to be written at MultiByteString. If this value causes the translated string to be truncated, RtlUpcaseUnicodeToMultiByteN does not return an error status.
 * \param UnicodeStringActualByteCount Pointer to a caller-allocated variable that receives the length, in bytes, of the translated string. This parameter can be NULL.
 * \param UTF8StringSource Pointer to the Unicode source string to be translated.
 * \param UTF8StringByteCount Size, in bytes, of the string at UnicodeString.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlutf8tounicoden
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlUTF8ToUnicodeN(
    _Out_writes_bytes_to_(UnicodeStringMaxByteCount, *UnicodeStringActualByteCount) PWSTR UnicodeStringDestination,
    _In_ ULONG UnicodeStringMaxByteCount,
    _Out_opt_ PULONG UnicodeStringActualByteCount,
    _In_reads_bytes_(UTF8StringByteCount) PCCH UTF8StringSource,
    _In_ ULONG UTF8StringByteCount
);

/**
 * The RtlUnicodeToUTF8N routine translates the specified Unicode string into a new character string, using the 8-bit Unicode Transformation Format (UTF-8) code page.
 *
 * \param UTF8StringDestination Pointer to a caller-allocated buffer to receive the translated string.
 * \param UTF8StringMaxByteCount Maximum number of bytes to be written to UTF8StringDestination. If this value causes the translated string to be truncated, RtlUnicodeToUTF8N returns an error status.
 * \param UTF8StringActualByteCount A pointer to a caller-allocated variable that receives the length, in bytes, of the translated string. This parameter is optional and can be NULL. If the string is truncated then the returned number counts the actual truncated string count.
 * \param UnicodeStringSource A pointer to the Unicode source string to be translated.
 * \param UnicodeStringByteCount Specifies the number of bytes in the Unicode source string that the UnicodeStringSource parameter points to.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlunicodetoutf8n
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToUTF8N(
    _Out_writes_bytes_to_(UTF8StringMaxByteCount, *UTF8StringActualByteCount) PCHAR UTF8StringDestination,
    _In_ ULONG UTF8StringMaxByteCount,
    _Out_opt_ PULONG UTF8StringActualByteCount,
    _In_reads_bytes_(UnicodeStringByteCount) PCWCH UnicodeStringSource,
    _In_ ULONG UnicodeStringByteCount
);

NTSYSAPI
NTSTATUS
NTAPI
RtlCustomCPToUnicodeN(
    _In_ PCPTABLEINFO CustomCP,
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInCustomCPString) PCH CustomCPString,
    _In_ ULONG BytesInCustomCPString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToCustomCPN(
    _In_ PCPTABLEINFO CustomCP,
    _Out_writes_bytes_to_(MaxBytesInCustomCPString, *BytesInCustomCPString) PCH CustomCPString,
    _In_ ULONG MaxBytesInCustomCPString,
    _Out_opt_ PULONG BytesInCustomCPString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToCustomCPN(
    _In_ PCPTABLEINFO CustomCP,
    _Out_writes_bytes_to_(MaxBytesInCustomCPString, *BytesInCustomCPString) PCH CustomCPString,
    _In_ ULONG MaxBytesInCustomCPString,
    _Out_opt_ PULONG BytesInCustomCPString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
);

NTSYSAPI
VOID
NTAPI
RtlInitCodePageTable(
    _In_reads_opt_(2) PUSHORT TableBase,
    _Inout_ PCPTABLEINFO CodePageTable
);

NTSYSAPI
VOID
NTAPI
RtlInitNlsTables(
    _In_ PUSHORT AnsiNlsBase,
    _In_ PUSHORT OemNlsBase,
    _In_ PUSHORT LanguageNlsBase,
    _Out_ PNLSTABLEINFO TableInfo // PCPTABLEINFO?
);

NTSYSAPI
VOID
NTAPI
RtlResetRtlTranslations(
    _In_ PNLSTABLEINFO TableInfo
);

NTSYSAPI
BOOLEAN
NTAPI
RtlIsTextUnicode(
    _In_ PVOID Buffer,
    _In_ ULONG Size,
    _Inout_opt_ PULONG Result
);

typedef enum _RTL_NORM_FORM
{
    NormOther = 0x0,
    NormC = 0x1,
    NormD = 0x2,
    NormKC = 0x5,
    NormKD = 0x6,
    NormIdna = 0xd,
    DisallowUnassigned = 0x100,
    NormCDisallowUnassigned = 0x101,
    NormDDisallowUnassigned = 0x102,
    NormKCDisallowUnassigned = 0x105,
    NormKDDisallowUnassigned = 0x106,
    NormIdnaDisallowUnassigned = 0x10d
} RTL_NORM_FORM;

NTSYSAPI
NTSTATUS
NTAPI
RtlNormalizeString(
    _In_ ULONG NormForm, // RTL_NORM_FORM
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIsNormalizedString(
    _In_ ULONG NormForm, // RTL_NORM_FORM
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_ PBOOLEAN Normalized
);

// ntifs:FsRtlIsNameInExpression
/**
 * The RtlIsNameInExpression routine determines whether a Unicode string matches the specified pattern.
 *
 * \param Expression A pointer to the pattern string. This string can contain wildcard characters. If the IgnoreCase parameter is TRUE, the string must contain only uppercase characters.
 * \param Name Maximum number of bytes to be written to UTF8StringDestination. If this value causes the translated string to be truncated, RtlUnicodeToUTF8N returns an error status.
 * \param IgnoreCase TRUE for case-insensitive matching, or FALSE for case-sensitive matching.
 * \param UpcaseTable An optional pointer to an uppercase character table to use for case-insensitive matching. If this parameter is NULL, the default system uppercase character table is used.
 * \return TRUE if the string matches the pattern. If the string does not match the pattern, this function returns FALSE.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlisnameinexpression
 */
NTSYSAPI
BOOLEAN
NTAPI
RtlIsNameInExpression(
    _In_ PUNICODE_STRING Expression,
    _In_ PUNICODE_STRING Name,
    _In_ BOOLEAN IgnoreCase,
    _In_opt_ PWCH UpcaseTable
);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS4)
// rev
NTSYSAPI
BOOLEAN
NTAPI
RtlIsNameInUnUpcasedExpression(
    _In_ PUNICODE_STRING Expression,
    _In_ PUNICODE_STRING Name,
    _In_ BOOLEAN IgnoreCase,
    _In_opt_ PWCH UpcaseTable
);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_19H1)
/**
 * The RtlDoesNameContainWildCards routine determines whether a Unicode string contains wildcard characters.
 *
 * \param Name A pointer to the string to be checked.
 * \return TRUE if one or more wildcard characters were found, FALSE otherwise.
 * \remarks The following are wildcard characters: *, ?, ANSI_DOS_STAR, ANSI_DOS_DOT, and ANSI_DOS_QM.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-_fsrtl_advanced_fcb_header-fsrtldoesnamecontainwildcards
 */
NTSYSAPI
BOOLEAN
NTAPI
RtlDoesNameContainWildCards(
    _In_ PUNICODE_STRING Expression
);
#endif

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualDomainName(
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2
);

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualComputerName(
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDnsHostNameToComputerName(
    _Out_ PUNICODE_STRING ComputerNameString,
    _In_ PUNICODE_STRING DnsHostNameString,
    _In_ BOOLEAN AllocateComputerNameString
);

NTSYSAPI
LONG
NTAPI
RtlCompareAltitudes(
    _In_ PUNICODE_STRING Altitude1,
    _In_ PUNICODE_STRING Altitude2
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIdnToAscii(
    _In_ ULONG Flags,
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIdnToUnicode(
    _In_ ULONG Flags,
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIdnToNameprepUnicode(
    _In_ ULONG Flags,
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIntegerToChar(
    _In_ ULONG Value,
    _In_opt_ ULONG Base,
    _In_ LONG OutputLength, // negative to pad to width
    _Out_ PSTR String
);

NTSYSAPI
NTSTATUS
NTAPI
RtlCharToInteger(
    _In_z_ PCSTR String,
    _In_opt_ ULONG Base,
    _Out_ PULONG Value
);

NTSYSAPI
NTSTATUS
NTAPI
RtlLargeIntegerToChar(
    _In_ PLARGE_INTEGER Value,
    _In_opt_ ULONG Base,
    _In_ LONG OutputLength,
    _Out_ PSTR String
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIntegerToUnicodeString(
    _In_ ULONG Value,
    _In_opt_ ULONG Base,
    _Inout_ PUNICODE_STRING String
);

NTSYSAPI
NTSTATUS
NTAPI
RtlInt64ToUnicodeString(
    _In_ ULONGLONG Value,
    _In_opt_ ULONG Base,
    _Inout_ PUNICODE_STRING String
);

#ifdef _WIN64
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlInt64ToUnicodeString(Value, Base, String)
#else
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlIntegerToUnicodeString(Value, Base, String)
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToInteger(
    _In_ PUNICODE_STRING String,
    _In_opt_ ULONG Base,
    _Out_ PULONG Value
);

// IPv4/6 conversion

typedef struct in_addr IN_ADDR, *PIN_ADDR;
typedef struct in6_addr IN6_ADDR, *PIN6_ADDR;
typedef IN_ADDR const *PCIN_ADDR;
typedef IN6_ADDR const *PCIN6_ADDR;

NTSYSAPI
PSTR
NTAPI
RtlIpv4AddressToStringA(
    _In_ PCIN_ADDR Address,
    _Out_writes_(16) PSTR AddressString
);

NTSYSAPI
PWSTR
NTAPI
RtlIpv4AddressToStringW(
    _In_ PCIN_ADDR Address,
    _Out_writes_(16) PWSTR AddressString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4AddressToStringExA(
    _In_ PCIN_ADDR Address,
    _In_ USHORT Port,
    _Out_writes_to_(*AddressStringLength, *AddressStringLength) PSTR AddressString,
    _Inout_ PULONG AddressStringLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4AddressToStringExW(
    _In_ PCIN_ADDR Address,
    _In_ USHORT Port,
    _Out_writes_to_(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    _Inout_ PULONG AddressStringLength
);

NTSYSAPI
PSTR
NTAPI
RtlIpv6AddressToStringA(
    _In_ PCIN6_ADDR Address,
    _Out_writes_(46) PSTR AddressString
);

NTSYSAPI
PWSTR
NTAPI
RtlIpv6AddressToStringW(
    _In_ PCIN6_ADDR Address,
    _Out_writes_(46) PWSTR AddressString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6AddressToStringExA(
    _In_ PCIN6_ADDR Address,
    _In_ ULONG ScopeId,
    _In_ USHORT Port,
    _Out_writes_to_(*AddressStringLength, *AddressStringLength) PSTR AddressString,
    _Inout_ PULONG AddressStringLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6AddressToStringExW(
    _In_ PCIN6_ADDR Address,
    _In_ ULONG ScopeId,
    _In_ USHORT Port,
    _Out_writes_to_(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    _Inout_ PULONG AddressStringLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressA(
    _In_ PCSTR AddressString,
    _In_ BOOLEAN Strict,
    _Out_ PCSTR* Terminator,
    _Out_ PIN_ADDR Address
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressW(
    _In_ PCWSTR AddressString,
    _In_ BOOLEAN Strict,
    _Out_ LPCWSTR* Terminator,
    _Out_ PIN_ADDR Address
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressExA(
    _In_ PCSTR AddressString,
    _In_ BOOLEAN Strict,
    _Out_ PIN_ADDR Address,
    _Out_ PUSHORT Port
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressExW(
    _In_ PCWSTR AddressString,
    _In_ BOOLEAN Strict,
    _Out_ PIN_ADDR Address,
    _Out_ PUSHORT Port
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressA(
    _In_ PCSTR AddressString,
    _Out_ PCSTR* Terminator,
    _Out_ PIN6_ADDR Address
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressW(
    _In_ PCWSTR AddressString,
    _Out_ PCWSTR* Terminator,
    _Out_ PIN6_ADDR Address
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressExA(
    _In_ PCSTR AddressString,
    _Out_ PIN6_ADDR Address,
    _Out_ PULONG ScopeId,
    _Out_ PUSHORT Port
);

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressExW(
    _In_ PCWSTR AddressString,
    _Out_ PIN6_ADDR Address,
    _Out_ PULONG ScopeId,
    _Out_ PUSHORT Port
);

#define RtlIpv4AddressToString RtlIpv4AddressToStringW
#define RtlIpv4AddressToStringEx RtlIpv4AddressToStringExW
#define RtlIpv6AddressToString RtlIpv6AddressToStringW
#define RtlIpv6AddressToStringEx RtlIpv6AddressToStringExW
#define RtlIpv4StringToAddress RtlIpv4StringToAddressW
#define RtlIpv4StringToAddressEx RtlIpv4StringToAddressExW
#define RtlIpv6StringToAddress RtlIpv6StringToAddressW
#define RtlIpv6StringToAddressEx RtlIpv6StringToAddressExW

#pragma region Prefix

typedef struct _PREFIX_TABLE_ENTRY
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    struct _PREFIX_TABLE_ENTRY *NextPrefixTree;
    RTL_SPLAY_LINKS Links;
    PSTRING Prefix;
} PREFIX_TABLE_ENTRY, *PPREFIX_TABLE_ENTRY;

typedef struct _PREFIX_TABLE
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    PPREFIX_TABLE_ENTRY NextPrefixTree;
} PREFIX_TABLE, *PPREFIX_TABLE;

NTSYSAPI
VOID
NTAPI
PfxInitialize(
    _Out_ PPREFIX_TABLE PrefixTable
);

NTSYSAPI
BOOLEAN
NTAPI
PfxInsertPrefix(
    _In_ PPREFIX_TABLE PrefixTable,
    _In_ PSTRING Prefix,
    _Out_ PPREFIX_TABLE_ENTRY PrefixTableEntry
);

NTSYSAPI
VOID
NTAPI
PfxRemovePrefix(
    _In_ PPREFIX_TABLE PrefixTable,
    _In_ PPREFIX_TABLE_ENTRY PrefixTableEntry
);

NTSYSAPI
PPREFIX_TABLE_ENTRY
NTAPI
PfxFindPrefix(
    _In_ PPREFIX_TABLE PrefixTable,
    _In_ PSTRING FullName
);

typedef struct _UNICODE_PREFIX_TABLE_ENTRY
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    struct _UNICODE_PREFIX_TABLE_ENTRY *NextPrefixTree;
    struct _UNICODE_PREFIX_TABLE_ENTRY *CaseMatch;
    RTL_SPLAY_LINKS Links;
    PUNICODE_STRING Prefix;
} UNICODE_PREFIX_TABLE_ENTRY, *PUNICODE_PREFIX_TABLE_ENTRY;

typedef struct _UNICODE_PREFIX_TABLE
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    PUNICODE_PREFIX_TABLE_ENTRY NextPrefixTree;
    PUNICODE_PREFIX_TABLE_ENTRY LastNextEntry;
} UNICODE_PREFIX_TABLE, *PUNICODE_PREFIX_TABLE;

NTSYSAPI
VOID
NTAPI
RtlInitializeUnicodePrefix(
    _Out_ PUNICODE_PREFIX_TABLE PrefixTable
);

NTSYSAPI
BOOLEAN
NTAPI
RtlInsertUnicodePrefix(
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ PUNICODE_STRING Prefix,
    _Out_ PUNICODE_PREFIX_TABLE_ENTRY PrefixTableEntry
);

NTSYSAPI
VOID
NTAPI
RtlRemoveUnicodePrefix(
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ PUNICODE_PREFIX_TABLE_ENTRY PrefixTableEntry
);

NTSYSAPI
PUNICODE_PREFIX_TABLE_ENTRY
NTAPI
RtlFindUnicodePrefix(
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ PUNICODE_STRING FullName,
    _In_ ULONG CaseInsensitiveIndex
);

NTSYSAPI
PUNICODE_PREFIX_TABLE_ENTRY
NTAPI
RtlNextUnicodePrefix(
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ BOOLEAN Restart
);

#pragma endregion

#pragma region Encode/Decode

NTSYSAPI
VOID
NTAPI
RtlRunEncodeUnicodeString(
    _Inout_ PUCHAR Seed,
    _Inout_ PUNICODE_STRING String);

NTSYSAPI
VOID
NTAPI
RtlRunDecodeUnicodeString(
    _In_ UCHAR Seed,
    _Inout_ PUNICODE_STRING String);

NTSYSAPI
NTSTATUS
NTAPI
RtlFindClosestEncodableLength(
    _In_ ULONGLONG SourceLength,
    _Out_ PULONGLONG TargetLength);

#pragma endregion

typedef struct _RTL_UNICODE_STRING_BUFFER RTL_UNICODE_STRING_BUFFER, *PRTL_UNICODE_STRING_BUFFER;

NTSYSAPI
NTSTATUS
NTAPI
RtlMultiAppendUnicodeStringBuffer(
    _Inout_ PRTL_UNICODE_STRING_BUFFER Buffer,
    _In_ ULONG BufferCount,
    _In_ PCUNICODE_STRING Source);

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendPathElement(
    _In_ ULONG Flags,
    _Inout_ PRTL_UNICODE_STRING_BUFFER Buffer,
    _In_ PCUNICODE_STRING Source);

EXTERN_C_END
