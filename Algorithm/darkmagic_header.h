
// this file is intended to be a .inc file. so #pragma once is not specified.

#define NOMINMAX
#include <Windows.h>


#ifdef _X86_
#define EXORTFUNC(f) __pragma(comment(linker, "/EXPORT:_" #f))
#elif defined(_AMD64_) || defined(_IA64_)
#define EXORTFUNC(f) __pragma(comment(linker, "/EXPORT:" #f))
#else
#error unsuportted architecture
#endif

EXORTFUNC(main)

// defines for debugging
#define D(Format, ...) if (fnConAttrPrintfA) fnConAttrPrintfA(0xF0, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY, Format, __VA_ARGS__);
#define DR(Format, ...) if (fnConAttrPrintfA) fnConAttrPrintfA(0xF0, FOREGROUND_RED, Format, __VA_ARGS__);
#define DG(Format, ...) if (fnConAttrPrintfA) fnConAttrPrintfA(0xF0, FOREGROUND_GREEN | FOREGROUND_INTENSITY, Format, __VA_ARGS__);
#define DB(Format, ...) if (fnConAttrPrintfA) fnConAttrPrintfA(0xF0, FOREGROUND_BLUE, Format, __VA_ARGS__);
#define DYL(Format, ...) if (fnConAttrPrintfA) fnConAttrPrintfA(0xF0, FOREGROUND_RED | FOREGROUND_GREEN, Format, __VA_ARGS__);
#define BKIF(x) { if(x) DebugBreak(); }



BOOL(*fnConAttrPrintfA)(WORD, WORD, LPCSTR, ...) = NULL;

void BeforeMain()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    fnConAttrPrintfA = (BOOL(*)(WORD, WORD, LPCSTR, ...))GetProcAddress(GetModuleHandle(0), "ConAttrPrintfA");
    return;
}

#pragma section(".CRT$XCV", read)
// 'i2' is guaranteed to be called after any compiler generated C++ dynamic initializer
__declspec(allocate(".CRT$XCV")) void(*fnBeforeMain)() = BeforeMain;


//#pragma section(".CRT$XCT", read)
//// 'i1' is guaranteed to be called before any compiler generated C++ dynamic initializer
//__declspec(allocate(".CRT$XCT")) int(*li)() = BeforeMain;
////