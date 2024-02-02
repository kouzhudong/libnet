// NetTool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "..\inc\libnet.h"
#include "pch.h"
#include "IPArp.h"
#include "whois.h"
#include "ping.h"
#include "pathpings.h"
#include "tracert.h"
#include "IPRoute.h"
#include "IPConfig.h"
#include "wfp.h"
#include "spi.h"
#include "NetBIOS.h"
#include "nslookup.h"


#ifdef _WIN64  
#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\libnet.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\libnet.lib")
#endif
#else 
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\libnet.lib")
#else
#pragma comment(lib, "..\\Release\\libnet.lib")
#endif
#endif


void banner()
{
    printf("made by correy.\r\n");
    printf("https://github.com/kouzhudong.\r\n");
    printf("Copyright (C) Correy Studio. All rights reserved.\r\n");
    printf("\r\n");
}


void Usage(_In_ LPCWSTR programName)
{
    banner();

    printf("Usage:\r\n");                 //printf("%s.\r\n", __FUNCTION__);
    printf("%ls ping.\r\n", programName); 
    printf("%ls pathping.\r\n", programName);
    printf("%ls tracert.\r\n", programName);



}


void help()
{
    banner();

 
}


void init()
{
 
}


int _cdecl main(_In_ int argc, _In_reads_(argc) CHAR * argv[])
{
    //__debugbreak();

    int ret = ERROR_SUCCESS;

    setlocale(LC_CTYPE, ".936");

    int Args;
    LPWSTR * Arglist = CommandLineToArgvW(GetCommandLineW(), &Args);
    if (nullptr == Arglist) {
        printf("LastError：%d", GetLastError());
        return 0;
    }

    if (1 == argc) {
        Usage(Arglist[0]);
        return ret;
    }

    if (_wcsicmp(Arglist[1], L"ping") == 0) {
        ping(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"pathping") == 0) {
        pathping(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"tracert") == 0) {
        tracert(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"whois") == 0) {
        whois(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"Arp") == 0) {
        IpArp(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"Route") == 0) {
        IpRoute(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"Ipconfig") == 0) {
        Ipconfig();
    }

    else if (_wcsicmp(Arglist[1], L"wfp") == 0) {
        EnumWfpInfo(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"spi") == 0) {
        EnumSpiInfo(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"nbtstat") == 0) {
        //nbtstat(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"netstat") == 0) {
        //netstat(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"netsh") == 0) {
        //netsh(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"finger") == 0) {
        //finger(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"nslookup") == 0) {
        nslookup(--argc, ++argv);
    }

    else if (_wcsicmp(Arglist[1], L"wlan") == 0) {
        //wlan(--argc, ++Arglist);
    }

    else if (_wcsicmp(Arglist[1], L"net") == 0) {
        //net(--argc, ++Arglist);
    }

    else {
        Usage(Arglist[0]);
    }

    LocalFree(Arglist);

    return ret;
}
