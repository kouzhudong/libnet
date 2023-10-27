// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "IOCTL.h"
#include "html.h"
#include "IpAddr.h"


void init()
{
    HMODULE hNtDLL = GetModuleHandle(TEXT("ntdll.dll"));
    if (nullptr != hNtDLL) {
        pNtCreateFile = reinterpret_cast<P_NT_CREATE_FILE>(GetProcAddress(hNtDLL, "NtCreateFile"));
        if (nullptr == pNtCreateFile) {
            printf("没有找到NtCreateFile函数\n");
        }

        Ipv4AddressToStringW = reinterpret_cast<PRtlIpv4AddressToString>(GetProcAddress(hNtDLL, "RtlIpv4AddressToStringW"));
        if (nullptr == Ipv4AddressToStringW) {
            printf("没有找到RtlIpv4AddressToStringW函数\n");
        }

        Ipv4StringToAddressW = reinterpret_cast<RtlIpv4StringToAddressRoutine>(GetProcAddress(hNtDLL, "RtlIpv4StringToAddressW"));
        if (nullptr == Ipv4StringToAddressW) {
            printf("没有找到RtlIpv4StringToAddressW函数\n");
        }

        Ipv6AddressToStringW = reinterpret_cast<PRtlIpv6AddressToString>(GetProcAddress(hNtDLL, "RtlIpv6AddressToStringW"));
        if (nullptr == Ipv6AddressToStringW) {
            printf("没有找到RtlIpv6AddressToStringW函数\n");
        }

        Ipv6StringToAddressW = reinterpret_cast<RtlIpv6StringToAddressRoutine>(GetProcAddress(hNtDLL, "RtlIpv6StringToAddressW"));
        if (nullptr == Ipv6StringToAddressW) {
            printf("没有找到RtlIpv6StringToAddressW函数\n");
        }

        Ipv6AddressToStringA = reinterpret_cast<PRtlIpv6AddressToStringA>(GetProcAddress(hNtDLL, "RtlIpv6AddressToStringA"));
        if (nullptr == Ipv6AddressToStringA) {
            printf("没有找到RtlIpv6AddressToStringA函数\n");
        }
    }
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        setlocale(LC_CTYPE, ".936");//这个没继承进程的，否者，汉字无法显示。
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
