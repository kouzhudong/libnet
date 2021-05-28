/*
������Ҫʵ��iphelapi.h��netioapi.h���һЩ��Ϣ��ȡ��ö�ٵĺ����Ĺ������ӡ�
*/

#pragma once


int GetTcpTable();
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
int WINAPI GetAdaptersAddressesInfo(int argc, char ** argv);

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


EXTERN_C_END
