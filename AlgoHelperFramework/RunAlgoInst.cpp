#include <Windows.h>
#include <process.h>
#include <cstdlib>

#include "RunAlgoInst.h"
#include "ConsoleIO.h"

#define CloseHandleNullIf(x) if(x) { CloseHandle(x); x = NULL; }
#define PIPE_SIZE 4096

SRWLOCK DllLoaderLock = SRWLOCK_INIT;


static BOOL CreateOneByteBlockNamedPipePair(PHANDLE hReadPipe, PHANDLE hWritePipe)
{
    WCHAR lpszPipename[120] = L"\\\\.\\pipe\\";
    for (int i = 0; i < 32; i++)
    {
        lpszPipename[i + 9] = rand() % 26 + L'a';
    }
    (*hReadPipe) = CreateNamedPipeW(
        lpszPipename,             // pipe name 
        PIPE_ACCESS_INBOUND |      // read/write access 
        0,     // overlapped mode 
        PIPE_TYPE_BYTE |       // byte-type pipe 
        PIPE_READMODE_BYTE |   // byte read mode 
        PIPE_WAIT,                // blocking mode 
        PIPE_UNLIMITED_INSTANCES, // unlimited instances 
        0,    // output buffer size 
        0,    // input buffer size 
        0,             // client time-out 
        NULL);
    if ((!(*hReadPipe)) || ((*hReadPipe) == INVALID_HANDLE_VALUE))
    {
        (*hReadPipe) = (*hWritePipe) = 0;
        return FALSE;
    }
    (*hWritePipe) = CreateFileW(lpszPipename, GENERIC_WRITE,
        0,              // no sharing 
        NULL,           // default security attributes
        OPEN_EXISTING,  // opens existing pipe 
        0,              // default attributes 
        NULL);          // no template file 
    if ((!(*hWritePipe)) || ((*hWritePipe) == INVALID_HANDLE_VALUE))
    {
        CloseHandle((*hReadPipe));
        (*hReadPipe) = (*hWritePipe) = 0;
        return FALSE;
    }
    return TRUE;
}



RUN_ALGO_INSTANCE::RUN_ALGO_INSTANCE(
    BOOL bPrintToScreen,
    BOOL bRedirectInput,
    ATL::CStringW InputText,
    ATL::CStringW DllName) :
    m_bPrintToScreen(bPrintToScreen),
    m_bRedirectInput(bRedirectInput),
    m_InputText(InputText),
    m_DllName(DllName),
    m_hDll(NULL),
    m_hThreadRunAlgo(NULL),
    m_hThreadWrite(NULL),
    m_hThreadRead(NULL),
    m_hPipeOutRead(NULL),
    m_hPipeOutWrite(NULL),
    m_hPipeInRead(NULL),
    m_hPipeInWrite(NULL),
    m_OutputText("")
{
    ;
}

RUN_ALGO_INSTANCE::~RUN_ALGO_INSTANCE()
{
    CloseHandleNullIf(m_hThreadRunAlgo);
    CloseHandleNullIf(m_hThreadRead);
    CloseHandleNullIf(m_hThreadWrite);

    CloseHandleNullIf(m_hPipeInRead);
    CloseHandleNullIf(m_hPipeInWrite);
    CloseHandleNullIf(m_hPipeOutRead);
    CloseHandleNullIf(m_hPipeOutWrite);

    if (m_hDll) FreeLibrary(m_hDll);
}
unsigned __stdcall RunAlgoFuncThread(void* lpfnAlgo)
{
    ALGORITHM_FUNC FuncAddr = (ALGORITHM_FUNC)lpfnAlgo;
    FuncAddr();
    return 0;
}

unsigned __stdcall IoWriteThread(void* pRunAlgoInstance)
{
    RUN_ALGO_INSTANCE* pRunAlgoInst = (RUN_ALGO_INSTANCE*)pRunAlgoInstance;
    LPSTR lpInput = NULL;
    __try
    {// convert the string to current used codepage, and write it through the pipe.

        int cbInput = WideCharToMultiByte(CP_ACP, 0, pRunAlgoInst->m_InputText,
            pRunAlgoInst->m_InputText.GetLength(), NULL, 0, NULL, NULL);
        if (cbInput == 0)
            __leave;

        lpInput = (LPSTR)HeapAlloc(GetProcessHeap(), NULL, cbInput * sizeof(CHAR));
        if (!lpInput)
            __leave;

        cbInput = WideCharToMultiByte(CP_ACP, 0, pRunAlgoInst->m_InputText,
            pRunAlgoInst->m_InputText.GetLength(), lpInput, cbInput, NULL, NULL);
        if (cbInput == 0)
            __leave;

        for (DWORD cbWritten = 0; cbWritten < cbInput; )
        {
            DWORD dwBytesWritten = 0;
            BOOL bRet = WriteFile(
                pRunAlgoInst->m_hPipeInWrite,
                lpInput + cbWritten,
                cbInput - cbWritten,
                &dwBytesWritten,
                NULL);
            if (!bRet)
                __leave;

            cbWritten += dwBytesWritten;
        }
    }
    __finally
    {
        if (lpInput)
        {
            HeapFree(GetProcessHeap(), 0, (LPVOID)lpInput);
        }
    }

    return 0;
}

unsigned __stdcall IoReadThread(void* pRunAlgoInstance)
{
    // TODO: error logging for this entire function
    RUN_ALGO_INSTANCE* pRunAlgoInst = (RUN_ALGO_INSTANCE*)pRunAlgoInstance;

    BYTE ReadBuffer[512];
    while (1)
    {
        // First read 0 bytes from the pipe, in order to wait for data.
        // After that, peek the pipe, write the content to console, then read it.
        // so we can ensure that the write end won't return before we write it.
        DWORD dwZeroBytesRead;
        if (!ReadFile(pRunAlgoInst->m_hPipeOutRead, ReadBuffer, 0, &dwZeroBytesRead, NULL))
            break;
        DWORD dwBytesPeek, dwTotal, dwLeft;
        BOOL bRet = PeekNamedPipe(pRunAlgoInst->m_hPipeOutRead, ReadBuffer, _countof(ReadBuffer), &dwBytesPeek, &dwTotal, &dwLeft);
        if (bRet && dwBytesPeek == 0) // no data to read. try later.
        {
            continue;
        }
        if (!bRet) // failed
            break;

        pRunAlgoInst->m_OutputText.Append((LPCSTR)ReadBuffer, dwBytesPeek);
        if (pRunAlgoInst->m_bPrintToScreen)
        {
            DWORD cchWritten;
            ConAttrWriteA((LPCSTR)ReadBuffer, dwBytesPeek, &cchWritten, 0xF0, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        }

        DWORD dwBytesRead;
        bRet = ReadFile(pRunAlgoInst->m_hPipeOutRead, ReadBuffer, dwBytesPeek, &dwBytesRead, NULL);
        if (!bRet) // failed
            break;
        if (dwBytesPeek != dwBytesRead)
        {
            // something's wrong. no idea what's happened.
            break;
        }
    }
    return 0;
}

BOOL RUN_ALGO_INSTANCE::Init()
{
    BOOL bSuccess = FALSE;
    AcquireSRWLockExclusive(&DllLoaderLock);

    m_hPipeOutRead = m_hPipeOutWrite = m_hPipeInRead = m_hPipeInWrite = NULL;
    HANDLE hOldStdOut = NULL, hOldStdIn = NULL;
    __try
    {
        // Create pipes and redirect input / output.
        // Output is always redirected, and input is redirected only if bRedirectInput.
        if (!CreateOneByteBlockNamedPipePair(&m_hPipeOutRead, &m_hPipeOutWrite))
            __leave;
        if ((hOldStdOut = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE) __leave;
        if (!SetStdHandle(STD_OUTPUT_HANDLE, m_hPipeOutWrite))
            __leave;

        if (m_bRedirectInput)
        {
            if (!CreatePipe(&m_hPipeInRead, &m_hPipeInWrite, NULL, PIPE_SIZE))
                __leave;
            if ((hOldStdIn = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE) __leave;
            SetStdHandle(STD_INPUT_HANDLE, m_hPipeInRead);
        }

        // Load the dll
        if ((m_hDll = LoadLibraryW(m_DllName)) == NULL)
            __leave;

        bSuccess = TRUE;
    }
    __finally
    {
        if (!bSuccess)
        {
            CloseHandleNullIf(m_hPipeInRead);
            CloseHandleNullIf(m_hPipeInWrite);
            CloseHandleNullIf(m_hPipeOutRead);
            CloseHandleNullIf(m_hPipeOutWrite);
            if (m_hDll)
            {
                FreeLibrary(m_hDll);
                m_hDll = NULL;
            }
        }
        // Restore stdin/out
        if (hOldStdOut) SetStdHandle(STD_OUTPUT_HANDLE, hOldStdOut);
        if (hOldStdIn) SetStdHandle(STD_INPUT_HANDLE, hOldStdIn);
        ReleaseSRWLockExclusive(&DllLoaderLock);
    }
    return bSuccess;
}

BOOL RUN_ALGO_INSTANCE::Start(LPCSTR lpFuncName)
{
    BOOL bSuccess = FALSE;
    __try
    {
        ALGORITHM_FUNC FuncAddr;
        if (!(FuncAddr = (ALGORITHM_FUNC)GetProcAddress(m_hDll, lpFuncName)))
            __leave;

        // Create Threads in order to run algorithm thread and IO with that thread.
        m_hThreadRunAlgo = (HANDLE)_beginthreadex(NULL, 0, RunAlgoFuncThread, (void*)FuncAddr, 0, NULL);
        if (m_hThreadRunAlgo == INVALID_HANDLE_VALUE)
            __leave;
        if (m_bRedirectInput)
        {
            m_hThreadWrite = (HANDLE)_beginthreadex(NULL, 0, IoWriteThread, (void*)this, 0, NULL);
            if (m_hThreadWrite == INVALID_HANDLE_VALUE)
                __leave;
        }
        m_hThreadRead = (HANDLE)_beginthreadex(NULL, 0, IoReadThread, (void*)this, 0, NULL);
        if (m_hThreadRead == INVALID_HANDLE_VALUE)
            __leave;

        bSuccess = TRUE;
    }
    __finally
    {
        if (!bSuccess)
        {
            if (m_hThreadRunAlgo)
            {
                TerminateThread(m_hThreadRunAlgo, 0);
                CloseHandle(m_hThreadRunAlgo);
                m_hThreadRunAlgo = NULL;
            }
            if (m_hThreadWrite)
            {
                TerminateThread(m_hThreadWrite, 0);
                CloseHandle(m_hThreadWrite);
                m_hThreadWrite = NULL;
            }
            if (m_hThreadRead)
            {
                TerminateThread(m_hThreadRead, 0);
                CloseHandle(m_hThreadRead);
                m_hThreadRead = NULL;
            }
        }
    }

    return bSuccess;
}

BOOL RUN_ALGO_INSTANCE::ForceStop()
{
    BOOL bSuccess = TRUE;
    if (m_hThreadRunAlgo)
    {
        bSuccess &= TerminateThread(m_hThreadRunAlgo, 0);
    }
    if (m_hThreadWrite)
    {
        bSuccess &= TerminateThread(m_hThreadWrite, 0);
    }
    if (m_hThreadRead)
    {
        bSuccess &= TerminateThread(m_hThreadRead, 0);
    }

    return bSuccess;
}

BOOL RUN_ALGO_INSTANCE::Wait()
{
    HANDLE WaitHandles[2] = { m_hThreadRead, m_hThreadWrite };
    WaitForSingleObject(m_hThreadRunAlgo, INFINITE);

    BOOL x = FlushFileBuffers(m_hPipeOutWrite);
    CloseHandleNullIf(m_hPipeOutWrite); // tell the read thread that the pipe is ended.

    WaitForMultipleObjects(m_hThreadWrite == NULL ? 1 : 2, WaitHandles, TRUE, INFINITE);

    return TRUE;
}

DWORD64 FileTime2Millisecond(const FILETIME& ft)
{
    LARGE_INTEGER li;
    li.HighPart = ft.dwHighDateTime;
    li.LowPart = ft.dwLowDateTime;
    return li.QuadPart / 10000;
}

BOOL RUN_ALGO_INSTANCE::GetRunningTime(
    DWORD64& RunningTimems,
    DWORD64& KernelTimems,
    DWORD64& UserTimems,
    DWORD64& CycleTime)
{
    if (!m_hThreadRunAlgo) return FALSE;

    if (WaitForSingleObject(m_hThreadRunAlgo, 0) == WAIT_OBJECT_0)
    {
        FILETIME ftCreation, ftExit, ftKernel, ftUser;
        if (!GetThreadTimes(m_hThreadRunAlgo, &ftCreation, &ftExit, &ftKernel, &ftUser))
            return FALSE;

        DWORD64 ulCycleTime;
        if (!QueryThreadCycleTime(m_hThreadRunAlgo, &ulCycleTime))
            return FALSE;

        RunningTimems = FileTime2Millisecond(ftExit) - FileTime2Millisecond(ftCreation);
        KernelTimems = FileTime2Millisecond(ftKernel);
        UserTimems = FileTime2Millisecond(ftUser);
        CycleTime = ulCycleTime;
        return TRUE;
    }
    return FALSE;
}

