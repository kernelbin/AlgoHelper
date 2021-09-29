#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <AtlStr.h>

#include "RunAlgoInst.h"
//写一个wrapper函数
//创建两个线程，一个负责call dll，一个负责写输入，读输出。
//给个选项，用于表明是否把输出重定向到screen（还是缓存起来），以及是否重定向输入。给个选项时间限制。
//再准备一个（全局的）线程，上计时器。到了时间就cancelioex, 关线程卸dll。
//捕获一下异常，遇到异常先反馈再不处理放行给Visual Studio
//
//然后可以先写个没GUI的版本


int main()
{
    while (1)
    {
        RUN_ALGO_INSTANCE *test = new RUN_ALGO_INSTANCE(TRUE, FALSE, L"", L"Algorithm.dll");
        test->Init();
        test->Start("main");
        test->Wait();
        delete test;
    }
}
