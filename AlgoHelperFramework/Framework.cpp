#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <AtlStr.h>

#include "RunAlgoInst.h"
//дһ��wrapper����
//���������̣߳�һ������call dll��һ������д���룬�������
//����ѡ����ڱ����Ƿ������ض���screen�����ǻ������������Լ��Ƿ��ض������롣����ѡ��ʱ�����ơ�
//��׼��һ����ȫ�ֵģ��̣߳��ϼ�ʱ��������ʱ���cancelioex, ���߳�жdll��
//����һ���쳣�������쳣�ȷ����ٲ�������и�Visual Studio
//
//Ȼ�������д��ûGUI�İ汾


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
