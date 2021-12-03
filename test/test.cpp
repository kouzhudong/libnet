// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "..\inc\libnet.h"


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
        //LOGA(ERROR_LEVEL, "LastError：%d", GetLastError());
        return 0;
    }

    //EnumWfpInfo(Args, Arglist);//宽字符函数入口示例。

    //GetAdaptersAddressesInfo(argc, argv);//单字符函数入口示例。

    //EnumExtendedUdpTable6();
    //EnumExtendedTcpTable(AF_INET6, TCP_TABLE_OWNER_MODULE_ALL);
    //EnumIPv4ByMasks("192.168.5.3", 24);
    EnumIPv4ByMasks("172.31.96.1", 20);
    //EnumIPv4ByMasks("1.2.3.5", 1);

    LocalFree(Arglist);
}
