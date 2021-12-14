#include "pch.h"
#include "Adapter.h"
#include "IpHelper.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
附上我很久以前写的汇编代码。
保留纪念吧！

;made by correy
;rc me.rc
;ml /coff test.asm /link /subsystem:windows me.res
;心得：不要高谈阔论，唯行之之难矣！
.386
.model flat,stdcall
option casemap:none
include windows.inc
include kernel32.inc
include user32.inc
include iphlpapi.inc
includelib user32.lib
includelib kernel32.lib
includelib iphlpapi.lib

.data?
buffer db 9*280h dup (?);最多显示9个，如想得到恰当的大小请先一错误运行一下GetAdaptersInfo。
buffer2 db 256 dup (?)

.data
correy db "made by correy",0
x dd 9*280h
formatmac db "%02X-%02X-%02X-%02X-%02X-%02X",13,10,0

x1 dd ?
x2 dd ?
x3 dd ?
x4 dd ?
x5 dd ?
x6 dd ?

.code

start:

invoke GetAdaptersInfo,addr buffer,addr x
;198h=408

lea esi,buffer;buffer的值不可以改变。

again:
mov eax,0
mov al,byte ptr [esi+404]
mov x1,eax

mov eax,0
mov al,byte ptr [esi+405]
mov x2,eax

mov eax,0
mov al,byte ptr [esi+406]
mov x3,eax

mov eax,0
mov al,byte ptr [esi+407]
mov x4,eax

mov eax,0
mov al,byte ptr [esi+408]
mov x5,eax

mov eax,0
mov al,byte ptr [esi+409]
mov x6,eax

mov eax,dword ptr [esi+8];mac的描述。
.if  eax!=0;必须用寄存器等，不能用变量名或内存。
;也可以辨别x1==0 &&  x2==0 && x3==0 && x4==0 && x5==0 && x6==0等。
;要显示别的也可以呀！如名字，ip，网关等。
invoke   wsprintf,addr buffer2,addr formatmac,x1,x2,x3,x4,x5,x6
invoke MessageBox,0,addr buffer2,addr correy,0;我显示的好像少一个。

add esi,280h

jmp again
.endif

invoke ExitProcess,NULL
end start
;made at 2011.01.31
*/


//////////////////////////////////////////////////////////////////////////////////////////////////


void DumpAddress(const char * Msg, PSOCKET_ADDRESS Address)
{
    switch (Address->lpSockaddr->sa_family) {
    case AF_INET:
    {
        PSOCKADDR_IN sockaddr_ipv4 = (PSOCKADDR_IN)Address->lpSockaddr;

        printf("\t%s:%s\n", Msg, inet_ntoa(sockaddr_ipv4->sin_addr));

        break;
    }
    case AF_INET6:
    {
        DWORD ipbufferlength = 46;
        char ipstringbuffer[46] = {0};

        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)Address->lpSockaddr;
        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

        printf("\t%s:%s\n", Msg, ipstringbuffer);

        break;
    }
    default:
        _ASSERTE(false);
        break;
    }
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family)
/*
功能：枚举每个网卡的信息(IPv4 or/and IPv6)。
      其实，这里好多信息都没打印。

Parameters：
[in] Family
The values currently supported are AF_INET, AF_INET6, and AF_UNSPEC.

addresses associated with the adapters

This example retrieves the IP_ADAPTER_ADDRESSES structure for the adapters associated with the system and
prints some members for each adapter interface.

The GetAdaptersAddresses function retrieves the addresses associated with the adapters on the local computer.
https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
*/
{
    /* Declare and initialize variables */
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses  
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX |
        GAA_FLAG_INCLUDE_WINS_INFO |
        GAA_FLAG_INCLUDE_GATEWAYS |
        GAA_FLAG_INCLUDE_ALL_INTERFACES |
        GAA_FLAG_INCLUDE_ALL_COMPARTMENTS |
        GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;

    LPVOID lpMsgBuf = NULL;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS * pDnServer = NULL;
    IP_ADAPTER_PREFIX * pPrefix = NULL;

    printf("Calling GetAdaptersAddresses function with family = ");
    if (Family == AF_INET)
        printf("AF_INET\n");
    if (Family == AF_INET6)
        printf("AF_INET6\n");
    if (Family == AF_UNSPEC)
        printf("AF_UNSPEC\n\n");

    outBufLen = WORKING_BUFFER_SIZE;// Allocate a 15 KB buffer to start with.

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            return(1);
        }

        dwRetVal = GetAdaptersAddresses(Family, flags, NULL, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n", pCurrAddresses->Length);
            printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex);
            printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++) {
                    /*这里可以打印IP地址信息*/
                    DumpAddress("ip(Unicast)", &pUnicast->Address);

                    pUnicast = pUnicast->Next;
                }
                printf("\tNumber of Unicast Addresses: %d\n", i);
            } else
                printf("\tNo Unicast Addresses\n");

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != NULL; i++) {
                    DumpAddress("Anycast", &pAnycast->Address);

                    pAnycast = pAnycast->Next;
                }
                printf("\tNumber of Anycast Addresses: %d\n", i);
            } else {
                printf("\tNo Anycast Addresses\n");
            }

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != NULL; i++) {
                    DumpAddress("Multicast", &pMulticast->Address);

                    pMulticast = pMulticast->Next;
                }
                printf("\tNumber of Multicast Addresses: %d\n", i);
            } else {
                printf("\tNo Multicast Addresses\n");
            }

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != NULL; i++) {
                    DumpAddress("DnServer", &pDnServer->Address);

                    pDnServer = pDnServer->Next;
                }
                printf("\tNumber of DNS Server Addresses: %d\n", i);
            } else {
                printf("\tNo DNS Server Addresses\n");
            }

            printf("\tDNS Suffix: %wS\n", pCurrAddresses->DnsSuffix);
            printf("\tDescription: %wS\n", pCurrAddresses->Description);
            printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);

            if (pCurrAddresses->PhysicalAddressLength != 0) {
                printf("\tPhysical address: ");
                for (i = 0; i < (int)pCurrAddresses->PhysicalAddressLength; i++) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1))
                        printf("%.2X\n", (int)pCurrAddresses->PhysicalAddress[i]);
                    else
                        printf("%.2X-", (int)pCurrAddresses->PhysicalAddress[i]);
                }
            }

            printf("\tFlags: %ld\n", pCurrAddresses->Flags);
            printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
            printf("\tIfType: %ld\n", pCurrAddresses->IfType);
            printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
            printf("\tIpv6IfIndex (IPv6 interface): %u\n", pCurrAddresses->Ipv6IfIndex);
            printf("\tZoneIndices (hex): ");
            for (i = 0; i < 16; i++)
                printf("%lx ", pCurrAddresses->ZoneIndices[i]);
            printf("\n");

            printf("\tTransmit link speed: %I64u\n", pCurrAddresses->TransmitLinkSpeed);
            printf("\tReceive link speed: %I64u\n", pCurrAddresses->ReceiveLinkSpeed);

            pPrefix = pCurrAddresses->FirstPrefix;
            if (pPrefix) {
                for (i = 0; pPrefix != NULL; i++) {
                    DumpAddress("Prefix", &pPrefix->Address);

                    pPrefix = pPrefix->Next;
                }
                printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
            } else {
                printf("\tNumber of IP Adapter Prefix entries: 0\n");
            }

            PIP_ADAPTER_WINS_SERVER_ADDRESS_LH FirstWinsServerAddress = pCurrAddresses->FirstWinsServerAddress;
            if (FirstWinsServerAddress) {
                for (i = 0; FirstWinsServerAddress != NULL; i++) {
                    //打印 Wins Server Address。
                    DumpAddress("WinsServerAddress", &FirstWinsServerAddress->Address);

                    FirstWinsServerAddress = FirstWinsServerAddress->Next;
                }
                printf("\tNumber of Wins Server: %d\n", i);
            } else {
                printf("\tNumber of Wins Server: 0\n");
            }

            PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress = pCurrAddresses->FirstGatewayAddress;
            if (FirstGatewayAddress) {
                for (i = 0; FirstGatewayAddress != NULL; i++) {
                    //打印 Gateway Address。
                    DumpAddress("GatewayAddress", &FirstGatewayAddress->Address);

                    FirstGatewayAddress = FirstGatewayAddress->Next;
                }
                printf("\tNumber of Gateway: %d\n", i);
            } else {
                printf("\tNumber of Gateway: 0\n");
            }

            printf("\n");

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              dwRetVal,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
                              (LPTSTR)&lpMsgBuf,
                              0,
                              NULL)) {
                printf("\tError: %s", (char *)lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
                return(1);
            }
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }

    return 0;
}


void WINAPI GetPerAdapterInfoEx(ULONG IfIndex)
/*

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getperadapterinfo
*/
{
    IP_PER_ADAPTER_INFO PerAdapterInfo = {};
    ULONG               OutBufLen = sizeof(IP_PER_ADAPTER_INFO);
    DWORD ret = GetPerAdapterInfo(IfIndex, &PerAdapterInfo, &OutBufLen);
    _ASSERTE(ERROR_BUFFER_OVERFLOW == ret);

    PIP_PER_ADAPTER_INFO pPerAdapterInfo = (PIP_PER_ADAPTER_INFO)HeapAlloc(GetProcessHeap(),
                                                                           HEAP_ZERO_MEMORY,
                                                                           OutBufLen);
    _ASSERTE(pPerAdapterInfo);

    ret = GetPerAdapterInfo(IfIndex, pPerAdapterInfo, &OutBufLen);
    _ASSERTE(ERROR_SUCCESS == ret);

    //这里也没啥东西，内容大多为NULL。

    HeapFree(GetProcessHeap(), 0, pPerAdapterInfo);
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumAdaptersInfo()
/*
This example retrieves the adapter information and prints various properties of each adapter.

The GetAdaptersInfo function retrieves adapter information for the local computer.

On Windows XP and later:  Use the GetAdaptersAddresses function instead of GetAdaptersInfo.

Remarks
1.The GetAdaptersInfo function can retrieve information only for IPv4 addresses.
2.The GetAdaptersInfo and GetInterfaceInfo functions do not return information about the IPv4 loopback interface. 
  Information on the loopback interface is returned by the GetIpAddrTable function.
3.谨记上面的两个局限。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo
*/
{
    /* Declare and initialize variables */

// It is possible for an adapter to have multiple IPv4 addresses, gateways,
// and secondary WINS servers assigned to the adapter. 
//
// Note that this sample code only prints out the first entry for the IP address/mask,
// and gateway, and the primary and secondary WINS server for each adapter. 

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;

    /* variables used to print DHCP time info */
    struct tm newtime;
    char buffer[32];
    errno_t error;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return 1;
    }

    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return 1;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
            printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            printf("\tAdapter Addr: \t");

            for (i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }

            printf("\tIndex: \t%d\n", pAdapter->Index);
            GetPerAdapterInfoEx(pAdapter->Index);

            printf("\tType: \t");
            PrintAdapterType(pAdapter->Type);

            printf("\tIP Address: \t%s\n", pAdapter->IpAddressList.IpAddress.String);
            printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

            printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
            printf("\t***\n");

            if (pAdapter->DhcpEnabled) {
                printf("\tDHCP Enabled: Yes\n");
                printf("\t  DHCP Server: \t%s\n", pAdapter->DhcpServer.IpAddress.String);

                printf("\t  Lease Obtained: ");
                /* Display local time */
                error = _localtime32_s(&newtime, (__time32_t *)&pAdapter->LeaseObtained);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation 
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }

                printf("\t  Lease Expires:  ");
                error = _localtime32_s(&newtime, (__time32_t *)&pAdapter->LeaseExpires);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation 
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }
            } else
                printf("\tDHCP Enabled: No\n");

            if (pAdapter->HaveWins) {
                printf("\tHave Wins: Yes\n");
                printf("\t  Primary Wins Server:    %s\n", pAdapter->PrimaryWinsServer.IpAddress.String);
                printf("\t  Secondary Wins Server:  %s\n", pAdapter->SecondaryWinsServer.IpAddress.String);
            } else
                printf("\tHave Wins: No\n");

            pAdapter = pAdapter->Next;
            printf("\n");
        }
    } else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }

    if (pAdapterInfo)
        FREE(pAdapterInfo);

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway)
/*
功能：获取本地IPv4地址的默认网关地址（也是IPv4，不包括IPv6）。
*/
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return 1;
    }

    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return 1;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (_stricmp(pAdapter->IpAddressList.IpAddress.String, IPv4) == 0) {
                lstrcpyA(Gateway, pAdapter->GatewayList.IpAddress.String);
                break;
            }

            pAdapter = pAdapter->Next;
        }
    } else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }

    if (pAdapterInfo)
        FREE(pAdapterInfo);

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI GetGatewayByIPv6(const char * IPv6, char * Gateway)
/*
功能：获取本地IPv6地址的(任意一个)默认网关地址（也是IPv6，不包括IPv4）。

参数：
1.IPv6是本地的IPv6地址（兼容%符号），可以取如下值：
  IPv6 地址 . . . . . . . . . . . . : 240e:471:810:2006:189e:961c:bcb0:246b(首选)
  临时 IPv6 地址. . . . . . . . . . : 240e:471:810:2006:89b7:f4a2:9406:a776(首选)
  本地链接 IPv6 地址. . . . . . . . : fe80::189e:961c:bcb0:246b%10(首选)
2.Gateway容纳下一个IPv6地址的字符串。
*/
{
    /* Declare and initialize variables */
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses  
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX |
        GAA_FLAG_INCLUDE_WINS_INFO |
        GAA_FLAG_INCLUDE_GATEWAYS |
        GAA_FLAG_INCLUDE_ALL_INTERFACES |
        GAA_FLAG_INCLUDE_ALL_COMPARTMENTS |
        GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;

    LPVOID lpMsgBuf = NULL;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = WORKING_BUFFER_SIZE;// Allocate a 15 KB buffer to start with.
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    IP_ADAPTER_PREFIX * pPrefix = NULL;

    IN6_ADDR sin6_addr = {0};
    InetPtonA(AF_INET6, IPv6, &sin6_addr);

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            return(1);
        }

        dwRetVal = GetAdaptersAddresses(AF_INET6, flags, NULL, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++) {
                    switch (pUnicast->Address.lpSockaddr->sa_family) {
                    case AF_INET6:
                    {
                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)pUnicast->Address.lpSockaddr;
                        if (IN6_ADDR_EQUAL(&sin6_addr, &sa_in6->sin6_addr)) {

                            PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress = pCurrAddresses->FirstGatewayAddress;
                            if (FirstGatewayAddress) {
                                for (i = 0; FirstGatewayAddress != NULL; i++) {

                                    switch (FirstGatewayAddress->Address.lpSockaddr->sa_family) {
                                    case AF_INET6:
                                    {
                                        DWORD ipbufferlength = 46;
                                        char ipstringbuffer[46] = {0};

                                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)FirstGatewayAddress->Address.lpSockaddr;
                                        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

                                        lstrcpyA(Gateway, ipstringbuffer);//会有多个，取最后一个。

                                        break;
                                    }
                                    default:
                                        _ASSERTE(false);
                                        break;
                                    }

                                    FirstGatewayAddress = FirstGatewayAddress->Next;
                                }
                            }
                        }

                        break;
                    }
                    default:
                        _ASSERTE(false);
                        break;
                    }

                    pUnicast = pUnicast->Next;
                }
            }            

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              dwRetVal,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
                              (LPTSTR)&lpMsgBuf,
                              0,
                              NULL)) {
                printf("\tError: %s", (char *)lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
                return(1);
            }
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI GetGatewayMacByIPv6(const char * IPv6, PBYTE GatewayMac)
/*
功能：获取一个本地IPv6的一个默认网关的MAC.

参数：
1.IPv6是本地的IPv6地址（兼容%符号），可以取如下值：
  IPv6 地址 . . . . . . . . . . . . : 240e:471:810:2006:189e:961c:bcb0:246b(首选)
  临时 IPv6 地址. . . . . . . . . . : 240e:471:810:2006:89b7:f4a2:9406:a776(首选)
  本地链接 IPv6 地址. . . . . . . . : fe80::189e:961c:bcb0:246b%10(首选)
2.GatewayMac容纳下一个MAC地址。
*/
{
    char Gateway[128];
    GetGatewayByIPv6(IPv6, Gateway);

    GetMacByGatewayIPv6(Gateway, GatewayMac);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
