#include "pch.h"
#include "wfp.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
功能：测试WFP的一些Enum函数的用法。
      虽然是应用程序，我想在内核驱动一样也可实现。

made by correy
made at 2016.08.19
hpmepage:http://correy.webs.com
*/


BOOLEAN GUIDsAreEqual(_In_ const GUID * pGUIDAlpha, _In_ const GUID * pGUIDOmega)
/**
   Purpose:  Determine if two GUIDs are identical.
   MSDN_Ref: HTTP://MSDN.Microsoft.com/En-US/Library/AA379329.aspx
*/
{
    RPC_STATUS status = RPC_S_OK;
    UINT32     areEqual = FALSE;

    if (pGUIDAlpha == 0 || pGUIDOmega == 0) {
        if ((pGUIDAlpha == 0 && pGUIDOmega) || (pGUIDAlpha && pGUIDOmega == 0))
            goto cleanup;
    }

    if (pGUIDAlpha == 0 && pGUIDOmega == 0) {
        areEqual = TRUE;
        goto cleanup;
    }

    areEqual = UuidEqual((UUID *)pGUIDAlpha, (UUID *)pGUIDOmega, &status);
    if (status != RPC_S_OK) {
        printf("GUIDsAreEqual : UuidEqual() [status %#x]", status);
    }

cleanup:
    return (BOOLEAN)areEqual;
}


const wchar_t * GUID2M(_In_ GUID * guid)
/*
功能：根据GUID获取编程语言用到的宏定义。
*/
{
    if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_IPPACKET_V4, guid))
        return L"FWPM_LAYER_INBOUND_IPPACKET_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_IPPACKET_V4_DISCARD, guid))
        return L"FWPM_LAYER_INBOUND_IPPACKET_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_IPPACKET_V6, guid))
        return L"FWPM_LAYER_INBOUND_IPPACKET_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_IPPACKET_V6_DISCARD, guid))
        return L"FWPM_LAYER_INBOUND_IPPACKET_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_IPPACKET_V4, guid))
        return L"FWPM_LAYER_OUTBOUND_IPPACKET_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_IPPACKET_V4_DISCARD, guid))
        return L"FWPM_LAYER_OUTBOUND_IPPACKET_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_IPPACKET_V6, guid))
        return L"FWPM_LAYER_OUTBOUND_IPPACKET_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_IPPACKET_V6_DISCARD, guid))
        return L"FWPM_LAYER_OUTBOUND_IPPACKET_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_IPFORWARD_V4, guid))
        return L"FWPM_LAYER_IPFORWARD_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_IPFORWARD_V4_DISCARD, guid))
        return L"FWPM_LAYER_IPFORWARD_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_IPFORWARD_V6, guid))
        return L"FWPM_LAYER_IPFORWARD_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_IPFORWARD_V6_DISCARD, guid))
        return L"FWPM_LAYER_IPFORWARD_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_TRANSPORT_V4, guid))
        return L"FWPM_LAYER_INBOUND_TRANSPORT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_TRANSPORT_V4_DISCARD, guid))
        return L"FWPM_LAYER_INBOUND_TRANSPORT_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_TRANSPORT_V6, guid))
        return L"FWPM_LAYER_INBOUND_TRANSPORT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_TRANSPORT_V6_DISCARD, guid))
        return L"FWPM_LAYER_INBOUND_TRANSPORT_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_TRANSPORT_V4, guid))
        return L"FWPM_LAYER_OUTBOUND_TRANSPORT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_TRANSPORT_V4_DISCARD, guid))
        return L"FWPM_LAYER_OUTBOUND_TRANSPORT_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_TRANSPORT_V6, guid))
        return L"FWPM_LAYER_OUTBOUND_TRANSPORT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_TRANSPORT_V6_DISCARD, guid))
        return L"FWPM_LAYER_OUTBOUND_TRANSPORT_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_STREAM_V4, guid))
        return L"FWPM_LAYER_STREAM_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_STREAM_V4_DISCARD, guid))
        return L"FWPM_LAYER_STREAM_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_STREAM_V6, guid))
        return L"FWPM_LAYER_STREAM_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_STREAM_V6_DISCARD, guid))
        return L"FWPM_LAYER_STREAM_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_DATAGRAM_DATA_V4, guid))
        return L"FWPM_LAYER_DATAGRAM_DATA_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_DATAGRAM_DATA_V4_DISCARD, guid))
        return L"FWPM_LAYER_DATAGRAM_DATA_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_DATAGRAM_DATA_V6, guid))
        return L"FWPM_LAYER_DATAGRAM_DATA_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_DATAGRAM_DATA_V6_DISCARD, guid))
        return L"FWPM_LAYER_DATAGRAM_DATA_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_ICMP_ERROR_V4, guid))
        return L"FWPM_LAYER_INBOUND_ICMP_ERROR_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_ICMP_ERROR_V4_DISCARD, guid))
        return L"FWPM_LAYER_INBOUND_ICMP_ERROR_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_ICMP_ERROR_V6, guid))
        return L"FWPM_LAYER_INBOUND_ICMP_ERROR_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_ICMP_ERROR_V6_DISCARD, guid))
        return L"FWPM_LAYER_INBOUND_ICMP_ERROR_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_ICMP_ERROR_V4, guid))
        return L"FWPM_LAYER_OUTBOUND_ICMP_ERROR_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_ICMP_ERROR_V4_DISCARD, guid))
        return L"FWPM_LAYER_OUTBOUND_ICMP_ERROR_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_ICMP_ERROR_V6, guid))
        return L"FWPM_LAYER_OUTBOUND_ICMP_ERROR_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_ICMP_ERROR_V6_DISCARD, guid))
        return L"FWPM_LAYER_OUTBOUND_ICMP_ERROR_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4, guid))
        return L"FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4_DISCARD, guid))
        return L"FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6, guid))
        return L"FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6_DISCARD, guid))
        return L"FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_LISTEN_V4, guid))
        return L"FWPM_LAYER_ALE_AUTH_LISTEN_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_LISTEN_V4_DISCARD, guid))
        return L"FWPM_LAYER_ALE_AUTH_LISTEN_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_LISTEN_V6, guid))
        return L"FWPM_LAYER_ALE_AUTH_LISTEN_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_LISTEN_V6_DISCARD, guid))
        return L"FWPM_LAYER_ALE_AUTH_LISTEN_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4, guid))
        return L"FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4_DISCARD, guid))
        return L"FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6, guid))
        return L"FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6_DISCARD, guid))
        return L"FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_CONNECT_V4, guid))
        return L"FWPM_LAYER_ALE_AUTH_CONNECT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_CONNECT_V4_DISCARD, guid))
        return L"FWPM_LAYER_ALE_AUTH_CONNECT_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_CONNECT_V6, guid))
        return L"FWPM_LAYER_ALE_AUTH_CONNECT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_AUTH_CONNECT_V6_DISCARD, guid))
        return L"FWPM_LAYER_ALE_AUTH_CONNECT_V6_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4, guid))
        return L"FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4_DISCARD, guid))
        return L"FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4_DISCARD";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6, guid))
        return L"FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6_DISCARD, guid))
        return L"FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6_DISCARD";
#if(NTDDI_VERSION >= NTDDI_WIN7)
    else if (GUIDsAreEqual(&FWPM_LAYER_NAME_RESOLUTION_CACHE_V4, guid))
        return L"FWPM_LAYER_NAME_RESOLUTION_CACHE_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_NAME_RESOLUTION_CACHE_V6, guid))
        return L"FWPM_LAYER_NAME_RESOLUTION_CACHE_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_RESOURCE_RELEASE_V4, guid))
        return L"FWPM_LAYER_ALE_RESOURCE_RELEASE_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_RESOURCE_RELEASE_V6, guid))
        return L"FWPM_LAYER_ALE_RESOURCE_RELEASE_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V4, guid))
        return L"FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V6, guid))
        return L"FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_CONNECT_REDIRECT_V4, guid))
        return L"FWPM_LAYER_ALE_CONNECT_REDIRECT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_CONNECT_REDIRECT_V6, guid))
        return L"FWPM_LAYER_ALE_CONNECT_REDIRECT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_BIND_REDIRECT_V4, guid))
        return L"FWPM_LAYER_ALE_BIND_REDIRECT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_ALE_BIND_REDIRECT_V6, guid))
        return L"FWPM_LAYER_ALE_BIND_REDIRECT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_STREAM_PACKET_V4, guid))
        return L"FWPM_LAYER_STREAM_PACKET_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_STREAM_PACKET_V6, guid))
        return L"FWPM_LAYER_STREAM_PACKET_V6";
#if(NTDDI_VERSION >= NTDDI_WIN8)
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_MAC_FRAME_ETHERNET, guid))
        return L"FWPM_LAYER_INBOUND_MAC_FRAME_ETHERNET";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_MAC_FRAME_ETHERNET, guid))
        return L"FWPM_LAYER_OUTBOUND_MAC_FRAME_ETHERNET";
    else if (GUIDsAreEqual(&FWPM_LAYER_INBOUND_MAC_FRAME_NATIVE, guid))
        return L"FWPM_LAYER_INBOUND_MAC_FRAME_NATIVE";
    else if (GUIDsAreEqual(&FWPM_LAYER_OUTBOUND_MAC_FRAME_NATIVE, guid))
        return L"FWPM_LAYER_OUTBOUND_MAC_FRAME_NATIVE";
    else if (GUIDsAreEqual(&FWPM_LAYER_INGRESS_VSWITCH_ETHERNET, guid))
        return L"FWPM_LAYER_INGRESS_VSWITCH_ETHERNET";
    else if (GUIDsAreEqual(&FWPM_LAYER_EGRESS_VSWITCH_ETHERNET, guid))
        return L"FWPM_LAYER_EGRESS_VSWITCH_ETHERNET";
    else if (GUIDsAreEqual(&FWPM_LAYER_INGRESS_VSWITCH_TRANSPORT_V4, guid))
        return L"FWPM_LAYER_INGRESS_VSWITCH_TRANSPORT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_INGRESS_VSWITCH_TRANSPORT_V6, guid))
        return L"FWPM_LAYER_INGRESS_VSWITCH_TRANSPORT_V6";
    else if (GUIDsAreEqual(&FWPM_LAYER_EGRESS_VSWITCH_TRANSPORT_V4, guid))
        return L"FWPM_LAYER_EGRESS_VSWITCH_TRANSPORT_V4";
    else if (GUIDsAreEqual(&FWPM_LAYER_EGRESS_VSWITCH_TRANSPORT_V6, guid))
        return L"FWPM_LAYER_EGRESS_VSWITCH_TRANSPORT_V6";
#endif // (NTDDI_VERSION >= NTDDI_WIN8)
#endif // (NTDDI_VERSION >= NTDDI_WIN7)

    return L"unknown GUID";
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumCallout(HANDLE engineHandle)
{
    FWPM_CALLOUT_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_CALLOUT ** ppCallouts = 0;
    UINT32                     calloutCount = 0;

    DWORD result = FwpmCalloutCreateEnumHandle0(engineHandle, &enumTemplate, &enumHandle);
    assert(ERROR_SUCCESS == result);

    result = FwpmCalloutEnum0(engineHandle, enumHandle, INFINITE, &ppCallouts, &calloutCount);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < calloutCount; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        int n = StringFromGUID2(ppCallouts[calloutIndex]->calloutKey, GUID, MAX_PATH);
        assert(n);
        printf("calloutKey:%ws.\n", GUID);

        printf("displayData.name:%ws.\n", ppCallouts[calloutIndex]->displayData.name);
        printf("displayData.description:%ws.\n", ppCallouts[calloutIndex]->displayData.description);
        printf("flags:0x%x.\n", ppCallouts[calloutIndex]->flags);

        if (ppCallouts[calloutIndex]->providerKey) {
            RtlZeroMemory(GUID, sizeof(GUID));
            n = StringFromGUID2(*ppCallouts[calloutIndex]->providerKey, GUID, MAX_PATH);
            assert(n);
            printf("providerKey:%ws.\n", GUID);
        }

        if (ppCallouts[calloutIndex]->providerData.size) {
            printf("calloutKey:%ws.\n", (wchar_t *)ppCallouts[calloutIndex]->providerData.data);//不以NULL结尾，有可能打印非法字符。
        }

        RtlZeroMemory(GUID, sizeof(GUID));
        n = StringFromGUID2(ppCallouts[calloutIndex]->applicableLayer, GUID, MAX_PATH);
        assert(n);
        printf("applicableLayer:%ws.\n", GUID);

        printf("calloutId:%d.\n", ppCallouts[calloutIndex]->calloutId);

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&ppCallouts);

    result = FwpmCalloutDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID PrintEventHeader(__in FWPM_NET_EVENT0 * pEvent)
/*++
Routine Description:
     This routine prints information in the event header for illustration purposes
Arguments:
    pEvent - Pointer to the event
--*/
{
    SYSTEMTIME systemTime = {0};
    SYSTEMTIME localTime = {0};
    LPTSTR sidString;
    WCHAR dateStr[MAX_PATH];
    WCHAR timeStr[MAX_PATH];

    // Static array containing string description of IP versions
    static const wchar_t * const  mIPVersion[] =
    {
        L"IPv4",
        L"IPv6"
    };

    // Static array containing string description of IKE event type
    static const wchar_t * const  mEventType[] =
    {
       L"Main Mode failure",
       L"Quick Mode failure",
       L"User Mode failure",
       L"Classify drop",
       L"IPsec Kernel drop"
    };

    // Get Time
    FileTimeToSystemTime(&(pEvent->header.timeStamp), &systemTime);
    SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &localTime);

    // Format time & date
    GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &localTime, NULL, timeStr, MAX_PATH);
    GetDateFormat(LOCALE_USER_DEFAULT, 0, &localTime, NULL, dateStr, MAX_PATH);

    wprintf(L"%-16s:%s %s\n", L"Event Time", dateStr, timeStr);
    if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_IP_VERSION_SET) {
        wprintf(L"%-16s:%s\n", L"IP Version", mIPVersion[pEvent->header.ipVersion]);
    }
    if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_IP_PROTOCOL_SET) {
        wprintf(L"%-16s:%u\n", L"IP Protocol", (UINT32)pEvent->header.ipProtocol);
    }

    if (pEvent->header.ipVersion == FWP_IP_VERSION_V4) {
        // Print IPv4 addresses
        char * addrstr = NULL;
        struct in_addr addr;

        if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_LOCAL_ADDR_SET) {
            addr.s_addr = htonl(pEvent->header.localAddrV4);
            addrstr = inet_ntoa(addr);
            if (addrstr) {
                wprintf(L"%-16s:%S\n", L"Local Address", addrstr);
            }
        }

        if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_REMOTE_ADDR_SET) {
            addr.s_addr = htonl(pEvent->header.remoteAddrV4);
            addrstr = inet_ntoa(addr);
            if (addrstr) {
                wprintf(L"%-16s:%S\n", L"Remote  Address", addrstr);
            }
        }
    } else {
        // Print IPv6 addresses
        int i;
        if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_LOCAL_ADDR_SET) {
            wprintf(L"%-16s:", L"Local Address");
            for (i = 0; i < 16; i = i + 2) {
                wprintf(L"%02x%02x",
                        pEvent->header.localAddrV6.byteArray16[i],
                        pEvent->header.localAddrV6.byteArray16[i + 1]);
                if (14 != i) wprintf(L":");
            }
            wprintf(L"\n");
        }

        if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_REMOTE_ADDR_SET) {
            wprintf(L"%-16s:", L"Remote Address");
            for (i = 0; i < 16; i = i + 2) {
                wprintf(L"%02x%02x",
                        pEvent->header.remoteAddrV6.byteArray16[i],
                        pEvent->header.remoteAddrV6.byteArray16[i + 1]);
                if (14 != i) wprintf(L":");
            }
            wprintf(L"\n");
        }
    }

    // Print local port
    if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_LOCAL_PORT_SET) {
        wprintf(L"%-16s:%u\n", L"Local Port", (UINT32)pEvent->header.localPort);
    }

    // Print remote port
    if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_REMOTE_PORT_SET) {
        wprintf(L"%-16s:%u\n", L"Remote Port", (UINT32)pEvent->header.remotePort);
    }

    // Print event type
    wprintf(L"%-16s:%s\n", L"Event Type", mEventType[pEvent->type]);

    // Print fully qualified path of the application
    if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_APP_ID_SET) {
        WCHAR * appid = (WCHAR *)pEvent->header.appId.data;
        wprintf(L"%-16s:%s[%u]\n", L"App Id[Size]", appid, pEvent->header.appId.size);
    }

    // Print User SID
    if (pEvent->header.flags & FWPM_NET_EVENT_FLAG_USER_ID_SET) {
        if (ConvertSidToStringSid(pEvent->header.userId, &sidString)) {
            wprintf(L"%-16s:%s\n", L"SID", sidString);
            LocalFree(sidString);
        }
    }
}


VOID PrintClassifyEvent(FWPM_NET_EVENT0 * pEvent)
/*++
Routine Description:
     This routine prints information in the classify drop events for illustration purposes.
Arguments:
    pEvent - Pointer to the event.
--*/
{
    FWPM_NET_EVENT_CLASSIFY_DROP0 * pClassifyEvent = pEvent->classifyDrop;
    PrintEventHeader(pEvent);
    wprintf(L"%-16s:%s%I64x\n", L"Filter Id", L"0x", pClassifyEvent->filterId);
    wprintf(L"%-16s:%s%x\n", L"Layer Id", L"0x", (UINT32)pClassifyEvent->layerId);
    wprintf(L"\n");
}


VOID PrintEvents(__in FWPM_NET_EVENT0 ** matchedEvents, __in UINT32 numMatchedEvents)
/*++
Routine Description:
     This routine prints the diagnostic event for illustration purposes.
Arguments:
    matchedEvents - Array of pointer to the events which are to be printed
    numMatchedEvents - Number of event pointers in the array.

参考，摘自：Microsoft SDKs\Windows\v6.0\Samples\NetDs\WFP\DiagEvents
--*/
{
    UINT32 num = 0;
    FWPM_NET_EVENT0 * pEvent;

    for (num = 0; num < numMatchedEvents; num++) {
        pEvent = matchedEvents[num];

        switch (pEvent->type) {
        case FWPM_NET_EVENT_TYPE_IKEEXT_MM_FAILURE:
            //PrintMMEvent(pEvent);
            break;
        case FWPM_NET_EVENT_TYPE_IKEEXT_QM_FAILURE:
            //PrintQMEvent(pEvent);
            break;
        case FWPM_NET_EVENT_TYPE_IKEEXT_EM_FAILURE:
            //PrintEMEvent(pEvent);
            break;
        case FWPM_NET_EVENT_TYPE_CLASSIFY_DROP:
            PrintClassifyEvent(pEvent);
            break;
        case FWPM_NET_EVENT_TYPE_IPSEC_KERNEL_DROP:
            //PrintKernelDropEvent(pEvent);
            break;
        default:
            return;
        }
    }
}


void EnumNetEvent(HANDLE engineHandle)
{
    FWPM_NET_EVENT_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_NET_EVENT0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmNetEventCreateEnumHandle0(engineHandle, &enumTemplate, &enumHandle);
    assert(ERROR_SUCCESS == result);

    result = FwpmNetEventEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    PrintEvents(entries, numEntriesReturned);

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmNetEventDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void PRINT_FWP_MATCH_TYPE_STRUCTURE(FWP_MATCH_TYPE pfmt)
{
    switch (pfmt) {
    case FWP_MATCH_EQUAL:

        break;
    case FWP_MATCH_GREATER:

        break;
    case FWP_MATCH_LESS:

        break;
    case FWP_MATCH_GREATER_OR_EQUAL:

        break;
    case FWP_MATCH_LESS_OR_EQUAL:

        break;
    case FWP_MATCH_RANGE:

        break;
    case FWP_MATCH_FLAGS_ALL_SET:

        break;
    case FWP_MATCH_FLAGS_ANY_SET:

        break;
    case FWP_MATCH_FLAGS_NONE_SET:

        break;
    case FWP_MATCH_EQUAL_CASE_INSENSITIVE:

        break;
    case FWP_MATCH_NOT_EQUAL:

        break;
    case FWP_MATCH_TYPE_MAX:
        assert(false);
        break;
    default:
        assert(false);
        break;
    }
}


void PRINT_FWP_CONDITION_VALUE0_STRUCTURE(FWP_CONDITION_VALUE0 * pfcv)
/*
其实FWP_CONDITION_VALUE0比FWP_VALUE0多几个类型而已，都是FWP_DATA_TYPE枚举类型。
*/
{
    assert(pfcv);
    if (pfcv == NULL) {
        return;
    }

    switch (pfcv->type) {
    case FWP_EMPTY:

        break;
    case FWP_UINT8:

        break;
    case FWP_UINT16:

        break;
    case FWP_UINT32:

        break;
    case FWP_UINT64:

        break;
    case FWP_INT8:

        break;
    case FWP_INT16:

        break;
    case FWP_INT32:

        break;
    case FWP_INT64:

        break;
    case FWP_FLOAT:

        break;
    case FWP_DOUBLE:

        break;
    case FWP_BYTE_ARRAY16_TYPE:

        break;
    case FWP_BYTE_BLOB_TYPE:

        break;
    case FWP_SID:

        break;
    case FWP_SECURITY_DESCRIPTOR_TYPE:

        break;
    case FWP_TOKEN_INFORMATION_TYPE:

        break;
    case FWP_TOKEN_ACCESS_INFORMATION_TYPE:

        break;
    case FWP_UNICODE_STRING_TYPE:

        break;
    case FWP_BYTE_ARRAY6_TYPE:

        break;
    case FWP_SINGLE_DATA_TYPE_MAX:

        break;
    case FWP_V4_ADDR_MASK:

        break;
    case FWP_V6_ADDR_MASK:

        break;
    case FWP_RANGE_TYPE:

        break;
    case FWP_DATA_TYPE_MAX:
        assert(false);
        break;
    default:
        assert(false);
        break;
    }
}


void PRINT_FWP_VALUE0_STRUCTURE(FWP_VALUE0 * pfv)
{
    assert(pfv);
    if (pfv == NULL) {
        return;
    }

    switch (pfv->type) {
    case FWP_EMPTY:

        break;
    case FWP_UINT8:

        break;
    case FWP_UINT16:

        break;
    case FWP_UINT32:

        break;
    case FWP_UINT64:

        break;
    case FWP_INT8:

        break;
    case FWP_INT16:

        break;
    case FWP_INT32:

        break;
    case FWP_INT64:

        break;
    case FWP_FLOAT:

        break;
    case FWP_DOUBLE:

        break;
    case FWP_BYTE_ARRAY16_TYPE:

        break;
    case FWP_BYTE_BLOB_TYPE:

        break;
    case FWP_SID:

        break;
    case FWP_SECURITY_DESCRIPTOR_TYPE:

        break;
    case FWP_TOKEN_INFORMATION_TYPE:

        break;
    case FWP_TOKEN_ACCESS_INFORMATION_TYPE:

        break;
    case FWP_UNICODE_STRING_TYPE:

        break;
    default:
        assert(false);
        break;
    }
}


void EnumFilter(HANDLE engineHandle)
{
    FWPM_FILTER_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_FILTER0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmFilterCreateEnumHandle0(engineHandle, /*&enumTemplate*/ NULL, &enumHandle);//必须填写，不可内容为空，或者填写地址NULL。
    assert(ERROR_SUCCESS == result);

    result = FwpmFilterEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < numEntriesReturned; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        printf("Index:%d.\n", (calloutIndex + 1));

        int n = StringFromGUID2(entries[calloutIndex]->filterKey, GUID, MAX_PATH);
        assert(n);
        printf("filterKey:%ws.\n", GUID);

        printf("displayData.name:%ws.\n", entries[calloutIndex]->displayData.name);
        printf("displayData.description:%ws.\n", entries[calloutIndex]->displayData.description);
        printf("flags:0x%x.\n", entries[calloutIndex]->flags);

        if (entries[calloutIndex]->providerKey != 0) {
            RtlZeroMemory(GUID, sizeof(GUID));
            n = StringFromGUID2(*entries[calloutIndex]->providerKey, GUID, MAX_PATH);
            if (0 == n) {
                DebugBreak();
            }
            printf("providerKey:%ws.\n", GUID);
        }

        if (entries[calloutIndex]->providerData.size) {
            printf("calloutKey:%ws.\n", (wchar_t *)entries[calloutIndex]->providerData.data);//不以NULL结尾，有可能打印非法字符。
        }

        RtlZeroMemory(GUID, sizeof(GUID));
        n = StringFromGUID2(entries[calloutIndex]->layerKey, GUID, MAX_PATH);
        assert(n);
        printf("layerKey:%ws(%ws).\n", GUID, GUID2M(&entries[calloutIndex]->layerKey));

        RtlZeroMemory(GUID, sizeof(GUID));
        n = StringFromGUID2(entries[calloutIndex]->subLayerKey, GUID, MAX_PATH);
        assert(n);
        printf("subLayerKey:%ws.\n", GUID);

        PRINT_FWP_VALUE0_STRUCTURE(&entries[calloutIndex]->weight);

        printf("numFilterConditions:%d.\n", entries[calloutIndex]->numFilterConditions);
        for (UINT32 x = 0; x < entries[calloutIndex]->numFilterConditions; x++) {
            RtlZeroMemory(GUID, sizeof(GUID));
            //n = StringFromGUID2(entries[calloutIndex]->filterCondition->fieldKey, GUID, MAX_PATH);//不变。是要变的。
            n = StringFromGUID2(entries[calloutIndex]->filterCondition[x].fieldKey, GUID, MAX_PATH);
            assert(n);
            printf("filterCondition->fieldKey:%ws.\n", GUID);

            PRINT_FWP_MATCH_TYPE_STRUCTURE(entries[calloutIndex]->filterCondition->matchType);
            PRINT_FWP_CONDITION_VALUE0_STRUCTURE(&entries[calloutIndex]->filterCondition->conditionValue);
        }

        printf("action.type:0x%x.\n", entries[calloutIndex]->action.type);
        //printf("action.filterType:0x%x.\n", entries[calloutIndex]->action.filterType);
        //printf("action.calloutKey:0x%x.\n", entries[calloutIndex]->action.calloutKey);

        if (entries[calloutIndex]->rawContext) {
            printf("rawContext:0x%I64x.\n", entries[calloutIndex]->rawContext);
        }

        if (entries[calloutIndex]->providerContextKey.Data1 &&
            entries[calloutIndex]->providerContextKey.Data2 &&
            entries[calloutIndex]->providerContextKey.Data3 &&
            entries[calloutIndex]->providerContextKey.Data4
            ) {
            RtlZeroMemory(GUID, sizeof(GUID));
            n = StringFromGUID2(entries[calloutIndex]->providerContextKey, GUID, MAX_PATH);
            assert(n);
            printf("providerContextKey:%ws.\n", GUID);
        }

        if (entries[calloutIndex]->reserved) {
            printf("reserved:0x%p.\n", entries[calloutIndex]->reserved);
        }

        printf("filterId:0x%I64x.\n", entries[calloutIndex]->filterId);

        PRINT_FWP_VALUE0_STRUCTURE(&entries[calloutIndex]->effectiveWeight);

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmFilterDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumLayer(HANDLE engineHandle)
{
    FWPM_LAYER_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_LAYER0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmLayerCreateEnumHandle0(engineHandle, &enumTemplate/* NULL*/, &enumHandle);
    assert(ERROR_SUCCESS == result);

    result = FwpmLayerEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < numEntriesReturned; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        printf("Index:%d.\n", (calloutIndex + 1));

        int n = StringFromGUID2(entries[calloutIndex]->layerKey, GUID, MAX_PATH);
        assert(n);
        printf("layerKey:%ws.\n", GUID);

        printf("displayData.name:%ws.\n", entries[calloutIndex]->displayData.name);
        printf("displayData.description:%ws.\n", entries[calloutIndex]->displayData.description);
        printf("flags:0x%x.\n", entries[calloutIndex]->flags);

        printf("numFields:0x%x.\n", entries[calloutIndex]->numFields);
        for (UINT32 x = 0; x < entries[calloutIndex]->numFields; x++) {
            //int n = StringFromGUID2(*entries[calloutIndex]->field->fieldKey, GUID, MAX_PATH);//重复。
            n = StringFromGUID2(*entries[calloutIndex]->field[x].fieldKey, GUID, MAX_PATH);
            assert(n);
            printf("field->fieldKey:%ws.\n", GUID);

            //printf("field->type:%ws.\n", GUID);
            //printf("field->dataType:%ws.\n", GUID);
        }

        n = StringFromGUID2(entries[calloutIndex]->defaultSubLayerKey, GUID, MAX_PATH);
        assert(n);
        printf("defaultSubLayerKey:%ws.\n", GUID);

        printf("layerId:0x%x.\n", entries[calloutIndex]->layerId);

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmLayerDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumProvider(HANDLE engineHandle)
{
    FWPM_PROVIDER_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_PROVIDER0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmProviderCreateEnumHandle0(engineHandle, &enumTemplate/* NULL*/, &enumHandle);
    assert(ERROR_SUCCESS == result);

    result = FwpmProviderEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < numEntriesReturned; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        printf("Index:%d.\n", (calloutIndex + 1));

        int n = StringFromGUID2(entries[calloutIndex]->providerKey, GUID, MAX_PATH);
        assert(n);
        printf("providerKey:%ws.\n", GUID);

        printf("displayData.name:%ws.\n", entries[calloutIndex]->displayData.name);
        printf("displayData.description:%ws.\n", entries[calloutIndex]->displayData.description);
        printf("flags:0x%x.\n", entries[calloutIndex]->flags);

        if (entries[calloutIndex]->providerData.size) {
            printf("providerData.data:%hs.\n", entries[calloutIndex]->providerData.data);
        }

        printf("serviceName:%ws.\n", entries[calloutIndex]->serviceName);

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmProviderDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumProviderContext(HANDLE engineHandle)
{
    FWPM_PROVIDER_CONTEXT_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_PROVIDER_CONTEXT0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmProviderContextCreateEnumHandle0(engineHandle, /*&enumTemplate*/ NULL, &enumHandle);//必须填写NULL，否则即使特到句柄，下面的获取数据为空。
    assert(ERROR_SUCCESS == result);

    result = FwpmProviderContextEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < numEntriesReturned; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        printf("Index:%d.\n", (calloutIndex + 1));

        int n = StringFromGUID2(entries[calloutIndex]->providerContextKey, GUID, MAX_PATH);
        assert(n);
        printf("providerContextKey:%ws.\n", GUID);

        printf("displayData.name:%ws.\n", entries[calloutIndex]->displayData.name);
        printf("displayData.description:%ws.\n", entries[calloutIndex]->displayData.description);
        printf("flags:0x%x.\n", entries[calloutIndex]->flags);

        printf("还有很多的信息，这里就暂时不打印了.\n");

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmProviderContextDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumSession(HANDLE engineHandle)
{
    FWPM_SESSION_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_SESSION0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmSessionCreateEnumHandle0(engineHandle, &enumTemplate/* NULL*/, &enumHandle);//
    assert(ERROR_SUCCESS == result);

    result = FwpmSessionEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < numEntriesReturned; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        printf("Index:%d.\n", (calloutIndex + 1));

        int n = StringFromGUID2(entries[calloutIndex]->sessionKey, GUID, MAX_PATH);
        assert(n);
        printf("sessionKey:%ws.\n", GUID);

        if (entries[calloutIndex]->displayData.name) {
            printf("displayData.name:%ws.\n", entries[calloutIndex]->displayData.name);
        }

        if (entries[calloutIndex]->displayData.description) {
            printf("displayData.description:%ws.\n", entries[calloutIndex]->displayData.description);
        }

        printf("txnWaitTimeoutInMSec:0x%x.\n", entries[calloutIndex]->txnWaitTimeoutInMSec);
        printf("processId:0x%x.\n", entries[calloutIndex]->processId);
        printf("SID的信息不打印了.\n");
        printf("username:%ws.\n", entries[calloutIndex]->username);
        printf("kernelMode:0x%x.\n", entries[calloutIndex]->kernelMode);//0xffff8000，这是啥意思？

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmSessionDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumSubLayer(HANDLE engineHandle)
{
    FWPM_SUBLAYER_ENUM_TEMPLATE0 enumTemplate = {0};
    HANDLE enumHandle;
    FWPM_SUBLAYER0 ** entries;
    UINT32 numEntriesReturned;

    DWORD result = FwpmSubLayerCreateEnumHandle0(engineHandle, &enumTemplate/* NULL*/, &enumHandle);//
    assert(ERROR_SUCCESS == result);

    result = FwpmSubLayerEnum0(engineHandle, enumHandle, INFINITE, &entries, &numEntriesReturned);
    assert(ERROR_SUCCESS == result);

    for (UINT32 calloutIndex = 0; calloutIndex < numEntriesReturned; calloutIndex++) {
        wchar_t GUID[MAX_PATH] = {0};

        printf("Index:%d.\n", (calloutIndex + 1));

        int n = StringFromGUID2(entries[calloutIndex]->subLayerKey, GUID, MAX_PATH);
        assert(n);
        printf("subLayerKey:%ws.\n", GUID);

        if (entries[calloutIndex]->displayData.name) {
            printf("displayData.name:%ws.\n", entries[calloutIndex]->displayData.name);
        }

        if (entries[calloutIndex]->displayData.description) {
            printf("displayData.description:%ws.\n", entries[calloutIndex]->displayData.description);
        }

        printf("flags:0x%x.\n", entries[calloutIndex]->flags);

        if (entries[calloutIndex]->providerKey) {
            n = StringFromGUID2(*entries[calloutIndex]->providerKey, GUID, MAX_PATH);
            assert(n);
            printf("providerKey:%ws.\n", GUID);
        }

        if (entries[calloutIndex]->providerData.size) {
            printf("displayData.name:%hs.\n", entries[calloutIndex]->providerData.data);
        }

        printf("weight:0x%x.\n", entries[calloutIndex]->weight);

        printf("\n");
    }

    FwpmFreeMemory((VOID **)&entries);

    result = FwpmSubLayerDestroyEnumHandle0(engineHandle, enumHandle);
    assert(ERROR_SUCCESS == result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID Usage(__in const wchar_t * name)
/*++
Routine Description:
    This routine prints the intended usage for this program.
Arguments:
    progName - NULL terminated string representing the name of the executable
--*/
{
    wprintf(L"Usage %s: XXX.\n", name);
    wprintf(L"Callout.\n");
    wprintf(L"NetEvent.\n");
    wprintf(L"Layer.\n");
    wprintf(L"Provider.\n");
    wprintf(L"ProviderContext.\n");
    wprintf(L"Session.\n");
    wprintf(L"SubLayer.\n");
    wprintf(L"all.\n");
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void EnumWfpInfo(int argc, _TCHAR * argv[])
{
    setlocale(LC_CTYPE, ".936");

    HANDLE engineHandle = NULL;
    FWPM_SESSION0 session;
    memset(&session, 0, sizeof(session));
    session.displayData.name = (wchar_t *)L"SDK Examples";
    session.txnWaitTimeoutInMSec = INFINITE;
    session.flags = FWPM_SESSION_FLAG_DYNAMIC;
    DWORD result = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &engineHandle);
    assert(ERROR_SUCCESS == result);

    if (argc == 2 && lstrcmpi(argv[1], TEXT("Callout")) == 0) {
        EnumCallout(engineHandle);//信息不少。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("NetEvent")) == 0) {
        EnumNetEvent(engineHandle);//信息太多。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Filter")) == 0) {
        EnumFilter(engineHandle);//信息很多很多。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Layer")) == 0) {
        EnumLayer(engineHandle);//信息不少。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Provider")) == 0) {
        EnumProvider(engineHandle);//这个信息不多。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("ProviderContext")) == 0) {
        EnumProviderContext(engineHandle);//这个信息不多。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Session")) == 0) {
        EnumSession(engineHandle);//这个信息不多。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("SubLayer")) == 0) {
        EnumSubLayer(engineHandle);//这个信息不多。
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("all")) == 0) {
        EnumCallout(engineHandle);//信息不少。
        EnumNetEvent(engineHandle);//信息太多。
        EnumFilter(engineHandle);//信息很多很多。
        EnumLayer(engineHandle);//信息不少。
        EnumProvider(engineHandle);//这个信息不多。
        EnumProviderContext(engineHandle);//这个信息不多。
        EnumSession(engineHandle);//这个信息不多。
        EnumSubLayer(engineHandle);//这个信息不多。
    } else {
        Usage(argv[0]);
    }

    FwpmEngineClose0(engineHandle);
}
