#pragma once
#include <Windows.h>

BOOL ConsoleIOWriteWithAttribute(LPCSTR lpString, DWORD cbString, LPDWORD lpcchWritten, WORD wOldAttrMask, WORD wAttributes);

