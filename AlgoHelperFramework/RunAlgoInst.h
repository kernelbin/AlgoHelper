#pragma once

#include <Windows.h>
#include <AtlStr.h>

typedef int (*ALGORITHM_FUNC)();

struct RUN_ALGO_INSTANCE
{
    BOOL m_bPrintToScreen;
    BOOL m_bRedirectInput;
    ATL::CStringW m_InputText;
    ATL::CStringW m_DllName;

    HMODULE m_hDll;

    HANDLE m_hThreadRunAlgo;
    HANDLE m_hThreadWrite;
    HANDLE m_hThreadRead;

    HANDLE m_hPipeOutRead, m_hPipeOutWrite, m_hPipeInRead, m_hPipeInWrite;

    ATL::CStringA m_OutputText;

    RUN_ALGO_INSTANCE(BOOL bPrintToScreen, BOOL bRedirectInput, ATL::CStringW InputText, ATL::CStringW DllName);
    BOOL Init();

    BOOL Start(LPCSTR lpFuncName);
    BOOL ForceStop();
    BOOL Wait();

    BOOL GetRunningTime(
        DWORD64& RunningTimems,
        DWORD64& KernelTimems,
        DWORD64& UserTimems,
        DWORD64& CycleTime);

    ~RUN_ALGO_INSTANCE();
};
