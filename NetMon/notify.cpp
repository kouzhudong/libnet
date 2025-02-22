#include "pch.h"
#include "notify.h"


#pragma warning(disable:4189) //局部变量已初始化但不引用


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID NETIOAPI_API_ NetworkConnectivityHintChange(_In_ PVOID CallerContext, _In_ NL_NETWORK_CONNECTIVITY_HINT ConnectivityHint)
/*
注册的过程中会被调用1次。

如何触发？使这里被运行呢？
1.关闭和开启网卡也不行。
2.改变IP地址也不行。
3.关闭和开启WIFI会走这里。
*/
{
    UNREFERENCED_PARAMETER(CallerContext);

    printf("ConnectivityLevel:%d, ConnectivityCost:%d, ApproachingDataLimit:%d, OverDataLimit:%d, Roaming:%d\r\n",
           ConnectivityHint.ConnectivityLevel,
           ConnectivityHint.ConnectivityCost,
           ConnectivityHint.ApproachingDataLimit,
           ConnectivityHint.OverDataLimit,
           ConnectivityHint.Roaming);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyNetworkConnectivityHintChange()
/*
Registers an application-defined callback function,
to be called when the aggregate network connectivity level and cost hints change.

这个也没有反注册的函数？没找到。

Minimum supported client	Windows 10, version 2004 (10.0; Build 19041)
Minimum supported server	Windows Server, version 2004 (10.0; Build 19041)

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifynetworkconnectivityhintchange
*/
{
    HANDLE NotificationHandle = nullptr;

    NTSTATUS Status = NotifyNetworkConnectivityHintChange(NetworkConnectivityHintChange, nullptr, TRUE, &NotificationHandle);
    //(void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotificationHandle = nullptr;


VOID NETIOAPI_API_ IpInterfaceChange(_In_ PVOID CallerContext, _In_ PMIB_IPINTERFACE_ROW Row OPTIONAL, _In_ MIB_NOTIFICATION_TYPE NotificationType)
/*
注册的过程中会被调用几次，NotificationType == MibInitialNotification。

如何触发？使这里被运行呢？
1.关闭和开启网卡也不行。
2.改变IP地址也不行。

手动添加一个网络，如：连接手机的网络。可以收到信息。
手动删除一个网络，如：禁用手机的连接。可以收到信息。
*/
{
    UNREFERENCED_PARAMETER(CallerContext);

    switch (NotificationType) {
    case MibParameterNotification:
        printf("A parameter was changed, Family:%u.\r\n", Row->Family);
        break;
    case MibAddInstance:
        printf("A new MIB instance was added, Family:%u.\r\n", Row->Family);
        break;
    case MibDeleteInstance:
        printf("An existing MIB instance was deleted, Family:%u.\r\n", Row->Family);
        break;
    case MibInitialNotification:
        printf("MibInitialNotification.\r\n");
        //此时：Row == nullptr;
        break;
    default:
        printf("NotificationType:%d.\r\n", NotificationType);
        break;
    }

    printf("\r\n");
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyIpInterfaceChange()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotificationHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyIpInterfaceChange()
/*
The NotifyIpInterfaceChange function registers to be notified for changes to all IP interfaces,
IPv4 interfaces, or IPv6 interfaces on a local computer.

netsh interface ipv4 show interface

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyipinterfacechange
*/
{
    NTSTATUS Status = NotifyIpInterfaceChange(AF_UNSPEC, IpInterfaceChange, nullptr, TRUE, &NotificationHandle);

    //(void)getchar();
}


IpInterfaceChange::IpInterfaceChange()
{
}

IpInterfaceChange::~IpInterfaceChange()
{
}

void IpInterfaceChange::Registers()
{
    NTSTATUS Status = NotifyIpInterfaceChange(AF_UNSPEC, Callback, nullptr, TRUE, &NotificationHandle);
}

void IpInterfaceChange::Deregister()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotificationHandle);
}

VOID NETIOAPI_API_ IpInterfaceChange::Callback(_In_ PVOID CallerContext, _In_ PMIB_IPINTERFACE_ROW Row OPTIONAL, _In_ MIB_NOTIFICATION_TYPE NotificationType)
/*
注册的过程中会被调用几次，NotificationType == MibInitialNotification。

如何触发？使这里被运行呢？
1.关闭和开启网卡也不行。
2.改变IP地址也不行。

手动添加一个网络，如：连接手机的网络。可以收到信息。
手动删除一个网络，如：禁用手机的连接。可以收到信息。
*/
{
    UNREFERENCED_PARAMETER(CallerContext);

    switch (NotificationType) {
    case MibParameterNotification:
        printf("A parameter was changed, Family:%u.\r\n", Row->Family);
        break;
    case MibAddInstance:
        printf("A new MIB instance was added, Family:%u.\r\n", Row->Family);
        break;
    case MibDeleteInstance:
        printf("An existing MIB instance was deleted, Family:%u.\r\n", Row->Family);
        break;
    case MibInitialNotification:
        printf("MibInitialNotification.\r\n");
        //此时：Row == nullptr;
        break;
    default:
        printf("NotificationType:%d.\r\n", NotificationType);
        break;
    }

    printf("\r\n");
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE RouteChange2Handle = nullptr;


VOID NETIOAPI_API_ RouteChange2(_In_ PVOID CallerContext, _In_opt_ PMIB_IPFORWARD_ROW2 Row, _In_ MIB_NOTIFICATION_TYPE NotificationType)
/*
注册的过程中会被调用几次。

route  -f命令会走这里。
关闭和开启WIFI会走这里。
*/
{
    UNREFERENCED_PARAMETER(CallerContext);

    switch (NotificationType) {
    case MibParameterNotification:
        printf("MibParameterNotification.\r\n");
        break;
    case MibAddInstance:
        printf("MibAddInstance.\r\n");
        break;
    case MibDeleteInstance:
        printf("MibDeleteInstance.\r\n");
        break;
    case MibInitialNotification:
        printf("MibInitialNotification.\r\n");
        break;
    default:
        printf("NotificationType:%d.\r\n", NotificationType);
        break;
    }

    printf("Row:%p.\r\n", Row);
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyRouteChange2()
{
    NTSTATUS Status = CancelMibChangeNotify2(RouteChange2Handle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyRouteChange2()
/*
The NotifyRouteChange2 function registers to be notified for changes to IP route entries on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyroutechange2
*/
{
#pragma prefast(push)
#pragma prefast(disable: 6387, "“_Param_(3)”可能是“0”: 这不符合函数“NotifyRouteChange2”的规范")
    NTSTATUS Status = NotifyRouteChange2(AF_UNSPEC, RouteChange2, nullptr, TRUE, &RouteChange2Handle);
#pragma prefast(pop)

    //(void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotifyStableUnicastIpAddressTableHandle = nullptr;
PMIB_UNICASTIPADDRESS_TABLE Table = nullptr;


VOID NETIOAPI_API_ StableUnicastIpAddressTableCallerCallback(_In_ PVOID CallerContext, _In_ PMIB_UNICASTIPADDRESS_TABLE AddressTable)
/*
如果 NotifyStableUnicastIpAddressTable 返回 ERROR_IO_PENDING，指示 I/O 请求处于挂起状态，则将调用此函数。
*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(AddressTable);
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyStableUnicastIpAddressTable()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotifyStableUnicastIpAddressTableHandle);
    FreeMibTable(Table);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyStableUnicastIpAddressTable()
/*
The NotifyStableUnicastIpAddressTable function retrieves the stable unicast IP address table on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifystableunicastipaddresstable
*/
{
#pragma prefast(push)
#pragma warning(disable:6387) //“_Param_(4)”可能是“0”: 这不符合函数“NotifyStableUnicastIpAddressTable”的规范。
    NTSTATUS Status = NotifyStableUnicastIpAddressTable(AF_UNSPEC, 
                                                        &Table,
                                                        StableUnicastIpAddressTableCallerCallback,
                                                        nullptr,
                                                        &NotifyStableUnicastIpAddressTableHandle);
#pragma prefast(pop)   

    if (Table) {
        printf("NumEntries:%d\r\n", Table->NumEntries);
    }

    //(void)getchar();
}


StableUnicastIpAddressTable::StableUnicastIpAddressTable()
{
}


StableUnicastIpAddressTable::~StableUnicastIpAddressTable()
{
}


void StableUnicastIpAddressTable::Registers()
{
#pragma prefast(push)
#pragma warning(disable:6387) //“_Param_(4)”可能是“0”: 这不符合函数“NotifyStableUnicastIpAddressTable”的规范。
    NTSTATUS Status = NotifyStableUnicastIpAddressTable(AF_UNSPEC, &Table, CallerCallback, nullptr, &Handle);
#pragma prefast(pop)    
    if (Table) {
        printf("NumEntries:%d\r\n", Table->NumEntries);
    }
}


void StableUnicastIpAddressTable::Deregister()
{
    if (Handle) {
        NTSTATUS Status = CancelMibChangeNotify2(Handle);
    }

    if (Table) {
        FreeMibTable(Table);
    }
}

void NTAPI StableUnicastIpAddressTable::CallerCallback(_In_ PVOID CallerContext, _In_ PMIB_UNICASTIPADDRESS_TABLE AddressTable)
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(AddressTable);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE TeredoPortChangeHandle = nullptr;


VOID NETIOAPI_API_ TeredoPortChange(_In_ PVOID CallerContext, _In_ USHORT Port, _Inout_ MIB_NOTIFICATION_TYPE NotificationType)
/*
注册时，这里被调用了。
*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(Port);
    UNREFERENCED_PARAMETER(NotificationType);
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyTeredoPortChange()
{
    NTSTATUS Status = CancelMibChangeNotify2(TeredoPortChangeHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyTeredoPortChange()
/*
The NotifyTeredoPortChange function registers to be notified for changes to the UDP port number used by the Teredo client for the Teredo service port on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyteredoportchange
*/
{
#pragma prefast(push)
#pragma warning(disable:4995) //“NotifyTeredoPortChange”: 名称被标记为 #pragma deprecated
#pragma warning(disable:6387) //“_Param_(2)”可能是“0”: 这不符合函数“NotifyTeredoPortChange”的规范。
    NTSTATUS Status = NotifyTeredoPortChange(TeredoPortChange, nullptr, TRUE, &TeredoPortChangeHandle);
#pragma prefast(pop)

    //(void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotifyUnicastIpAddressChangeHandle = nullptr;


VOID NETIOAPI_API_ UnicastIpAddressChange(_In_ PVOID CallerContext, _In_opt_ PMIB_UNICASTIPADDRESS_ROW Row, _In_ MIB_NOTIFICATION_TYPE NotificationType)
/*

*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(Row);

    switch (NotificationType) {
    case MibParameterNotification:
        printf("A parameter was changed.\r\n");
        break;
    case MibAddInstance:
        printf("A new MIB instance was added.\r\n");
        break;
    case MibDeleteInstance:
        printf("An existing MIB instance was deleted.\r\n");
        break;
    case MibInitialNotification:
        printf("MibInitialNotification.\r\n");
        break;
    default:
        printf("NotificationType:%d.\r\n", NotificationType);
        break;
    }

    printf("\r\n");
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyUnicastIpAddressChange()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotifyUnicastIpAddressChangeHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyUnicastIpAddressChange()
/*
The NotifyUnicastIpAddressChange function registers to be notified for changes to all unicast IP interfaces,
unicast IPv4 addresses, or unicast IPv6 addresses on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyunicastipaddresschange
https://learn.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-createunicastipaddressentry
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = nullptr;
    BOOLEAN InitialNotification = TRUE;

    NTSTATUS Status = NotifyUnicastIpAddressChange(Family, UnicastIpAddressChange, CallerContext, InitialNotification, &NotifyUnicastIpAddressChangeHandle);

    //(void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
WlanRegisterNotification
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlanregisternotification

WlanRegisterDeviceServiceNotification
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlanregisterdeviceservicenotification

WlanRegisterVirtualStationNotification
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlanregistervirtualstationnotification
*/


void WINAPI WlanNotificationCallback(PWLAN_NOTIFICATION_DATA unnamedParam1, PVOID unnamedParam2)
/*

触发时机：
1.注册过程中(WlanRegisterNotification)。
2.关闭WIFI。
3.开启WIFI。
4.反注册的过程中。
5.WlanOpenHandle/WlanCloseHandle
*/
{
    UNREFERENCED_PARAMETER(unnamedParam2);

    WCHAR GuidString[39] = {0};
    (void)StringFromGUID2(unnamedParam1->InterfaceGuid, (LPOLESTR)&GuidString, sizeof(GuidString) / sizeof(*GuidString));

    printf("NotificationSource:%d, NotificationCode:%d, InterfaceGuid:%ls, DataSize:%d\r\n",
           unnamedParam1->NotificationSource,
           unnamedParam1->NotificationCode,
           GuidString,
           unnamedParam1->dwDataSize);//注意：进程退出的处理。
}


DWORD DeRegisterWlanNotification()
/*

Any registration to receive notifications caused by this function would be automatically undone if the calling application closes its calling handle (by calling WlanCloseHandle with the hClientHandle parameter) or if the process ends.

如果调用应用程序通过使用 hClientHandle 参数) 调用 WlanCloseHandle 关闭其调用句柄 (，或者进程结束，则会自动撤消由此函数引起的接收通知的任何注册。

https://learn.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanregisternotification
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlanregisternotification
*/
{
    HANDLE hClient = nullptr;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = WlanOpenHandle(dwMaxClient, nullptr, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    }

    DWORD PrevNotifSource{};
    DWORD ret = WlanRegisterNotification(hClient, WLAN_NOTIFICATION_SOURCE_NONE, TRUE, NULL, NULL, NULL, &PrevNotifSource);
    _ASSERTE(ERROR_SUCCESS == ret);

    return ret;
}


DWORD RegisterWlanNotification()
/*
WlanRegisterNotification 函数用于在所有无线接口上注册和注销通知。

https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlanregisternotification
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nc-wlanapi-wlan_notification_callback
*/
{
    HANDLE hClient = nullptr;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = WlanOpenHandle(dwMaxClient, nullptr, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    }

    DWORD PrevNotifSource{};
    DWORD ret = WlanRegisterNotification(hClient, WLAN_NOTIFICATION_SOURCE_ALL, TRUE, WlanNotificationCallback, NULL, NULL, &PrevNotifSource);
    _ASSERTE(ERROR_SUCCESS == ret);

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


OVERLAPPED overlap{};


void WINAPI DeRegistersNotifyAddrChange()
{
    CancelIPChangeNotify(&overlap);
}


void WINAPI RegistersNotifyAddrChange()
/*
The NotifyAddrChange function causes a notification to be sent to the caller whenever a change occurs in the table that maps IPv4 addresses to interfaces.

Once an application has been notified of a change, the application can then call the GetIpAddrTable or GetAdaptersAddresses function to retrieve the table of IPv4 addresses to determine what has changed

If the application is notified and requires notification for the next change, then the NotifyAddrChange function must be called again.

On Windows Vista and later, the NotifyIpInterfaceChange function can be used to register to be notified for changes to IPv4 and IPv6 interfaces on the local computer.

The following example waits for a change to occur in the table that maps IP addresses to interfaces.

https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-notifyaddrchange?source=recommendations
*/
{
    HANDLE hand = nullptr;
    overlap.hEvent = WSACreateEvent();

    DWORD ret = NotifyAddrChange(&hand, &overlap);
    if (ret != NO_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            printf("NotifyAddrChange error...%d\n", WSAGetLastError());
            return;
        }
    }

    if (WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0)
        printf("IP Address table changed..\n");//修改IPv4，触发事件，走到这里。IPv6的修改没走这里。
}


//////////////////////////////////////////////////////////////////////////////////////////////////


OVERLAPPED RouteChangeOverlap;


void DeRegistersNotifyRouteChange()
/*
CancelIPChangeNotify 函数通过成功调用 NotifyAddrChange 或 NotifyRouteChange 函数，取消之前请求的 IP 地址和路由更改的通知。

https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-notifyroutechange
*/
{
    CancelIPChangeNotify(&RouteChangeOverlap);
}


void RegistersNotifyRouteChange()
/*
只要 IPv4 路由表中发生更改， NotifyRouteChange 函数就会向调用方发送通知。

通知应用程序更改后，应用程序可以调用 GetIpForwardTable 或 GetIpForwardTable2 函数来检索 IPv4 路由表以确定更改的内容。

如果应用程序收到通知并要求通知进行下一次更改，则必须再次调用 NotifyRouteChange 函数。

在 Windows Vista 及更高版本上，可以使用 NotifyRouteChange2 函数注册，以便收到本地计算机上 IPv6 路由表更改的通知。

以下示例等待 IP 路由表中发生更改。

https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-notifyroutechange
https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-notifyroutechange
*/
{
    HANDLE hand = NULL;
    RouteChangeOverlap.hEvent = WSACreateEvent();

    DWORD ret = NotifyRouteChange(&hand, &RouteChangeOverlap);
    if (ret != NO_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            printf("NotifyRouteChange error...%d\n", WSAGetLastError());
            return;
        }
    }

    if (WaitForSingleObject(RouteChangeOverlap.hEvent, INFINITE) == WAIT_OBJECT_0)
        printf("Routing table changed..\n");
}


//////////////////////////////////////////////////////////////////////////////////////////////////
