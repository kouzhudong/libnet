/*
此文主要实现iphelapi.h和netioapi.h里的一些信息获取和枚举的函数的功能例子。

此文的内容太多，已经分离出notify.h，Adapter.h，tcp.h，udp.h内容。
*/

#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
int WINAPI GetGatewayMacByIPv4(const char * IPv4, PBYTE GatewayMac);

__declspec(dllexport)
int WINAPI EnumIpNetTable2(_In_ ADDRESS_FAMILY Family);

__declspec(dllexport)
bool WINAPI GetMacByGatewayIPv6(const char * ipv6, PBYTE mac);

__declspec(dllexport)
int WINAPI GetMacByIPv4Test(int argc, char ** argv);

__declspec(dllexport)
int WINAPI GetMacByIPv4(IPAddr DestIp, PBYTE MacAddr);

__declspec(dllexport)
int WINAPI EnumUnicastIpAddressTable();

__declspec(dllexport)
void WINAPI Icmp6Test();


EXTERN_C_END
