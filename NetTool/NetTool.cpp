﻿// NetTool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "..\inc\libnet.h"
#include "pch.h"


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

    printf("Usage:\r\n");                            //printf("%s.\r\n", __FUNCTION__);
    //printf("%ls install.\r\n", programName);         // 这是第一个要运行的命令



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

    //EnumWfpInfo(Args, Arglist);//宽字符函数入口示例。
    //tracert(argc, argv);//单字符函数入口示例。

    if (1 == argc) {
        Usage(Arglist[0]);
        return ret;
    }

    if (_wcsicmp(Arglist[1], L"ping") == 0) {
        ping(argc--, argv++);
    }

    //else if (_wcsicmp(Arglist[1], L"pathping") == 0) {
    //    pathping(argc--, argv++);
    //}

    //else if (_wcsicmp(Arglist[1], L"tracert") == 0) {
    //    tracert(argc--, argv++);
    //}

    else {
        Usage(Arglist[0]);
    }

    LocalFree(Arglist);

    return ret;
}

