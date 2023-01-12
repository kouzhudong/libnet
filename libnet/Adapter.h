/*
本文只要收集关于网卡（适配器的代码）。

网卡（适配器的代码）是一个基本的概念，不可忽视的知识点，必须把握。
*/

#pragma once
#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family);

__declspec(dllexport)
int WINAPI EnumAdaptersInfo();

__declspec(dllexport)
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway);

__declspec(dllexport)
int WINAPI GetGatewayByIPv6(const char * IPv6, char * Gateway);

__declspec(dllexport)
int WINAPI GetGatewayMacByIPv6(const char * IPv6, PBYTE GatewayMac);


EXTERN_C_END
