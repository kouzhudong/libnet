/*
此文主要实现套接字的一些信息获取和枚举的函数的功能例子。
*/

#pragma once


#define MAX_ADDRESS_STRING_LENGTH   64


//  Asynchronous query context structure.
typedef struct _QueryContext {
    OVERLAPPED      QueryOverlapped;
    PADDRINFOEX     QueryResults;
    HANDLE          CompleteEvent;
}QUERY_CONTEXT, * PQUERY_CONTEXT;


int __cdecl getaddrinfo(int argc, char ** argv);
int __cdecl getaddrinfo2(int argc, char ** argv);

int __cdecl GetAddrInfoW(int argc, wchar_t ** argv);

int __cdecl GetAddrInfoExA(int argc, wchar_t ** argv);
int __cdecl GetAddrInfoExA2(_In_ int Argc, PWCHAR Argv[]);

int __cdecl GetAddrInfoExW(int argc, wchar_t ** argv);
int __cdecl GetAddrInfoExW2(_In_ int Argc, PWCHAR Argv[]);

int __cdecl getnameinfo(int argc, char ** argv);
int __cdecl GetNameInfoW(int argc, char ** argv);

int gethostbyaddr(int argc, char ** argv);

int gethostbyname(int argc, char ** argv);
