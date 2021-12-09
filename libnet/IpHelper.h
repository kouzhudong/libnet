/*
������Ҫʵ��iphelapi.h��netioapi.h���һЩ��Ϣ��ȡ��ö�ٵĺ����Ĺ������ӡ�

���ĵ�����̫�࣬�Ѿ������notify.h��Adapter.h���ݡ�
*/

#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway);

__declspec(dllexport)
int WINAPI GetGatewayMacByIPv4(const char * IPv4, PBYTE GatewayMac);

__declspec(dllexport)
int WINAPI EnumIpNetTable2(_In_ ADDRESS_FAMILY Family);

__declspec(dllexport)
int WINAPI RunEstats();

__declspec(dllexport)
int WINAPI GetMacByIPv4Test(int argc, char ** argv);

__declspec(dllexport)
int WINAPI GetMacByIPv4(IPAddr DestIp, PBYTE MacAddr);

__declspec(dllexport)
int WINAPI EnumUnicastIpAddressTable();

__declspec(dllexport)
void WINAPI Icmp6Test();


EXTERN_C_END
