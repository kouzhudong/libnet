/*
���ܣ�һЩ�������仯�Ĵ��롣
*/

#pragma once
#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


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


EXTERN_C_END
