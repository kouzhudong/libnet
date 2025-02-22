/*
功能：一些监控网络变化的代码。
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

DWORD DeRegisterWlanNotification();
DWORD RegisterWlanNotification();

void WINAPI DeRegistersNotifyAddrChange();
void WINAPI RegistersNotifyAddrChange();

void DeRegistersNotifyRouteChange();
void RegistersNotifyRouteChange();


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////


class Notify {
public:
    Notify()
    {
    }

    ~Notify()
    {
    }

    virtual void Registers() = 0;
    virtual void Deregister() = 0;

private:

};


class StableUnicastIpAddressTable {
public:
    StableUnicastIpAddressTable();
    ~StableUnicastIpAddressTable();

    void Registers();
    void Deregister();
    //PSTABLE_UNICAST_IPADDRESS_TABLE_CALLBACK CallerCallback;//这样定义不好。
    static VOID NTAPI CallerCallback(_In_ PVOID CallerContext, _In_ PMIB_UNICASTIPADDRESS_TABLE AddressTable);

private:
    HANDLE Handle = nullptr;
    PMIB_UNICASTIPADDRESS_TABLE Table = nullptr;
};


class IpInterfaceChange : public Notify {
public:
    IpInterfaceChange();
    ~IpInterfaceChange();

    void Registers();
    void Deregister();

private:
    HANDLE NotificationHandle = nullptr;

    static VOID NETIOAPI_API_ Callback(_In_ PVOID CallerContext, _In_ PMIB_IPINTERFACE_ROW Row OPTIONAL, _In_ MIB_NOTIFICATION_TYPE NotificationType);
};
