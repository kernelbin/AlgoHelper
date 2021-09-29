
#define _CRT_SECURE_NO_WARNINGS

#ifdef __cplusplus
#include <cstdio>
extern "C" {
#else
#include <stdio.h>
#endif


#ifdef RUN_LOCALLY
#include "darkmagic_header.h"
#else
#endif


int main()
{
#ifdef RUN_LOCALLY
    setvbuf(stdout, NULL, _IONBF, 0);
#endif



}


#ifdef __cplusplus
}
#endif