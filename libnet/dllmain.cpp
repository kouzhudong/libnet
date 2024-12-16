// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "ioctl.h"
#include "html.h"
#include "IpAddr.h"


void init()
{
    setlocale(LC_CTYPE, ".936"); //这个没继承进程的，否者，汉字无法显示。
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        init();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
