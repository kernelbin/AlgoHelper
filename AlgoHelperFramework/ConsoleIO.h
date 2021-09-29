#pragma once
#include <Windows.h>

BOOL ConAttrWriteA(LPCSTR lpString, DWORD cchString, LPDWORD lpcchWritten, WORD wOldAttrMask, WORD wAttributes);

BOOL ConAttrPrintfA(WORD wOldAttrMask, WORD wAttributes, LPCSTR lpFormat, ...);
