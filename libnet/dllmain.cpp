// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "log.h"
#include "ioctl.h"
#include "html.h"
#include "IpAddr.h"


void init()
{
    setlocale(LC_CTYPE, ".936");
    InitializeCriticalSection(&g_log_cs);
}


void cleanup()
{
    DeleteCriticalSection(&g_log_cs);
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
        cleanup();
        break;
    }

    return TRUE;
}
