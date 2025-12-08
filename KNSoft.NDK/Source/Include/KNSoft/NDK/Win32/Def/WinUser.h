#pragma once

#include <WinUser.h>

#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDATA 0x49
#endif

#pragma region Resource File

/* See also: https://learn.microsoft.com/en-us/windows/win32/menurc/resources-structures */

#pragma pack(push, 1)

typedef struct _CURSORDIR
{
    WORD Width;
    WORD Height;
} CURSORDIR, *PCURSORDIR;

typedef struct _DIRENTRY
{
    WORD fontOrdinal;
} DIRENTRY, *PDIRENTRY;

typedef struct _FONTDIRENTRY
{
    WORD  dfVersion;
    DWORD dfSize;
    CHAR  dfCopyright[60];
    WORD  dfType;
    WORD  dfPoints;
    WORD  dfVertRes;
    WORD  dfHorizRes;
    WORD  dfAscent;
    WORD  dfInternalLeading;
    WORD  dfExternalLeading;
    BYTE  dfItalic;
    BYTE  dfUnderline;
    BYTE  dfStrikeOut;
    WORD  dfWeight;
    BYTE  dfCharSet;
    WORD  dfPixWidth;
    WORD  dfPixHeight;
    BYTE  dfPitchAndFamily;
    WORD  dfAvgWidth;
    WORD  dfMaxWidth;
    BYTE  dfFirstChar;
    BYTE  dfLastChar;
    BYTE  dfDefaultChar;
    BYTE  dfBreakChar;
    WORD  dfWidthBytes;
    DWORD dfDevice;
    DWORD dfFace;
    DWORD dfReserved;
    CHAR  szDeviceName[];
    // CHAR  szFaceName[];
} FONTDIRENTRY, *PFONTDIRENTRY;

typedef struct _FONTGROUPHDR
{
    WORD     NumberOfFonts;
    DIRENTRY DE[];
} FONTGROUPHDR, *PFONTGROUPHDR;

typedef struct _ICONRESDIR
{
    BYTE Width;
    BYTE Height;
    BYTE ColorCount;
    BYTE reserved;
} ICONRESDIR, *PICONRESDIR;

typedef struct _LOCALHEADER
{
    WORD xHotSpot;
    WORD yHotSpot;
} LOCALHEADER, *PLOCALHEADER;

typedef struct _MENUHEADER
{
    WORD wVersion;
    WORD cbHeaderSize;
} MENUHEADER, *PMENUHEADER;

typedef struct _MENUHELPID
{
    DWORD helpID;
} MENUHELPID, *PMENUHELPID;

typedef struct _NEWHEADER
{
    WORD Reserved;
    WORD ResType;
    WORD ResCount;
} NEWHEADER, *PNEWHEADER;

typedef struct _NORMALMENUITEM
{
    WORD    resInfo;
    WCHAR   menuText[]; // szOrOrd
} NORMALMENUITEM, *PNORMALMENUITEM;

typedef struct _POPUPMENUITEM
{
    DWORD   type;
    DWORD   state;
    DWORD   id;
    WORD    resInfo;
    WCHAR   menuText[]; // szOrOrd
} POPUPMENUITEM, *PPOPUPMENUITEM;

typedef struct _RESDIR
{
    union
    {
        ICONRESDIR   Icon;
        CURSORDIR    Cursor;
    };
    WORD       Planes;
    WORD       BitCount;
    DWORD      BytesInRes;
    WORD       IconCursorId;
} RESDIR, *PRESDIR;

typedef struct _DLGTEMPLATEEX
{
    WORD    dlgVer;
    WORD    signature;
    DWORD   helpID;
    DWORD   exStyle;
    DWORD   style;
    WORD    cDlgItems;
    short   x;
    short   y;
    short   cx;
    short   cy;
    /*
    sz_Or_Ord menu;
    sz_Or_Ord windowClass;
    WCHAR     title[titleLen];
    WORD      pointsize;
    WORD      weight;
    BYTE      italic;
    BYTE      charset;
    WCHAR     typeface[stringLen];
    */
} DLGTEMPLATEEX, *PDLGTEMPLATEEX;

DECLSPEC_ALIGN(4) typedef struct _DLGITEMTEMPLATEEX
{
    DWORD           helpID;
    DLGITEMTEMPLATE itemTemplate;
} DLGITEMTEMPLATEEX, *PDLGITEMTEMPLATEEX;

#pragma pack(pop)

#pragma endregion
