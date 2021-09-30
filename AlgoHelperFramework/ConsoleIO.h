#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL ConAttrWriteA(LPCSTR lpString, DWORD cchString, LPDWORD lpcchWritten, WORD wOldAttrMask, WORD wAttributes);

BOOL ConAttrPrintfA(WORD wOldAttrMask, WORD wAttributes, LPCSTR lpFormat, ...);

#ifdef __cplusplus
}
#endif