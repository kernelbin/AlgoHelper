#ifdef __cplusplus
extern "C" {
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <Windows.h>
#pragma comment(linker, "/EXPORT:_main")


int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    char str[100];
    scanf("%s", str);
    printf("hello, dll %s\n", str);
}

#ifdef __cplusplus
}

#endif