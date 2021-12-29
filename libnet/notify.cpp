#include "pch.h"
#include "notify.h"


#pragma warning(disable:4189) //�ֲ������ѳ�ʼ����������


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID NETIOAPI_API_ NetworkConnectivityHintChange(_In_ PVOID CallerContext,
                                                 _In_ NL_NETWORK_CONNECTIVITY_HINT ConnectivityHint
)
/*
ע��Ĺ����лᱻ����1�Ρ�

��δ�����ʹ���ﱻ�����أ�
1.�رպͿ�������Ҳ���С�
2.�ı�IP��ַҲ���С�
*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(ConnectivityHint);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyNetworkConnectivityHintChange()
/*
Registers an application-defined callback function,
to be called when the aggregate network connectivity level and cost hints change.

���Ҳû�з�ע��ĺ�����û�ҵ���

Minimum supported client	Windows 10, version 2004 (10.0; Build 19041)
Minimum supported server	Windows Server, version 2004 (10.0; Build 19041)

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifynetworkconnectivityhintchange
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;
    HANDLE NotificationHandle = NULL;

    NTSTATUS Status = NotifyNetworkConnectivityHintChange(NetworkConnectivityHintChange,
                                                          CallerContext,
                                                          InitialNotification,
                                                          &NotificationHandle);

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotificationHandle = NULL;


VOID NETIOAPI_API_ IpInterfaceChange(_In_ PVOID CallerContext,
                                     _In_ PMIB_IPINTERFACE_ROW Row OPTIONAL,
                                     _In_ MIB_NOTIFICATION_TYPE NotificationType
)
/*
ע��Ĺ����лᱻ���ü��Σ�NotificationType == MibInitialNotification��

��δ�����ʹ���ﱻ�����أ�
1.�رպͿ�������Ҳ���С�
2.�ı�IP��ַҲ���С�
*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(Row);

    switch (NotificationType) {
    case MibParameterNotification:

        break;
    case MibAddInstance:

        break;
    case MibDeleteInstance:

        break;
    case MibInitialNotification:

        break;
    default:
        break;
    }
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
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

    NTSTATUS Status = NotifyIpInterfaceChange(Family,
                                              IpInterfaceChange,
                                              CallerContext,
                                              InitialNotification,
                                              &NotificationHandle);

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE RouteChange2Handle = NULL;


VOID NETIOAPI_API_ RouteChange2(_In_ PVOID CallerContext,
                                _In_opt_ PMIB_IPFORWARD_ROW2 Row,
                                _In_ MIB_NOTIFICATION_TYPE NotificationType
)
/*
ע��Ĺ����лᱻ���ü��Ρ�

route  -f����������
*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(Row);

    switch (NotificationType) {
    case MibParameterNotification:

        break;
    case MibAddInstance:

        break;
    case MibDeleteInstance:

        break;
    case MibInitialNotification:

        break;
    default:
        break;
    }
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
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

#pragma prefast(push)
#pragma prefast(disable: 6387, "��_Param_(3)�������ǡ�0��: �ⲻ���Ϻ�����NotifyRouteChange2���Ĺ淶")
    NTSTATUS Status = NotifyRouteChange2(Family,
                                         RouteChange2,
                                         CallerContext,
                                         InitialNotification,
                                         &RouteChange2Handle);
#pragma prefast(pop)

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotifyStableUnicastIpAddressTableHandle = NULL;


VOID NETIOAPI_API_ StableUnicastIpAddressTableCallerCallback(_In_ PVOID CallerContext,
                                                             _In_ PMIB_UNICASTIPADDRESS_TABLE AddressTable
)
/*

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
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyStableUnicastIpAddressTable()
/*
The NotifyStableUnicastIpAddressTable function retrieves the stable unicast IP address table on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifystableunicastipaddresstable
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PMIB_UNICASTIPADDRESS_TABLE * Table = NULL;
    PVOID CallerContext = NULL;

#pragma prefast(push)
#pragma prefast(disable: 6387, "��Table�������ǡ�0��: �ⲻ���Ϻ�����NotifyStableUnicastIpAddressTable���Ĺ淶")
    NTSTATUS Status = NotifyStableUnicastIpAddressTable(Family,
                                                        Table,
                                                        StableUnicastIpAddressTableCallerCallback,
                                                        CallerContext,
                                                        &NotifyStableUnicastIpAddressTableHandle);
#pragma prefast(pop)

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE TeredoPortChangeHandle = NULL;


VOID NETIOAPI_API_ TeredoPortChange(_In_ PVOID CallerContext,
                                    _In_ USHORT Port,
                                    _Inout_ MIB_NOTIFICATION_TYPE NotificationType
)
/*
ע��ʱ�����ﱻ�����ˡ�
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
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

#pragma prefast(push)
#pragma prefast(disable: 6387, "��CallerContext�������ǡ�0��: �ⲻ���Ϻ�����NotifyTeredoPortChange���Ĺ淶")
#pragma warning(disable:4995)
    NTSTATUS Status = NotifyTeredoPortChange(TeredoPortChange,
                                             CallerContext,
                                             InitialNotification,
                                             &TeredoPortChangeHandle);
#pragma prefast(pop)

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotifyUnicastIpAddressChangeHandle = NULL;


VOID NETIOAPI_API_ UnicastIpAddressChange(_In_ PVOID CallerContext,
                                          _In_opt_ PMIB_UNICASTIPADDRESS_ROW Row,
                                          _In_ MIB_NOTIFICATION_TYPE NotificationType
)
/*

*/
{
    UNREFERENCED_PARAMETER(CallerContext);
    UNREFERENCED_PARAMETER(Row);
    UNREFERENCED_PARAMETER(NotificationType);
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
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

    NTSTATUS Status = NotifyUnicastIpAddressChange(Family,
                                                   UnicastIpAddressChange,
                                                   CallerContext,
                                                   InitialNotification,
                                                   &NotifyUnicastIpAddressChangeHandle);

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////
