#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <AtlStr.h>
#include <cstdlib>
#include <ctime>

#include "RunAlgoInst.h"
#include "ConsoleIO.h"

int main()
{
    srand(time(0));
    while (1)
    {
        RUN_ALGO_INSTANCE *test = new RUN_ALGO_INSTANCE(TRUE, FALSE, L"", L"Algorithm.dll");
        if (!test->Init())
        {
            ConAttrPrintfA(0xFF, 0, "Initialization failed! can not run algorithm.\n");
            break;
        }
        if (!test->Start("main"))
        {
            ConAttrPrintfA(0xFF, 0, "Failed to start algorithm! perhaps symbol not exported\n");
            break;
        }
        test->Wait();
        
        int cchOutputLen = test->m_OutputText.GetLength();
        if (cchOutputLen && test->m_OutputText[cchOutputLen - 1] != '\n')
        {
            ConAttrPrintfA(0xF0, FOREGROUND_RED, "$\n");
        }
        DWORD64 TotalTime, UserTime, KernelTime, CpuCycle;
        test->GetRunningTime(TotalTime, KernelTime, UserTime, CpuCycle);

        ConAttrPrintfA(0xFF, 0, "\n");
        ConAttrPrintfA(0x00, BACKGROUND_GREEN | BACKGROUND_BLUE,
            "  执行结束 总用时 %lld ms  用户态 %lld ms  内核态 %lld ms  CPU周期 %lld  ", TotalTime, UserTime, KernelTime, CpuCycle);
        ConAttrPrintfA(0xFF, 0, "\n");
        delete test;
    }
}
