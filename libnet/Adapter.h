/*
����ֻҪ�ռ������������������Ĵ��룩��

�������������Ĵ��룩��һ�������ĸ�����ɺ��ӵ�֪ʶ�㣬������ա�
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


EXTERN_C_END
