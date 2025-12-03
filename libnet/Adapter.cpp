#include "pch.h"
#include "Adapter.h"
#include "IpHelper.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void DumpAddress(const char * Msg, PSOCKET_ADDRESS Address)
{
    switch (Address->lpSockaddr->sa_family) {
    case AF_INET: {
        const PSOCKADDR_IN sockaddr_ipv4 = reinterpret_cast<const PSOCKADDR_IN>(Address->lpSockaddr);

        printf("\t%s:%s\n", Msg, inet_ntoa(sockaddr_ipv4->sin_addr));

        break;
    }
    case AF_INET6: {
        const DWORD ipbufferlength = 46;
        char ipstringbuffer[46] = {0};

        const PSOCKADDR_IN6_LH sa_in6 = reinterpret_cast<const PSOCKADDR_IN6_LH>(Address->lpSockaddr);
        inet_ntop(AF_INET6, &sa_in6->sin6_addr, ipstringbuffer, ipbufferlength);

        printf("\t%s:%s\n", Msg, ipstringbuffer);

        break;
    }
    default:
        _ASSERTE(false);
        break;
    }
}


void WINAPI GetPerAdapterInfoEx(ULONG IfIndex)
/*
GetPerAdapterInfo 函数检索与指定接口对应的适配器的相关信息。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getperadapterinfo
https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-getperadapterinfo
*/
{
    IP_PER_ADAPTER_INFO PerAdapterInfo = {};
    ULONG OutBufLen = sizeof(IP_PER_ADAPTER_INFO);
    DWORD ret = GetPerAdapterInfo(IfIndex, &PerAdapterInfo, &OutBufLen);
    if (ERROR_NO_DATA == ret) {
        return;
    }

    _ASSERTE(ERROR_BUFFER_OVERFLOW == ret);

    auto pPerAdapterInfo = static_cast<PIP_PER_ADAPTER_INFO>(MALLOC(OutBufLen));
    _ASSERTE(pPerAdapterInfo);

    ret = GetPerAdapterInfo(IfIndex, pPerAdapterInfo, &OutBufLen);
    _ASSERTE(ERROR_SUCCESS == ret);

    //这里也没啥东西，内容大多为NULL。

    FREE(pPerAdapterInfo);
}


EXTERN_C
DLLEXPORT
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family)
/*
功能：枚举每个网卡的信息(IPv4 or/and IPv6)。
      其实，这里好多信息都没打印。

Parameters：
[in] Family
The values currently supported are AF_INET, AF_INET6, and AF_UNSPEC.

addresses associated with the adapters

This example retrieves the IP_ADAPTER_ADDRESSES structure for the adapters associated with the system and prints some members for each adapter interface.

The GetAdaptersAddresses function retrieves the addresses associated with the adapters on the local computer.
https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
*/
{
    /* Declare and initialize variables */
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_WINS_INFO | GAA_FLAG_INCLUDE_GATEWAYS |
                  GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_ALL_COMPARTMENTS |
                  GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;

    PIP_ADAPTER_ADDRESSES pAddresses = nullptr;
    ULONG outBufLen = WORKING_BUFFER_SIZE; // Allocate a 15 KB buffer to start with.
    ULONG Iterations = 0;

    printf("Calling GetAdaptersAddresses function with family = ");
    if (Family == AF_INET)
        printf("AF_INET\n");
    if (Family == AF_INET6)
        printf("AF_INET6\n");
    if (Family == AF_UNSPEC)
        printf("AF_UNSPEC\n\n");

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == nullptr) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            return (1);
        }

        dwRetVal = GetAdaptersAddresses(Family, flags, nullptr, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = nullptr;
        } else {
            break;
        }

        Iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    PIP_ADAPTER_ADDRESSES pCurrAddresses = nullptr;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = nullptr;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = nullptr;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = nullptr;
    IP_ADAPTER_DNS_SERVER_ADDRESS * pDnServer = nullptr;
    IP_ADAPTER_PREFIX * pPrefix = nullptr;

    if (dwRetVal == NO_ERROR) { // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            printf("\tLength of the IP_ADAPTER_ADDRESS struct: %lu\n", pCurrAddresses->Length);
            printf("\tIfIndex (IPv4 interface): %lu\n", pCurrAddresses->IfIndex);
            printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

            GetPerAdapterInfoEx(pCurrAddresses->IfIndex);

            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != nullptr) {
                for (i = 0; pUnicast != nullptr; i++) {
                    DumpAddress("ip(Unicast)", &pUnicast->Address); /*这里可以打印IP地址信息*/

                    pUnicast = pUnicast->Next;
                }
                printf("\tNumber of Unicast Addresses: %u\n", i);
            } else {
                printf("\tNo Unicast Addresses\n");
            }

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != nullptr; i++) {
                    DumpAddress("Anycast", &pAnycast->Address);

                    pAnycast = pAnycast->Next;
                }
                printf("\tNumber of Anycast Addresses: %u\n", i);
            } else {
                printf("\tNo Anycast Addresses\n");
            }

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != nullptr; i++) {
                    DumpAddress("Multicast", &pMulticast->Address);

                    pMulticast = pMulticast->Next;
                }
                printf("\tNumber of Multicast Addresses: %u\n", i);
            } else {
                printf("\tNo Multicast Addresses\n");
            }

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != nullptr; i++) {
                    DumpAddress("DnServer", &pDnServer->Address);

                    pDnServer = pDnServer->Next;
                }
                printf("\tNumber of DNS Server Addresses: %u\n", i);
            } else {
                printf("\tNo DNS Server Addresses\n");
            }

            printf("\tDNS Suffix: %wS\n", pCurrAddresses->DnsSuffix);
            printf("\tDescription: %wS\n", pCurrAddresses->Description);
            printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);

            if (pCurrAddresses->PhysicalAddressLength != 0) {
                printf("\tPhysical address: ");
                for (i = 0; i < pCurrAddresses->PhysicalAddressLength; i++) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1))
                        printf("%.2X\n", (int)pCurrAddresses->PhysicalAddress[i]);
                    else
                        printf("%.2X-", (int)pCurrAddresses->PhysicalAddress[i]);
                }
            }

            printf("\tFlags: %lu\n", pCurrAddresses->Flags);
            printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
            printf("\tIfType: %lu\n", pCurrAddresses->IfType);
            printf("\tOperStatus: %d\n", pCurrAddresses->OperStatus);
            printf("\tIpv6IfIndex (IPv6 interface): %lu\n", pCurrAddresses->Ipv6IfIndex);
            printf("\tZoneIndices (hex): ");
            for (i = 0; i < 16; i++)
                printf("%lx ", pCurrAddresses->ZoneIndices[i]);
            printf("\n");

            GetPerAdapterInfoEx(pCurrAddresses->Ipv6IfIndex);

            printf("\tTransmit link speed: %I64u\n", pCurrAddresses->TransmitLinkSpeed);
            printf("\tReceive link speed: %I64u\n", pCurrAddresses->ReceiveLinkSpeed);

            pPrefix = pCurrAddresses->FirstPrefix;
            if (pPrefix) {
                for (i = 0; pPrefix != nullptr; i++) {
                    DumpAddress("Prefix", &pPrefix->Address);

                    pPrefix = pPrefix->Next;
                }
                printf("\tNumber of IP Adapter Prefix entries: %u\n", i);
            } else {
                printf("\tNumber of IP Adapter Prefix entries: 0\n");
            }

            PIP_ADAPTER_WINS_SERVER_ADDRESS_LH FirstWinsServerAddress = pCurrAddresses->FirstWinsServerAddress;
            if (FirstWinsServerAddress) {
                for (i = 0; FirstWinsServerAddress != nullptr; i++) {
                    //打印 Wins Server Address。
                    DumpAddress("WinsServerAddress", &FirstWinsServerAddress->Address);

                    FirstWinsServerAddress = FirstWinsServerAddress->Next;
                }
                printf("\tNumber of Wins Server: %u\n", i);
            } else {
                printf("\tNumber of Wins Server: 0\n");
            }

            PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress = pCurrAddresses->FirstGatewayAddress;
            if (FirstGatewayAddress) {
                for (i = 0; FirstGatewayAddress != nullptr; i++) {
                    DumpAddress("GatewayAddress", &FirstGatewayAddress->Address); //打印 Gateway Address。

                    FirstGatewayAddress = FirstGatewayAddress->Next;
                }
                printf("\tNumber of Gateway: %u\n", i);
            } else {
                printf("\tNumber of Gateway: 0\n");
            }

            printf("\n");

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %lu\n", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {
            DisplayError(dwRetVal);
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumAdaptersInfo()
/*
This example retrieves the adapter information and prints various properties of each adapter.
The GetAdaptersInfo function retrieves adapter information for the local computer.
On Windows XP and later:  Use the GetAdaptersAddresses function instead of GetAdaptersInfo.

Remarks
1.The GetAdaptersInfo function can retrieve information only for IPv4 addresses.
  GetAdaptersInfo 函数只能检索 IPv4 地址的信息。
2.The GetAdaptersInfo and GetInterfaceInfo functions do not return information about the IPv4 loopback interface.
  Information on the loopback interface is returned by the GetIpAddrTable function.
3.谨记上面的两个局限。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo
*/
{
    // It is possible for an adapter to have multiple IPv4 addresses, gateways, and secondary WINS servers assigned to the adapter.
    // Note that this sample code only prints out the first entry for the IP address/mask, and gateway, and the primary and secondary WINS server for each adapter.

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == nullptr) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return 1;
    }

    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == nullptr) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return 1;
        }
    }

    DWORD dwRetVal = 0;
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter) {
            printf("\tComboIndex: \t%lu\n", pAdapter->ComboIndex);
            printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            printf("\tAdapter Addr: \t");

            for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }

            printf("\tIndex: \t%lu\n", pAdapter->Index);
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

                /* variables used to print DHCP time info */
                tm newtime{};
                char buffer[32]{};

                printf("\t  Lease Obtained: ");
                /* Display local time */
                errno_t error = _localtime32_s(&newtime, (__time32_t *)&pAdapter->LeaseObtained);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    error = asctime_s(buffer, 32, &newtime); // Convert to an ASCII representation
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }

                printf("\t  Lease Expires:  ");
                error = _localtime32_s(&newtime, (__time32_t *)&pAdapter->LeaseExpires);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    error = asctime_s(buffer, 32, &newtime); // Convert to an ASCII representation
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else /* asctime_s returns the string terminated by \n\0 */
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
        printf("GetAdaptersInfo failed with error: %lu\n", dwRetVal);
    }

    if (pAdapterInfo)
        FREE(pAdapterInfo);

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumInterfaceInfo()
/*
GetInterfaceInfo 函数获取本地系统上启用了 IPv4 的网络接口适配器列表。

https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-getinterfaceinfo
*/
{
    // Declare and initialize variables
    PIP_INTERFACE_INFO pInfo{};
    ULONG ulOutBufLen = 0;
    int iReturn = 1;

    // Make an initial call to GetInterfaceInfo to get the necessary size in the ulOutBufLen variable
    DWORD dwRetVal = GetInterfaceInfo(nullptr, &ulOutBufLen);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        pInfo = (IP_INTERFACE_INFO *)MALLOC(ulOutBufLen);
        if (pInfo == nullptr) {
            printf("Unable to allocate memory needed to call GetInterfaceInfo\n");
            return 1;
        }
    }

    // Make a second call to GetInterfaceInfo to get the actual data we need
    dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen);
    if (dwRetVal == NO_ERROR) {
        DWORD NumIf = 0;
        iReturn = GetNumberOfInterfaces(&NumIf);
        //经测试：NumIf > pInfo->NumAdapters.

#pragma prefast(push)
#pragma prefast(disable : 6011, "取消对 NULL 指针“pInfo”的引用")
        printf("Number of Adapters: %ld\n\n", pInfo->NumAdapters);
#pragma prefast(pop)

        for (int i = 0; i < pInfo->NumAdapters; i++) {
            printf("Adapter Index[%d]: %lu\n", i, pInfo->Adapter[i].Index);
            printf("Adapter Name[%d]: %ws\n\n", i, pInfo->Adapter[i].Name);
        }
        iReturn = 0;
    } else if (dwRetVal == ERROR_NO_DATA) {
        printf("There are no network adapters with IPv4 enabled on the local system\n");
        iReturn = 0;
    } else {
        printf("GetInterfaceInfo failed with error: %lu\n", dwRetVal);
        iReturn = 1;
    }

    FREE(pInfo);
    return (iReturn);
}


EXTERN_C
DLLEXPORT
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway)
/*
功能：获取本地IPv4地址的默认网关地址（也是IPv4，不包括IPv6）。
*/
{
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == nullptr) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return 1;
    }

    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == nullptr) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return 1;
        }
    }

    DWORD dwRetVal = 0;
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (_stricmp(pAdapter->IpAddressList.IpAddress.String, IPv4) == 0) {
                lstrcpyA(Gateway, pAdapter->GatewayList.IpAddress.String);
                break;
            }

            pAdapter = pAdapter->Next;
        }
    } else {
        printf("GetAdaptersInfo failed with error: %lu\n", dwRetVal);
    }

    if (pAdapterInfo) {
        FREE(pAdapterInfo);
    }

    return 0;
}


EXTERN_C
DLLEXPORT
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
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_WINS_INFO | GAA_FLAG_INCLUDE_GATEWAYS |
                  GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_ALL_COMPARTMENTS | GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;

    PIP_ADAPTER_ADDRESSES pAddresses = nullptr;
    ULONG outBufLen = WORKING_BUFFER_SIZE; // Allocate a 15 KB buffer to start with.
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = nullptr;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = nullptr;

    IN6_ADDR sin6_addr{};
    InetPtonA(AF_INET6, IPv6, &sin6_addr);

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == nullptr) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            return (1);
        }

        dwRetVal = GetAdaptersAddresses(AF_INET6, flags, nullptr, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = nullptr;
        } else {
            break;
        }

        Iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != nullptr) {
                for (i = 0; pUnicast != nullptr; i++) {
                    switch (pUnicast->Address.lpSockaddr->sa_family) {
                    case AF_INET6: {
                        PSOCKADDR_IN6_LH sa_in6 = (PSOCKADDR_IN6_LH)pUnicast->Address.lpSockaddr;
                        if (IN6_ADDR_EQUAL(&sin6_addr, &sa_in6->sin6_addr)) {

                            PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress = pCurrAddresses->FirstGatewayAddress;
                            if (FirstGatewayAddress) {
                                for (i = 0; FirstGatewayAddress != nullptr; i++) {

                                    switch (FirstGatewayAddress->Address.lpSockaddr->sa_family) {
                                    case AF_INET6: {
                                        DWORD ipbufferlength = 46;
                                        char ipstringbuffer[46] = {0};

                                        PSOCKADDR_IN6_LH temp = (PSOCKADDR_IN6_LH)FirstGatewayAddress->Address.lpSockaddr;
                                        inet_ntop(AF_INET6, &temp->sin6_addr, ipstringbuffer, ipbufferlength);

                                        lstrcpyA(Gateway, ipstringbuffer); //会有多个，取最后一个。

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
        printf("Call to GetAdaptersAddresses failed with error: %lu\n", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {
            DisplayError(dwRetVal);
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI GetGatewayMacByIPv6(const char * IPv6, PDL_EUI48 GatewayMac)
/*
功能：获取一个本地IPv6的一个默认网关的MAC.

参数：
1.IPv6是本地的IPv6地址（兼容%符号），可以取如下值：
  IPv6 地址 . . . . . . . . . . . . : 240e:471:810:2006:189e:961c:bcb0:246b(首选)
  临时 IPv6 地址. . . . . . . . . . : 240e:471:810:2006:89b7:f4a2:9406:a776(首选)
  本地链接 IPv6 地址. . . . . . . . : fe80::189e:961c:bcb0:246b%10(首选)
2.GatewayMac容纳下一个MAC地址。

注意：
1.有的interface/NIC/ipv6没有对应的网关。
2.改进的思路是用Icmp6SendEcho2发送邻居报文。
3.最好的思路是用ResolveIpNetEntry2实现。
*/
{
    char Gateway[128];
    GetGatewayByIPv6(IPv6, Gateway);

    GetMacByGatewayIPv6(Gateway, GatewayMac);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
