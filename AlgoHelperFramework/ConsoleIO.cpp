#include <Windows.h>

BOOL ConsoleIOWriteWithAttribute(LPCSTR lpString, DWORD cbString, LPDWORD lpcchWritten, WORD wOldAttrMask, WORD wAttributes)
{
	if (!lpString)
		return FALSE;

	if (cbString == 0)
	{
		cbString = (DWORD)strlen(lpString);
	}
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO OrigScrnBufferInfo;
	GetConsoleScreenBufferInfo(hStdOut, &OrigScrnBufferInfo);

	SetConsoleTextAttribute(hStdOut, (OrigScrnBufferInfo.wAttributes & wOldAttrMask) | wAttributes);
	BOOL bRet = WriteConsoleA(hStdOut,
		lpString, cbString, lpcchWritten, NULL);
	SetConsoleTextAttribute(hStdOut,
		OrigScrnBufferInfo.wAttributes);

	return bRet;
}