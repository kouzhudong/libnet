#include "pch.h"
#include "Adapter.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
int WINAPI EnumAdaptersAddressesInfo(int argc, char ** argv)
/*
功能：枚举每个网卡的信息。
      其实，这里好多信息都没打印。

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

    ULONG family = AF_UNSPEC;// default to unspecified address family (both)
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

    if (argc != 2) {
        printf(" Usage: getadapteraddresses family\n");
        printf("        getadapteraddresses 4 (for IPv4)\n");
        printf("        getadapteraddresses 6 (for IPv6)\n");
        printf("        getadapteraddresses A (for both IPv4 and IPv6)\n");
        exit(1);
    }

    if (atoi(argv[1]) == 4)
        family = AF_INET;
    else if (atoi(argv[1]) == 6)
        family = AF_INET6;

    printf("Calling GetAdaptersAddresses function with family = ");
    if (family == AF_INET)
        printf("AF_INET\n");
    if (family == AF_INET6)
        printf("AF_INET6\n");
    if (family == AF_UNSPEC)
        printf("AF_UNSPEC\n\n");

    outBufLen = WORKING_BUFFER_SIZE;// Allocate a 15 KB buffer to start with.

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            exit(1);
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
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
                    switch (pUnicast->Address.lpSockaddr->sa_family) {
                    case AF_INET:
                    {
                        PSOCKADDR_IN sockaddr_ipv4 = (PSOCKADDR_IN)pUnicast->Address.lpSockaddr;

                        printf("\tIPv4:%s\n", inet_ntoa(sockaddr_ipv4->sin_addr));

                        break;
                    }
                    case AF_INET6:
                    {
                        DWORD ipbufferlength = 46;
                        char ipstringbuffer[46] = {0};

                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)pUnicast->Address.lpSockaddr;
                        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

                        printf("\tIPv6:%s\n", ipstringbuffer);

                        break;
                    }
                    default:
                        _ASSERTE(false);
                        break;
                    }

                    pUnicast = pUnicast->Next;
                }
                printf("\tNumber of Unicast Addresses: %d\n", i);
            } else
                printf("\tNo Unicast Addresses\n");

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != NULL; i++) {
                    switch (pAnycast->Address.lpSockaddr->sa_family) {
                    case AF_INET:
                    {
                        PSOCKADDR_IN sockaddr_ipv4 = (PSOCKADDR_IN)pAnycast->Address.lpSockaddr;

                        printf("\tAnycast:%s\n", inet_ntoa(sockaddr_ipv4->sin_addr));

                        break;
                    }
                    case AF_INET6:
                    {
                        DWORD ipbufferlength = 46;
                        char ipstringbuffer[46] = {0};

                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)pAnycast->Address.lpSockaddr;
                        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

                        printf("\tAnycast:%s\n", ipstringbuffer);

                        break;
                    }
                    default:
                        _ASSERTE(false);
                        break;
                    }

                    pAnycast = pAnycast->Next;
                }
                printf("\tNumber of Anycast Addresses: %d\n", i);
            } else {
                printf("\tNo Anycast Addresses\n");
            }

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != NULL; i++) {
                    switch (pMulticast->Address.lpSockaddr->sa_family) {
                    case AF_INET:
                    {
                        PSOCKADDR_IN sockaddr_ipv4 = (PSOCKADDR_IN)pMulticast->Address.lpSockaddr;

                        printf("\tMulticast:%s\n", inet_ntoa(sockaddr_ipv4->sin_addr));

                        break;
                    }
                    case AF_INET6:
                    {
                        DWORD ipbufferlength = 46;
                        char ipstringbuffer[46] = {0};

                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)pMulticast->Address.lpSockaddr;
                        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

                        printf("\tMulticast:%s\n", ipstringbuffer);

                        break;
                    }
                    default:
                        _ASSERTE(false);
                        break;
                    }

                    pMulticast = pMulticast->Next;
                }
                printf("\tNumber of Multicast Addresses: %d\n", i);
            } else {
                printf("\tNo Multicast Addresses\n");
            }

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != NULL; i++) {
                    switch (pDnServer->Address.lpSockaddr->sa_family) {
                    case AF_INET:
                    {
                        PSOCKADDR_IN sockaddr_ipv4 = (PSOCKADDR_IN)pDnServer->Address.lpSockaddr;

                        printf("\tDnsServer:%s\n", inet_ntoa(sockaddr_ipv4->sin_addr));

                        break;
                    }
                    case AF_INET6:
                    {
                        DWORD ipbufferlength = 46;
                        char ipstringbuffer[46] = {0};

                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)pDnServer->Address.lpSockaddr;
                        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

                        printf("\tDnsServer:%s\n", ipstringbuffer);

                        break;
                    }
                    default:
                        _ASSERTE(false);
                        break;
                    }

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

                    FirstWinsServerAddress = FirstWinsServerAddress->Next;
                }
                printf("\tNumber of Wins Server: %d\n", i);
            } else {
                printf("\tNumber of Wins Server: 0\n");//好像都是这个。
            }

            PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress = pCurrAddresses->FirstGatewayAddress;
            if (FirstGatewayAddress) {
                for (i = 0; FirstGatewayAddress != NULL; i++) {
                    //打印 Gateway Address。
                    switch (FirstGatewayAddress->Address.lpSockaddr->sa_family) {
                    case AF_INET:
                    {
                        PSOCKADDR_IN sockaddr_ipv4 = (PSOCKADDR_IN)FirstGatewayAddress->Address.lpSockaddr;

                        printf("\tGatewayAddress:%s\n", inet_ntoa(sockaddr_ipv4->sin_addr));

                        break;
                    }
                    case AF_INET6:
                    {
                        DWORD ipbufferlength = 46;
                        char ipstringbuffer[46] = {0};

                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)FirstGatewayAddress->Address.lpSockaddr;
                        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

                        printf("\tGatewayAddress:%s\n", ipstringbuffer);

                        break;
                    }
                    default:
                        _ASSERTE(false);
                        break;
                    }

                    FirstGatewayAddress = FirstGatewayAddress->Next;
                }
                printf("\tNumber of Gateway: %d\n", i);
            } else {
                printf("\tNumber of Gateway: 0\n");//好像都是这个。
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
                exit(1);
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
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                printf("Other\n");
                break;
            case MIB_IF_TYPE_ETHERNET:
                printf("Ethernet\n");
                break;
            case MIB_IF_TYPE_TOKENRING:
                printf("Token Ring\n");
                break;
            case MIB_IF_TYPE_FDDI:
                printf("FDDI\n");
                break;
            case MIB_IF_TYPE_PPP:
                printf("PPP\n");
                break;
            case MIB_IF_TYPE_LOOPBACK:
                printf("Lookback\n");
                break;
            case MIB_IF_TYPE_SLIP:
                printf("Slip\n");
                break;
            default:
                printf("Unknown type %ld\n", pAdapter->Type);
                break;
            }

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


//////////////////////////////////////////////////////////////////////////////////////////////////
