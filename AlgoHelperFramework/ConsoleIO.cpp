#include <Windows.h>
#include <cstdarg>
#include <cstdio>

BOOL ConAttrWriteA(LPCSTR lpString, DWORD cchString, LPDWORD lpcchWritten, WORD wOldAttrMask, WORD wAttributes)
{
    if (!lpString)
        return FALSE;

    if (cchString == 0)
    {
        cchString = (DWORD)strlen(lpString);
    }
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO OrigScrnBufferInfo;
    GetConsoleScreenBufferInfo(hStdOut, &OrigScrnBufferInfo);

    SetConsoleTextAttribute(hStdOut, (OrigScrnBufferInfo.wAttributes & wOldAttrMask) | wAttributes);
    BOOL bRet = WriteConsoleA(hStdOut,
        lpString, cchString, lpcchWritten, NULL);
    SetConsoleTextAttribute(hStdOut,
        OrigScrnBufferInfo.wAttributes);

    return bRet;
}

BOOL ConAttrPrintfA(WORD wOldAttrMask, WORD wAttributes, LPCSTR lpFormat, ...)
{
    BOOL bSuccess = FALSE;
    LPSTR lpFormattedStr = NULL;
    __try
    {
        va_list VarList;
        va_start(VarList, lpFormat);

        int len = _vscprintf(lpFormat, VarList);

        if (len == -1)
            __leave;

        lpFormattedStr = (LPSTR)HeapAlloc(GetProcessHeap(), 0, (len + 1) * sizeof(CHAR));
        if (!lpFormattedStr)
            __leave;

        if (vsprintf_s(lpFormattedStr, (len + 1), lpFormat, VarList) < 0)
            __leave;

        DWORD cchWritten = 0;

        if (!ConAttrWriteA(lpFormattedStr, len, &cchWritten, wOldAttrMask, wAttributes))
            __leave;

        bSuccess = TRUE;
    }
    __finally
    {
        if (lpFormattedStr) HeapFree(GetProcessHeap(), 0, lpFormattedStr);
    }
    return bSuccess;
}