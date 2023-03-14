// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "..\inc\libnet.h"
#include "init.h"
#include "IpHelper.h"


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
    if (NULL == Arglist) {
        printf("LastError：%d", GetLastError());
        return 0;
    }

    init();

    //EnumWfpInfo(Args, Arglist);//宽字符函数入口示例。

    //tracert(argc, argv);//单字符函数入口示例。

    //pathping(argc, argv);

    //BYTE MacAddr[6] = {0};
    //GetGatewayMacByIPv4("192.168.5.3", MacAddr);

    GetNetworkParamsTest();

    //ResolveIpNetEntry2Test("8.8.8.8");

    //BYTE mac[6];
    //GetMacByIPv6("fe80::95c9:6378:91c0:d5b2", mac);

    LocalFree(Arglist);
}
