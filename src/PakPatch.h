#include "PakFile.h"

DWORD resources_pak_size = 0;

HANDLE resources_pak_map = NULL;

typedef HANDLE(WINAPI *pMapViewOfFile)(
    _In_ HANDLE hFileMappingObject,
    _In_ DWORD  dwDesiredAccess,
    _In_ DWORD  dwFileOffsetHigh,
    _In_ DWORD  dwFileOffsetLow,
    _In_ SIZE_T dwNumberOfBytesToMap
    );

pMapViewOfFile RawMapViewOfFile = NULL;

HANDLE WINAPI MyMapViewOfFile(
    _In_ HANDLE hFileMappingObject,
    _In_ DWORD  dwDesiredAccess,
    _In_ DWORD  dwFileOffsetHigh,
    _In_ DWORD  dwFileOffsetLow,
    _In_ SIZE_T dwNumberOfBytesToMap
)
{
    if (hFileMappingObject == resources_pak_map)
    {
        // 修改属性为可修改
        LPVOID buffer = RawMapViewOfFile(hFileMappingObject, FILE_MAP_COPY, dwFileOffsetHigh,
            dwFileOffsetLow, dwNumberOfBytesToMap);

        // 不再需要hook
        resources_pak_map = NULL;
        MH_DisableHook(MapViewOfFile);

        if (buffer)
        {
            // 遍历gzip文件
            TraversalGZIPFile((BYTE*)buffer, [=](uint8_t *begin, uint32_t size, uint32_t &new_len) {
                bool changed = false;

                BYTE search_start[] = R"(</settings-about-page>)";
                uint8_t* pos = memmem(begin, size, search_start, sizeof(search_start) - 1);
                if (pos)
                {

                    // 压缩HTML以备写入补丁信息
                    std::string html((char*)begin, size);
                    compression_html(html);

                    // RemoveUpdateError
					//if (IsNeedPortable())
					{
						ReplaceStringInPlace(html, R"(hidden="[[!showUpdateStatus_]]")", R"(hidden="true")");
						ReplaceStringInPlace(html, R"(hidden="[[!shouldShowIcons_(showUpdateStatus_)]]")", R"(hidden="true")");
					}

                    const char prouct_title[] = u8R"({aboutBrowserVersion}</div><div class="secondary"><a target="_blank" href="https://github.com/shuax/chrome_plus">Chrome++</a> )" RELEASE_VER_STR u8R"( inside</div>)";
                    ReplaceStringInPlace(html, R"({aboutBrowserVersion}</div>)", prouct_title);

                    if (html.length() <= size)
                    {
                        // 写入修改
                        memcpy(begin, html.c_str(), html.length());

                        // 修改长度
                        new_len = html.length();
                        changed = true;
                    }
                }

                return changed;
            });

        }

        return buffer;
    }

    return RawMapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
        dwFileOffsetLow, dwNumberOfBytesToMap);
}

HANDLE resources_pak_file = NULL;

typedef HANDLE(WINAPI *pCreateFileMapping)(
    _In_     HANDLE                hFile,
    _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes,
    _In_     DWORD                 flProtect,
    _In_     DWORD                 dwMaximumSizeHigh,
    _In_     DWORD                 dwMaximumSizeLow,
    _In_opt_ LPCTSTR               lpName
    );

pCreateFileMapping RawCreateFileMapping = NULL;

HANDLE WINAPI MyCreateFileMapping(
    _In_     HANDLE                hFile,
    _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes,
    _In_     DWORD                 flProtect,
    _In_     DWORD                 dwMaximumSizeHigh,
    _In_     DWORD                 dwMaximumSizeLow,
    _In_opt_ LPCTSTR               lpName
)
{
    if (hFile == resources_pak_file)
    {
        // 修改属性为可修改
        resources_pak_map = RawCreateFileMapping(hFile, lpAttributes, PAGE_WRITECOPY,
            dwMaximumSizeHigh, dwMaximumSizeLow, lpName);

        // 不再需要hook
        resources_pak_file = NULL;
        MH_DisableHook(CreateFileMappingW);

        if (MH_CreateHook(MapViewOfFile, MyMapViewOfFile, (LPVOID*)&RawMapViewOfFile) == MH_OK)
        {
            MH_EnableHook(MapViewOfFile);
        }

        return resources_pak_map;
    }
    return RawCreateFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh,
        dwMaximumSizeLow, lpName);
}

typedef HANDLE(WINAPI *pCreateFile)(
    _In_     LPCTSTR               lpFileName,
    _In_     DWORD                 dwDesiredAccess,
    _In_     DWORD                 dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_     DWORD                 dwCreationDisposition,
    _In_     DWORD                 dwFlagsAndAttributes,
    _In_opt_ HANDLE                hTemplateFile
    );

pCreateFile RawCreateFile = NULL;

HANDLE WINAPI MyCreateFile(
    _In_     LPCTSTR               lpFileName,
    _In_     DWORD                 dwDesiredAccess,
    _In_     DWORD                 dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_     DWORD                 dwCreationDisposition,
    _In_     DWORD                 dwFlagsAndAttributes,
    _In_opt_ HANDLE                hTemplateFile
)
{
    HANDLE file = RawCreateFile(lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,
        hTemplateFile);

    if (isEndWith(lpFileName, L"resources.pak"))
    {
        resources_pak_file = file;
        resources_pak_size = GetFileSize(resources_pak_file, NULL);

        if (MH_CreateHook(CreateFileMappingW, MyCreateFileMapping, (LPVOID*)&RawCreateFileMapping) == MH_OK)
        {
            MH_EnableHook(CreateFileMappingW);
        }

        // 不再需要hook
        MH_DisableHook(CreateFileW);
    }

    return file;
}


void PakPatch()
{
    MH_STATUS status = MH_CreateHook(CreateFileW, MyCreateFile, (LPVOID*)&RawCreateFile);
    if (status == MH_OK)
    {
        MH_EnableHook(CreateFileW);
    }
    else
    {
        DebugLog(L"MH_CreateHook CreateFileW failed:%d", status);
    }
}
