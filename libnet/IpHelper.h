/*
此文主要实现iphelapi.h和netioapi.h里的一些信息获取和枚举的函数的功能例子。
*/

#pragma once


int GetTcpTable2();

int GetTcp6Table();
int GetTcp6Table2();

int GetTcpStatistics();

int GetExtendedTcpTable();

int GetUdpTable();
int GetUdp6Table();

int GetExtendedUdpTable4();
int GetExtendedUdpTable6();

int __cdecl GetIpAddrTable();

int __cdecl GetAdaptersInfo();

int GetIpNetTable();
int GetIpNetTable2();

int GetIpForwardTable();
int GetIpForwardTable2();

int GetIfTable();
int __cdecl GetIfTable2(int argc, char ** argv);
int __cdecl GetIfTable2Ex(int argc, char ** argv);


EXTERN_C_START


__declspec(dllexport)
int WINAPI EnumTcpTable();

__declspec(dllexport)
int WINAPI EnumExtendedTcp4TableByPid();

__declspec(dllexport)
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass);

__declspec(dllexport)
int WINAPI EnumExtendedUdpTable4();

__declspec(dllexport)
int WINAPI EnumExtendedUdpTable6();

__declspec(dllexport)
int WINAPI EnumdaptersInfo();

__declspec(dllexport)
int WINAPI EnumAdaptersAddressesInfo(int argc, char ** argv);

__declspec(dllexport)
void WINAPI RegistersNotifyIpInterfaceChange();

__declspec(dllexport)
void WINAPI DeregisterNotifyIpInterfaceChange();

__declspec(dllexport)
void WINAPI RegistersNotifyNetworkConnectivityHintChange();

__declspec(dllexport)
void WINAPI DeregisterNotifyRouteChange2();

__declspec(dllexport)
void WINAPI RegistersNotifyRouteChange2();

__declspec(dllexport)
void WINAPI DeregisterNotifyStableUnicastIpAddressTable();

__declspec(dllexport)
void WINAPI RegistersNotifyStableUnicastIpAddressTable();

__declspec(dllexport)
void WINAPI DeregisterNotifyTeredoPortChange();

__declspec(dllexport)
void WINAPI RegistersNotifyTeredoPortChange();

__declspec(dllexport)
void WINAPI DeregisterNotifyUnicastIpAddressChange();

__declspec(dllexport)
void WINAPI RegistersNotifyUnicastIpAddressChange();

__declspec(dllexport)
int WINAPI RunEstats();

__declspec(dllexport)
int WINAPI GetMacByIPv4Test(int argc, char ** argv);

__declspec(dllexport)
int WINAPI GetMacByIPv4(IPAddr DestIp, PBYTE MacAddr);


EXTERN_C_END
