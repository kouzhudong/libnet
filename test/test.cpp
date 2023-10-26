// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "..\inc\libnet.h"
#include "init.h"
#include "IpHelper.h"
#include "c.h"
#include "pch.h"
#include "WinHttp.h"


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


int _cdecl main(_In_ int argc, _In_reads_(argc) CHAR * argv[])
{
    //__debugbreak();

    setlocale(LC_CTYPE, ".936");

    int Args;
    LPWSTR * Arglist = CommandLineToArgvW(GetCommandLineW(), &Args);
    if (nullptr == Arglist) {
        printf("LastError：%d", GetLastError());
        return 0;
    }

    init();

    Exclusiveaddruse(Args, Arglist);//宽字符函数入口示例。

    //tracert(argc, argv);//单字符函数入口示例。

    //test_c();

    //TestWinHttp();  
    //TestNetworkListManager();
    //TestNetworkListManagerEvents();
    //ListenToNetworkConnectivityChangesSample(false);

    LocalFree(Arglist);
}
