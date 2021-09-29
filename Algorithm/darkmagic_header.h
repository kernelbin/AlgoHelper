
// this file is intended to be a .inc file. so #pragma once is not specified.

#include <Windows.h>

#ifdef _X86_
#pragma comment(linker, "/EXPORT:_main") // x86 __cdecl decoration
#elif defined(_AMD64_) || defined(_IA64_)
#pragma comment(linker, "/EXPORT:_main") // funcions are not decorated under x64
#else
#error Unsupported architecture!
#endif

