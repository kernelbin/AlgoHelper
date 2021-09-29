#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <AtlStr.h>

#include "RunAlgoInst.h"
#include "ConsoleIO.h"



int main()
{
    ATL::CStringW s;
    s.Format(L"", 1);
    while (1)
    {
        RUN_ALGO_INSTANCE *test = new RUN_ALGO_INSTANCE(TRUE, FALSE, L"", L"Algorithm.dll");
        test->Init();
        test->Start("main");
        test->Wait();
        DWORD64 TotalTime, UserTime, KernelTime, CpuCycle;
        test->GetRunningTime(TotalTime, KernelTime, UserTime, CpuCycle);

        ConAttrPrintfA(0x00, BACKGROUND_BLUE, 
            "执行结束 总用时 %lld ms  用户态 %lld ms  内核态 %lld ms  CPU周期 %lld \n", TotalTime, UserTime, KernelTime, CpuCycle);
        //printf("执行完毕。")
        delete test;
    }
}
