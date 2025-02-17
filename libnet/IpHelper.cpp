#include "pch.h"
#include "IpHelper.h"
#include "Adapter.h"


#pragma warning(disable : 6387)


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI EnumIpAddrTable()
/*
interface–to–IPv4 address mapping table

The GetIpAddrTable function retrieves the interface–to–IPv4 address mapping table.
The following example retrieves the IP address table,
then prints some members of the IP address entries in the table.

https://msdn.microsoft.com/en-us/library/windows/desktop/aa365949(v=vs.85).aspx
https://msdn.microsoft.com/en-us/library/windows/desktop/aa366309(v=vs.85).aspx
*/
{
    /* Variables used by GetIpAddrTable */
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Before calling AddIPAddress we use GetIpAddrTable to get an adapter to which we can add the IP.
    PMIB_IPADDRTABLE pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(sizeof(MIB_IPADDRTABLE));
    if (pIPAddrTable) {
        // Make an initial call to GetIpAddrTable to get the necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(dwSize);
        }

        if (pIPAddrTable == nullptr) {
            printf("Memory allocation failed for GetIpAddrTable\n");
            return (1);
        }
    }

    // Make a second call to GetIpAddrTable to get the actual data we want
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
        printf("GetIpAddrTable failed with error %lu\n", dwRetVal);
        DisplayError(dwRetVal);
        return (1);
    }

    printf("\tNum Entries: %lu\n", pIPAddrTable->dwNumEntries);
    for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
        IN_ADDR IPAddr{};
        printf("\n\tInterface Index[%d]:\t%lu\n", i, pIPAddrTable->table[i].dwIndex);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
        printf("\tIP Address[%d]:     \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwMask;
        printf("\tSubnet Mask[%d]:    \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwBCastAddr;

#pragma warning(push)
#pragma warning(disable : 4476) //"printf": 格式说明符中的类型字段字符“)”未知
        printf("\tBroadCast[%d]:      \t%s (%lu%)\n", i, inet_ntoa(IPAddr), pIPAddrTable->table[i].dwBCastAddr);
#pragma warning(pop)

        printf("\tReassembly size[%d]:\t%lu\n", i, pIPAddrTable->table[i].dwReasmSize);
        printf("\tType and State[%d]:", i);
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
            printf("\tPrimary IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
            printf("\tDynamic IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
            printf("\tAddress is on disconnected interface");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
            printf("\tAddress is being deleted");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
            printf("\tTransient address");
        printf("\n");
    }

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = nullptr;
    }

    return (0);
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIpNetTable()
/*
IPv4 to physical address mapping table

此代码实现类似于arp -a即arp缓存。

The GetIpNetTable function retrieves the IPv4 to physical address mapping table.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipnettable
https://docs.microsoft.com/en-us/windows/win32/iphlp/using-the-address-resolution-protocol

这个微软没有例子，是自己写的。
*/
{
    ULONG SizePointer = 0;
    ULONG ret = GetIpNetTable(nullptr, &SizePointer, TRUE);
    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PMIB_IPNETTABLE IpNetTable = (PMIB_IPNETTABLE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, SizePointer);
    _ASSERTE(IpNetTable);

    ret = GetIpNetTable(IpNetTable, &SizePointer, TRUE);
    _ASSERTE(NO_ERROR == ret); // ERROR_NO_DATA

    printf("Number of IPv4 table entries: %lu\n\n", IpNetTable->dwNumEntries);

    for (DWORD i = 0; i < IpNetTable->dwNumEntries; i++) {
        printf("Index: %02u\t", IpNetTable->table[i].dwIndex); //类似于arp -a的接口。

        in_addr in;
        in.S_un.S_addr = IpNetTable->table[i].dwAddr;
        printf("IPv4:%-16s\t", inet_ntoa(in));

        printf("MAC:"); // Physical Address
        for (DWORD j = 0; j < IpNetTable->table[i].dwPhysAddrLen; j++) {
            if (j == (IpNetTable->table[i].dwPhysAddrLen - 1))
                printf("%.2X\t", (int)IpNetTable->table[i].bPhysAddr[j]);
            else
                printf("%.2X-", (int)IpNetTable->table[i].bPhysAddr[j]);
        }

        PrintArpType(IpNetTable->table[i].Type);
    }

    HeapFree(GetProcessHeap(), 0, IpNetTable);

    return 0;
}


EXTERN_C
DLLEXPORT
BOOL WINAPI GetIPv4ByMac(_In_ PDL_EUI48 Mac, _Inout_ PIN_ADDR IPv4)
/*
功能：根据MAC获取响应的IPv4。

说明：这个是SendARP的逆向操作。

实现：遍历GetIpNetTable（类似于arp -a命令）的结构。

注意：有可能失败哟！
*/
{
    ULONG SizePointer = 0;
    ULONG ret = GetIpNetTable(nullptr, &SizePointer, TRUE);
    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PMIB_IPNETTABLE IpNetTable = (PMIB_IPNETTABLE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, SizePointer);
    _ASSERTE(IpNetTable);

    ret = GetIpNetTable(IpNetTable, &SizePointer, TRUE);
    _ASSERTE(NO_ERROR == ret); // ERROR_NO_DATA

    ret = 0;

    for (DWORD i = 0; i < IpNetTable->dwNumEntries; i++) {
        if (0 == memcmp(&IpNetTable->table[i].bPhysAddr, Mac, sizeof(DL_EUI48))) {
            IPv4->S_un.S_addr = IpNetTable->table[i].dwAddr;
            ret = true;
            break; //有可能是多个。可考虑用stl的string存储。
        }
    }

    HeapFree(GetProcessHeap(), 0, IpNetTable);

    return ret;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIpNetTable2(_In_ ADDRESS_FAMILY Family)
/*
功能：枚举IPv4的路由表或（和）IPv6的邻居表。

看来网络表（NetTable）包含IPv4的路由表（route table）和IPv6的邻居表（neighbor table）。

Parameters：
[in] Family
The values currently supported are AF_INET, AF_INET6, and AF_UNSPEC.

The following example retrieves the IP neighbor table,
then prints the values for IP neighbor row entries in the table.

The GetIpNetTable2 function retrieves the IP neighbor table on the local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getipnettable2
*/
{
    PMIB_IPNET_TABLE2 pipTable = nullptr;
    unsigned long status = GetIpNetTable2(Family, &pipTable);
    if (status != NO_ERROR) {
        printf("GetIpNetTable for IP table returned error: %lu\n", status);
        return (1);
    }

    // Print some variables from the table
    printf("Number of IP table entries: %lu\n\n", pipTable->NumEntries);

    for (ULONG i = 0; (unsigned)i < pipTable->NumEntries; i++) {
        //        printf("Table entry: %d\n", i);

        wchar_t IpStr[46] = {0};
        switch (pipTable->Table[i].Address.Ipv4.sin_family) {
        case AF_INET:
            InetNtop(AF_INET, &pipTable->Table[i].Address.Ipv4.sin_addr, IpStr, _ARRAYSIZE(IpStr));
            break;
        case AF_INET6:
            InetNtop(AF_INET6, &pipTable->Table[i].Address.Ipv6.sin6_addr, IpStr, _ARRAYSIZE(IpStr));
            break;
        default:
            _ASSERTE(false);
            break;
        }
        printf("IP Address[%d]:\t %ls\n", (int)i, IpStr);

        printf("Interface index[%d]:\t\t %lu\n", (int)i, pipTable->Table[i].InterfaceIndex);

        printf("Interface LUID NetLuidIndex[%d]:\t %llu\n", (int)i, pipTable->Table[i].InterfaceLuid.Info.NetLuidIndex);
        printf("Interface LUID IfType[%d]: ", (int)i);
        PrintInterfaceType(pipTable->Table[i].InterfaceLuid.Info.IfType);

        printf("Physical Address[%d]:\t ", (int)i);
        if (pipTable->Table[i].PhysicalAddressLength == 0)
            printf("\n");
        //        for (j = 0; (unsigned) j < pipTable->Table[i].PhysicalAddressLength; j++)
        //         printf ("%c"
        for (ULONG j = 0; j < pipTable->Table[i].PhysicalAddressLength; j++) {
            if (j == (pipTable->Table[i].PhysicalAddressLength - 1))
                printf("%.2X\n", (int)pipTable->Table[i].PhysicalAddress[j]);
            else
                printf("%.2X-", (int)pipTable->Table[i].PhysicalAddress[j]);
        }

        printf("Physical Address Length[%d]:\t %lu\n", (int)i, pipTable->Table[i].PhysicalAddressLength);

        printf("Neighbor State[%d]:\t ", (int)i);
        PrintNeighborState(pipTable->Table[i].State);

        printf("Flags[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].Flags);
        printf("IsRouter[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].IsRouter);
        printf("IsUnreachable[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].IsUnreachable);

        printf("ReachabilityTime[%d]:\t %lu, %lu\n\n", 
               (int)i,
               pipTable->Table[i].ReachabilityTime.LastReachable,
               pipTable->Table[i].ReachabilityTime.LastUnreachable);

        status = ResolveIpNetEntry2(&pipTable->Table[i], nullptr); //此函数会改变LastUnreachable的值。
    }

    FreeMibTable(pipTable);
    pipTable = nullptr;
    return 0;
}


EXTERN_C
DLLEXPORT
void WINAPI GetMacByIPv6(const char * IPv6, PBYTE Mac)
{
    MIB_IPNET_ROW2 Row{};

    Row.Address.si_family = AF_INET6;
    InetPtonA(AF_INET6, IPv6, &Row.Address.Ipv6.sin6_addr);

    NTSTATUS status = ResolveIpNetEntry2(&Row, nullptr);
    if (NO_ERROR != status) {
        printf("%ld\n", status);
        return;
    }

    RtlCopyMemory(Mac, Row.PhysicalAddress, Row.PhysicalAddressLength);
}


EXTERN_C
DLLEXPORT
void WINAPI ResolveIpNetEntry2Test(const char * ip)
/*
功能：获取IP地址的一些信息（主要是MAC）。

参数：
ip的取值可以是：
   1.IPv6 地址 . . . . . . . . . . . . : 240e:473:800:3d64:bdd2:6c5:62e5:c423
   2.临时 IPv6 地址. . . . . . . . . . : 240e:473:800:3d64:6957:b225:3185:1f75
   3.本地链接 IPv6 地址. . . . . . . . : fe80::bdd2:6c5:62e5:c423%10
   4.IPv4 地址 . . . . . . . . . . . . : 192.168.42.21
   5.默认网关. . . . . . . . . . . . . : fe80::ac72:b0ff:fe68:99b%10
                                         192.168.42.129
但不可以是2001:4860:4860::6464和8.8.8.8之类的互联网地址（理论上也不应该是），否者返回0x43，而且时间还超慢.

注意：
1.对于IPv6的网关地址不要带%符号。否则，返回0x57（参数错误），可能是某个成员没有设置。
2.对于IPv6的网关地址，操作之前最好先ping一下。
3.本地链接 IPv6 地址不要带%符号。否则，返回0x57（参数错误），可能是某个成员没有设置。

The ResolveIpNetEntry2 function resolves the physical address for a neighbor IP address entry on the local
computer.

The ResolveIpNetEntry2 function is used to resolve the physical address for a neighbor IP address entry on a local
computer. This function flushes any existing neighbor entry that matches the IP address on the interface and then
resolves the physical address (MAC) address by sending ARP requests for an IPv4 address or neighbor solicitation
requests for an IPv6 address.

In addition, at least one of the following members in the MIB_IPNET_ROW2 structure pointed to the Row parameter
must be initialized to the interface: the InterfaceLuid or InterfaceIndex.

The fields are used in the order listed above. So if the InterfaceLuid is specified,
then this member is used to determine the interface on which to add the unicast IP address.
If no value was set for the InterfaceLuid member (the values of this member was set to zero),
then the InterfaceIndex member is next used to determine the interface.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-resolveipnetentry2
*/
{
    MIB_IPNET_ROW2 Row{};
    IN6_ADDR sin6_addr{};
    IN_ADDR sin_addr{};

    if (InetPtonA(AF_INET6, ip, &sin6_addr)) {
        Row.Address.si_family = AF_INET6;
        RtlCopyMemory(&Row.Address.Ipv6.sin6_addr, &sin6_addr, sizeof(IN6_ADDR));
    }

    if (InetPtonA(AF_INET, ip, &sin_addr)) {
        Row.Address.si_family = AF_INET;
        Row.Address.Ipv4.sin_addr.S_un.S_addr = sin_addr.S_un.S_addr;
    }

    NTSTATUS status = ResolveIpNetEntry2(&Row, nullptr);
    if (NO_ERROR != status) {
        printf("%ld\n", status);
        return;
    }

    for (ULONG j = 0; j < Row.PhysicalAddressLength; j++) {
        if (j == (Row.PhysicalAddressLength - 1))
            printf("%.2X\n", (int)Row.PhysicalAddress[j]);
        else
            printf("%.2X-", (int)Row.PhysicalAddress[j]);
    }
}


EXTERN_C
DLLEXPORT
BOOL WINAPI GetMacByGatewayIPv6(const char * ipv6, PBYTE mac)
/*
功能：获取一个(本地的)IPv6地址(默认网关)的MAC地址。

参数：
1.ipv6是一个网关的（IPv6）地址，兼容%符号。
2.mac确保容纳6字节。

思路：一个网关的MAC总是在IPv6的邻居表（neighbor table）中的。

注意：这个功能不能叫 获取一个IPv6地址的默认网关的MAC地址，因为不是所有的IPv6都有MAC在邻居表中。
*/
{
    bool ret = false;

    PMIB_IPNET_TABLE2 pipTable = nullptr;
    unsigned long status = GetIpNetTable2(AF_INET6, &pipTable);
    if (status != NO_ERROR) {
        return ret;
    }

    IN6_ADDR sin6_addr{};
    InetPtonA(AF_INET6, ipv6, &sin6_addr);

    for (int i = 0; (unsigned)i < pipTable->NumEntries; i++) {
        if (IN6_ADDR_EQUAL(&sin6_addr, &pipTable->Table[i].Address.Ipv6.sin6_addr)) {
            if (pipTable->Table[i].IsRouter) { // && (pipTable->Table[i].State == NlnsStale)
                if (6 == pipTable->Table[i].PhysicalAddressLength) {
                    RtlCopyMemory(
                        mac, pipTable->Table[i].PhysicalAddress, pipTable->Table[i].PhysicalAddressLength);
                    ret = true;
                    break;
                }
            }
        }
    }

    FreeMibTable(pipTable);
    return ret;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIpForwardTable()
/*
IPv4 routing table

The GetIpForwardTable function retrieves the IPv4 routing table.

The following example retrieves the IP routing table then prints some fields for each route in the table.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipforwardtable

程序打印如下，类似route PRINT的输出，但是没有IPv6的。
*/
{
    // Declare and initialize variables.

    /* variables used for GetIfForwardTable */
    PMIB_IPFORWARDTABLE pIpForwardTable{};
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    char szDestIp[128]{};
    char szMaskIp[128]{};
    char szGatewayIp[128]{};
    in_addr IpAddr{};
    int i{};

    pIpForwardTable = (MIB_IPFORWARDTABLE *)MALLOC(sizeof(MIB_IPFORWARDTABLE));
    if (pIpForwardTable == nullptr) {
        printf("Error allocating memory\n");
        return 1;
    }

    if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pIpForwardTable);
        pIpForwardTable = (MIB_IPFORWARDTABLE *)MALLOC(dwSize);
        if (pIpForwardTable == nullptr) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    /*
    Note that the IPv4 addresses returned in GetIpForwardTable entries are in network byte order
     */
    if ((dwRetVal = GetIpForwardTable(pIpForwardTable, &dwSize, 0)) == NO_ERROR) {
        printf("\tNumber of entries: %d\n", (int)pIpForwardTable->dwNumEntries);
        for (i = 0; i < (int)pIpForwardTable->dwNumEntries; i++) {
            /* Convert IPv4 addresses to strings */
            IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardDest;
            strcpy_s(szDestIp, sizeof(szDestIp), inet_ntoa(IpAddr));
            IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardMask;
            strcpy_s(szMaskIp, sizeof(szMaskIp), inet_ntoa(IpAddr));
            IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardNextHop;
            strcpy_s(szGatewayIp, sizeof(szGatewayIp), inet_ntoa(IpAddr));

            printf("\n\tRoute[%d] Dest IP: %s\n", i, szDestIp);
            printf("\tRoute[%d] Subnet Mask: %s\n", i, szMaskIp);
            printf("\tRoute[%d] Next Hop: %s\n", i, szGatewayIp);
            printf("\tRoute[%d] If Index: %lu\n", i, pIpForwardTable->table[i].dwForwardIfIndex);
            printf("\tRoute[%d] Type: %lu - ", i, pIpForwardTable->table[i].dwForwardType);
            PrintRouteType(pIpForwardTable->table[i].dwForwardType);

            printf("\tRoute[%d] Proto: %lu - ", i, pIpForwardTable->table[i].dwForwardProto);
            PrintRoutingProtocols(pIpForwardTable->table[i].dwForwardProto);

            printf("\tRoute[%d] Age: %lu\n", i, pIpForwardTable->table[i].dwForwardAge);
            printf("\tRoute[%d] Metric1: %lu\n", i, pIpForwardTable->table[i].dwForwardMetric1);
        }

        FREE(pIpForwardTable);
        return 0;
    } else {
        printf("\tGetIpForwardTable failed.\n");
        FREE(pIpForwardTable);
        return 1;
    }
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIpForwardTable2()
/*
The GetIpForwardTable2 function retrieves the IP route entries on the local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getipforwardtable2
https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipforwardtable

微软没有例子，这个是自己写的。
*/
{
    PMIB_IPFORWARD_TABLE2 pIpForwardTable;
    if (GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable) != NO_ERROR) {
        printf("\tGetIpForwardTable failed.\n");
        return 1;
    }

    printf("Number of entries: %d\n", (int)pIpForwardTable->NumEntries);

    for (ULONG i = 0; i < (int)pIpForwardTable->NumEntries; i++) {
    }

    FreeMibTable(pIpForwardTable);

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIfTable()
/*
The GetIfTable function retrieves the MIB-II interface table.

The following example retrieves the interface table and prints the number of entries in the table and
some data on each entry.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getiftable
*/
{
    // Declare and initialize variables.
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    unsigned int i{}, j{};

    /* variables used for GetIfTable and GetIfEntry */
    MIB_IFTABLE * pIfTable{};
    MIB_IFROW * pIfRow{};

    // Allocate memory for our pointers.
    pIfTable = (MIB_IFTABLE *)MALLOC(sizeof(MIB_IFTABLE));
    if (pIfTable == nullptr) {
        printf("Error allocating memory needed to call GetIfTable\n");
        return 1;
    }

    // Make an initial call to GetIfTable to get the necessary size into dwSize
    dwSize = sizeof(MIB_IFTABLE);
    if (GetIfTable(pIfTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pIfTable);
        pIfTable = (MIB_IFTABLE *)MALLOC(dwSize);
        if (pIfTable == nullptr) {
            printf("Error allocating memory needed to call GetIfTable\n");
            return 1;
        }
    }

    // Make a second call to GetIfTable to get the actual data we want.
    if ((dwRetVal = GetIfTable(pIfTable, &dwSize, FALSE)) == NO_ERROR) {
        printf("\tNum Entries: %lu\n\n", pIfTable->dwNumEntries);
        for (i = 0; i < pIfTable->dwNumEntries; i++) {
            pIfRow = (MIB_IFROW *)&pIfTable->table[i];
            printf("\tIndex[%u]:\t %lu\n", i, pIfRow->dwIndex);
            printf("\tInterfaceName[%u]:\t %ws", i, pIfRow->wszName);
            printf("\n");
            printf("\tDescription[%u]:\t ", i);

            for (j = 0; j < pIfRow->dwDescrLen; j++)
                printf("%c", pIfRow->bDescr[j]);

            printf("\n");
            printf("\tType[%u]:\t ", i);
            PrintInterfaceType((WORD)pIfRow->dwType);

            printf("\tMtu[%u]:\t\t %lu\n", i, pIfRow->dwMtu);
            printf("\tSpeed[%u]:\t %lu\n", i, pIfRow->dwSpeed);
            printf("\tPhysical Addr:\t ");

            if (pIfRow->dwPhysAddrLen == 0)
                printf("\n");

            for (j = 0; j < pIfRow->dwPhysAddrLen; j++) {
                if (j == (pIfRow->dwPhysAddrLen - 1))
                    printf("%.2X\n", (int)pIfRow->bPhysAddr[j]);
                else
                    printf("%.2X-", (int)pIfRow->bPhysAddr[j]);
            }

            printf("\tAdmin Status[%u]:\t %lu\n", i, pIfRow->dwAdminStatus);
            printf("\tOper Status[%u]:\t ", i);

            PrintInterfaceOperationalStatus(pIfRow->dwOperStatus);

            printf("\n");
        }
    } else {
        printf("GetIfTable failed with error:%lu \n", dwRetVal); // MSDN少写%了。
        if (pIfTable != nullptr) {
            FREE(pIfTable);
            pIfTable = nullptr;
        }

        return 1;
        // Here you can use FormatMessage to find out why it failed.
    }

    if (pIfTable != nullptr) {
        FREE(pIfTable);
        pIfTable = nullptr;
    }
    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIfTable2()
/*
微软没有例子，这个是自己写的。
*/
{
    PMIB_IF_TABLE2 table;
    if (GetIfTable2(&table) != ERROR_SUCCESS) {

        return GetLastError();
    }

    printf("Number of entries: %lu\n", table->NumEntries);

    printf("\n");

    for (ULONG i = 0; i < table->NumEntries; i++) {


        printf("Alias: %ls\n", table->Table[i].Alias);
        printf("Description: %ls\n", table->Table[i].Description);
        printf("PhysicalAddress: %s\n", table->Table[i].PhysicalAddress);
        printf("PermanentPhysicalAddress: %s\n", table->Table[i].PermanentPhysicalAddress);


        printf("\n");
    }

    FreeMibTable(table);

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIfTable2Ex()
/*
https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getiftable2ex
此文是自己编写，微软没有例子。
*/
{
    PMIB_IF_TABLE2 table;
    if (GetIfTable2Ex(MibIfTableNormal, &table) != ERROR_SUCCESS) {

        return GetLastError();
    }

    printf("Number of entries: %lu\n", table->NumEntries);

    printf("\n");

    for (ULONG i = 0; i < table->NumEntries; i++) {


        printf("Alias: %ls\n", table->Table[i].Alias);
        printf("Description: %ls\n", table->Table[i].Description);
        printf("PhysicalAddress: %s\n", table->Table[i].PhysicalAddress);
        printf("PermanentPhysicalAddress: %s\n", table->Table[i].PermanentPhysicalAddress);


        printf("\n");
    }

    FreeMibTable(table);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void usage(char * pname)
{
    printf("Usage: %s [options] ip-address\n", pname);
    printf("\t -h \t\thelp\n");
    printf("\t -l length \tMAC physical address length to set\n");
    printf("\t -s src-ip \tsource IP address\n");
    exit(1);
}


EXTERN_C
DLLEXPORT
int WINAPI GetMacByIPv4Test(int argc, char ** argv)
/*
功能：获取(局域网，不能是互联网)IPv4对应的MAC地址。

The SendARP function sends an Address Resolution Protocol (ARP) request to
obtain the physical address that corresponds to the specified destination IPv4 address.

The following code demonstrates how to obtain the hardware or media access control (MAC) address associated with a
specified IPv4 address.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-sendarp
https://msdn.microsoft.com/en-us/library/aa366358(VS.85).aspx
*/
{
    DWORD dwRetVal{};
    IPAddr DestIp = 0;
    IPAddr SrcIp = 0;      /* default for src ip */
    ULONG MacAddr[2]{};    /* for 6-byte hardware addresses */
    ULONG PhysAddrLen = 6; /* default to length of six bytes */

    char * DestIpString = nullptr;
    char * SrcIpString = nullptr;

    BYTE * bPhysAddr{};
    unsigned int i{};

    if (argc > 1) {
        for (i = 1; i < (unsigned int)argc; i++) {
            if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
                switch (tolower(argv[i][1])) {
                case 'l':
                    PhysAddrLen = (ULONG)atol(argv[++i]);
                    break;
                case 's':
                    SrcIpString = argv[++i];
                    SrcIp = inet_addr(SrcIpString);
                    break;
                case 'h':
                default:
                    usage(argv[0]);
                    break;
                } /* end switch */
            } else {
                DestIpString = argv[i];
            }
        } /* end for */
    } else {
        usage(argv[0]);
    }

    if (DestIpString == nullptr || DestIpString[0] == '\0')
        usage(argv[0]);

    DestIp = inet_addr(DestIpString);

    memset(&MacAddr, 0xff, sizeof(MacAddr));

    printf("Sending ARP request for IP address: %s\n", DestIpString);

    dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen);
    if (dwRetVal == NO_ERROR) {
        bPhysAddr = (BYTE *)&MacAddr;
        if (PhysAddrLen) {
            for (i = 0; i < PhysAddrLen; i++) {
                if (i == (PhysAddrLen - 1))
                    printf("%.2X\n", (int)bPhysAddr[i]);
                else
                    printf("%.2X-", (int)bPhysAddr[i]);
            }
        } else {
            printf("Warning: SendArp completed successfully, but returned length=0\n");
        }
    } else {
        printf("Error: SendArp failed with error: %lu", dwRetVal);
        switch (dwRetVal) {
        case ERROR_GEN_FAILURE:
            printf(" (ERROR_GEN_FAILURE)\n");
            break;
        case ERROR_INVALID_PARAMETER:
            printf(" (ERROR_INVALID_PARAMETER)\n");
            break;
        case ERROR_INVALID_USER_BUFFER:
            printf(" (ERROR_INVALID_USER_BUFFER)\n");
            break;
        case ERROR_BAD_NET_NAME:
            printf(" (ERROR_GEN_FAILURE)\n");
            break;
        case ERROR_BUFFER_OVERFLOW:
            printf(" (ERROR_BUFFER_OVERFLOW)\n");
            break;
        case ERROR_NOT_FOUND:
            printf(" (ERROR_NOT_FOUND)\n");
            break;
        default:
            printf("\n");
            break;
        }
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI GetMacByIPv4(IPAddr DestIp, PBYTE MacAddr)
/*
功能：获取(局域网，不能是互联网)IPv4对应的MAC地址。

用法实例：
BYTE MacAddr[6] = {0};
GetMacByIPv4(inet_addr("192.168.5.1"), MacAddr);
*/
{
    // if (DestIpString == nullptr || DestIpString[0] == '\0') {
    //    return ERROR_INVALID_PARAMETER;
    //}

    ULONG PhysAddrLen = 6;
    DWORD dwRetVal = SendARP(DestIp, 0, MacAddr, &PhysAddrLen);
    if (dwRetVal == NO_ERROR) {
        if (PhysAddrLen) {

        } else {
            printf("Warning: SendArp completed successfully, but returned length=0\n");
        }
    } else {
        printf("Error: SendArp failed with error: %lu", dwRetVal);
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI GetGatewayMacByIPv4(const char * IPv4, PBYTE GatewayMac)
{
    char Gateway[4 * 4];
    GetGatewayByIPv4(IPv4, Gateway);

    GetMacByIPv4(inet_addr(Gateway), GatewayMac);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI EnumUnicastIpAddressTable()
/*
The following example retrieves a unicast IP address table and
prints some values from each of the retrieved MIB_UNICASTIPADDRESS_ROW structures.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getunicastipaddresstable
*/
{
    // Declare and initialize variables
    unsigned int i{};
    DWORD Result = 0;
    WCHAR Ipv4String[16] = {0};
    WCHAR Ipv6String[46] = {0};
    PMIB_UNICASTIPADDRESS_TABLE pipTable = nullptr;

    Result = GetUnicastIpAddressTable(AF_UNSPEC, &pipTable);
    if (Result != NO_ERROR) {
        wprintf(L"GetUnicastIpAddressTable returned error: %lu\n", Result);
        return (1);
    }
    // Print some variables from the rows in the table
    wprintf(L"Number of table entries: %u\n\n", pipTable->NumEntries);

    for (i = 0; i < pipTable->NumEntries; i++) {
        wprintf(L"AddressFamily[%u]:\t\t ", i);

        switch (pipTable->Table[i].Address.si_family) {
        case AF_INET:
            wprintf(L"IPv4\n");
            if (InetNtopW(AF_INET, &pipTable->Table[i].Address.Ipv4.sin_addr, Ipv4String, 16) != nullptr)
                wprintf(L"IPv4 Address:\t\t\t %ws\n", Ipv4String);
            break;
        case AF_INET6:
            wprintf(L"IPv6\n");
            if (InetNtopW(AF_INET6, &pipTable->Table[i].Address.Ipv6.sin6_addr, Ipv6String, 46) != nullptr)
                wprintf(L"IPv6 Address:\t\t\t %ws\n", Ipv6String);
            break;
        default:
            wprintf(L"Other: %u\n", pipTable->Table[i].Address.si_family);
            break;
        }

        wprintf(L"Interface LUID NetLuidIndex[%u]:  %I64u\n", i, pipTable->Table[i].InterfaceLuid.Info.NetLuidIndex);
        wprintf(L"Interface LUID IfType[%u]:\t ", i);
        PrintInterfaceType(pipTable->Table[i].InterfaceLuid.Info.IfType);

        wprintf(L"Interface Index[%u]:\t\t %lu\n", i, pipTable->Table[i].InterfaceIndex);

        wprintf(L"Prefix Origin[%u]:\t\t ", i);
        PrintPrefixOrigin(pipTable->Table[i].PrefixOrigin);

        wprintf(L"Suffix Origin[%u]:\t\t ", i);
        PrintSuffixOrigin(pipTable->Table[i].SuffixOrigin);

        wprintf(L"Valid Lifetime[%u]:\t\t 0x%x (%u)\n", i, pipTable->Table[i].ValidLifetime, pipTable->Table[i].ValidLifetime);
        wprintf(L"Preferred Lifetime[%u]:\t\t 0x%x (%u)\n", i, pipTable->Table[i].PreferredLifetime, pipTable->Table[i].PreferredLifetime);
        wprintf(L"OnLink PrefixLength[%u]:\t\t %lu\n", i, pipTable->Table[i].OnLinkPrefixLength);

        wprintf(L"Skip As Source[%u]:\t\t ", i);
        if (pipTable->Table[i].SkipAsSource)
            wprintf(L"Yes\n");
        else
            wprintf(L"No\n");

        wprintf(L"Dad State[%u]:\t\t\t ", i);
        PrintDadState(pipTable->Table[i].DadState);

        wprintf(L"\n");
    }

    if (pipTable != nullptr) {
        FreeMibTable(pipTable);
        pipTable = nullptr;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI Icmp6Test()
/*
功能：演示Icmp6CreateFile+Icmp6SendEcho2+Icmp6ParseReplies的用法。

参考：
https://docs.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmp6createfile
https://docs.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmp6sendecho2
https://docs.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmp6parsereplies

更多参考：
\Win2K3\NT\net\tcpip\commands\pathping\pathping.c
\Win2K3\NT\net\tcpip\commands\ping\ping.c
\Win2K3\NT\net\tcpip\commands\tracert\tracert.c

注意：要测试互联网，不要你所在的网络支持IPv6.

扩展功能：根据IPv6获取MAC，实现类似SendArp的类似功能。
思路：组装发送ND_NEIGHBOR_SOLICIT类型的nd_neighbor_solicit结构，
      解析返回的ND_NEIGHBOR_ADVERT类型的nd_neighbor_advert结构。
*/
{
    HANDLE hIcmpFile = Icmp6CreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("\tUnable to open handle.\n");
        printf("Icmp6Createfile returned error: %lu\n", GetLastError());
        return;
    }

    sockaddr_in6 SourceAddress{};
    InetPtonA(AF_INET6, "fe80::10bb:f0a9:744b:aac2", &SourceAddress.sin6_addr);

    // SourceAddress.sin6_addr = in6addr_any;//这个更通用。
    SourceAddress.sin6_family = AF_INET6;

    sockaddr_in6 DestinationAddress{};
    InetPtonA(AF_INET6, "fe80::ec0f:f6:4a6d:89d2", &DestinationAddress.sin6_addr);

    DestinationAddress.sin6_family = AF_INET6;

    WORD RequestSize = 0;

    /*
    A pointer to the IPv6 header options for the request,
    in the form of an IP_OPTION_INFORMATION structure.
    On a 64-bit platform, this parameter is in the form for an IP_OPTION_INFORMATION32 structure.

    This parameter may be NULL if no IP header options need to be specified.

    Note  On Windows Server 2003 and Windows XP,
    the RequestOptions parameter is not optional and must not be NULL and
    only the Ttl and Flags members are used.
    */
    IP_OPTION_INFORMATION RequestOptions = {30, 0, 0, 0, nullptr};

    PVOID ReplyBuffer = nullptr;

    /*
    The size, in bytes, of the reply buffer pointed to by the ReplyBuffer parameter.
    This buffer should be large enough to hold at least one ICMPV6_ECHO_REPLY structure plus RequestSize bytes of
    data. This buffer should also be large enough to also hold 8 more bytes of data (the size of an ICMP error
    message) plus space for an IO_STATUS_BLOCK structure.
    */
    DWORD ReplySize = sizeof(ICMPV6_ECHO_REPLY) + RequestSize + 8; // + sizeof(IO_STATUS_BLOCK)

    /*
    The time, in milliseconds, to wait for replies.
    This parameter is only used if the Icmp6SendEcho2 function is called synchronously.
    So this parameter is not used if either the ApcRoutine or Event parameter are not NULL.
    （谷歌翻译：因此，如果 ApcRoutine 或 Event 参数不为 NULL，则不使用此参数。）
    反过来，ApcRoutine 和 Event 参数为 NULL，则使用此参数。

    这个必须设置，不是可选的，且不能为0，你看是_In_，否则，返回ERROR_INVALID_PARAMETER。
    */
    DWORD Timeout = 1000;

    ReplyBuffer = (PVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ReplySize);
    ;
    _ASSERTE(ReplyBuffer);

    DWORD ret = Icmp6SendEcho2(hIcmpFile,
                               nullptr,
                               nullptr,
                               nullptr,
                               &SourceAddress,
                               &DestinationAddress,
                               nullptr, // RequestData,
                               0,       // RequestSize,
                               &RequestOptions,
                               ReplyBuffer,
                               ReplySize,
                               Timeout);
    if (0 == ret) {
        printf("LastError:%lu\n", GetLastError()); // ERROR_INVALID_PARAMETER
        IcmpCloseHandle(hIcmpFile);
        return;
    }

#pragma prefast(push)
#pragma prefast(disable : 28020, "XXXXX")
    ret = Icmp6ParseReplies(ReplyBuffer, ReplySize);
#pragma prefast(pop)
    if (0 == ret) {
        printf("LastError:%lu\n", GetLastError());
        IcmpCloseHandle(hIcmpFile);
        return;
    }

    PICMPV6_ECHO_REPLY Reply = static_cast<PICMPV6_ECHO_REPLY>(ReplyBuffer);
    printf("Status:%lu\n", Reply->Status);

    HeapFree(GetProcessHeap(), 0, ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI GetNetworkParamsTest()
/*

https://learn.microsoft.com/zh-CN/windows/win32/api/iphlpapi/nf-iphlpapi-getnetworkparams
*/
{
    FIXED_INFO * pFixedInfo{};
    ULONG ulOutBufLen{};
    DWORD dwRetVal{};
    IP_ADDR_STRING * pIPAddr{};

    pFixedInfo = (FIXED_INFO *)MALLOC(sizeof(FIXED_INFO));
    if (pFixedInfo == nullptr) {
        printf("Error allocating memory needed to call GetNetworkParams\n");
        return 1;
    }
    ulOutBufLen = sizeof(FIXED_INFO);

    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pFixedInfo);
        pFixedInfo = (FIXED_INFO *)MALLOC(ulOutBufLen);
        if (pFixedInfo == nullptr) {
            printf("Error allocating memory needed to call GetNetworkParams\n");
            return 1;
        }
    }

    dwRetVal = GetNetworkParams(pFixedInfo, &ulOutBufLen);
    if (dwRetVal == NO_ERROR) {
        printf("Host Name: %s\n", pFixedInfo->HostName);
        printf("Domain Name: %s\n", pFixedInfo->DomainName);

        printf("DNS Servers:\n");
        printf("\t%s\n", pFixedInfo->DnsServerList.IpAddress.String);

        pIPAddr = pFixedInfo->DnsServerList.Next;
        while (pIPAddr) {
            printf("\t%s\n", pIPAddr->IpAddress.String);
            pIPAddr = pIPAddr->Next;
        }

        printf("Node Type: ");
        PrintNodeType(pFixedInfo->NodeType);

        printf("DHCP scope name: %s\n", pFixedInfo->ScopeId);

        if (pFixedInfo->EnableRouting)
            printf("Routing: enabled\n");
        else
            printf("Routing: disabled\n");

        if (pFixedInfo->EnableProxy)
            printf("ARP proxy: enabled\n");
        else
            printf("ARP Proxy: disabled\n");

        if (pFixedInfo->EnableDns)
            printf("DNS: enabled\n");
        else
            printf("DNS: disabled\n");
    } else {
        printf("GetNetworkParams failed with error: %lu\n", dwRetVal);
        return 1;
    }

    if (pFixedInfo)
        FREE(pFixedInfo);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI EnumAnycastIpAddress()
/*
GetAnycastIpAddressTable 函数检索本地计算机上的 anycast IP 地址表。

https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/nf-netioapi-getanycastipaddresstable
https://learn.microsoft.com/zh-cn/windows-hardware/drivers/network/getunicastipaddresstable
*/
{
    PMIB_ANYCASTIPADDRESS_TABLE Table{};
    NTSTATUS Status = GetAnycastIpAddressTable(AF_UNSPEC, &Table);
    if (NO_ERROR != Status) {

        return Status;
    }

    for (ULONG i = 0; i < Table->NumEntries; i++) {

        printf("Zone:%d, Level:%d\r\n", Table->Table[i].ScopeId.Zone, Table->Table[i].ScopeId.Level);

        printf("\r\n");
    }

    FreeMibTable(Table);

    return Status;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumMulticastIpAddress()
/*
GetMulticastIpAddressTable 函数检索本地计算机上的多播 IP 地址表。

https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/nf-netioapi-getmulticastipaddresstable
https://learn.microsoft.com/zh-cn/windows-hardware/drivers/network/getmulticastipaddresstable
*/
{
    PMIB_MULTICASTIPADDRESS_TABLE Table{};
    NTSTATUS Status = GetMulticastIpAddressTable(AF_UNSPEC, &Table);
    if (NO_ERROR != Status) {

        return Status;
    }

    for (ULONG i = 0; i < Table->NumEntries; i++) {

        printf("Zone:%d, Level:%d\r\n", Table->Table[i].ScopeId.Zone, Table->Table[i].ScopeId.Level);

        printf("\r\n");
    }

    FreeMibTable(Table);

    return Status;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumStackInterface()
/*
GetIfStackTable 函数检索网络接口堆栈行条目表，这些条目指定接口堆栈上网络接口的关系。

GetIfStackTable 函数在 Windows Vista 及更高版本上定义。

The
GetIfStackTable 函数枚举本地系统上接口堆栈上的物理和逻辑网络接口，并在 MIB_IFSTACK_TABLE 结构中返回此信息。

接口堆栈条目在 Table 参数指向的缓冲区中的MIB_IFSTACK_TABLE结构中返回。
MIB_IFSTACK_TABLE 结构包含接口堆栈条目计数和每个接口堆栈条目的MIB_IFSTACK_ROW结构数组。

接口堆栈中的接口之间的关系是，在 MIB_IFSTACK_ROW 结构的 HigherLayerInterfaceIndex 成员中具有索引的接口正上方，
其索引位于 MIB_IFSTACK_ROW 结构的 LowerLayerInterfaceIndex 成员中。

内存由 GetIfStackTable 函数为此结构中的 MIB_IFSTACK_TABLE 和 MIB_IFSTACK_ROW 项分配。
当不再需要这些返回的结构时，通过调用 FreeMibTable 释放内存。

请注意，Table 参数指向的返回MIB_IFSTACK_TABLE结构可能包含 NumEntries 成员与
MIB_IFSTACK_TABLE 结构的Table 成员中的第一个MIB_IFSTACK_ROW数组项之间的对齐填充。
MIB_IFSTACK_ROW 数组条目 之间还可能存在对齐的填充。
对 MIB_IFSTACK_ROW 数组条目的任何访问都应假定可能存在填充。

https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/nf-netioapi-getifstacktable
https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/ns-netioapi-mib_ifstack_table
https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/ns-netioapi-mib_ifstack_row
*/
{
    PMIB_IFSTACK_TABLE Table{};
    NTSTATUS Status = GetIfStackTable(&Table);
    if (NO_ERROR != Status) {

        return Status;
    }

    for (ULONG i = 0; i < Table->NumEntries; i++) {
        printf("HigherLayerInterfaceIndex:%u, LowerLayerInterfaceIndex:%u\r\n", Table->Table[i].HigherLayerInterfaceIndex, Table->Table[i].LowerLayerInterfaceIndex);
    }

    FreeMibTable(Table);

    return Status;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumInvertedStackInterface()
/*
GetInvertedIfStackTable 函数检索反转网络接口堆栈行条目的表，这些条目指定接口堆栈上网络接口的关系。

https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/nf-netioapi-getinvertedifstacktable
*/
{
    PMIB_INVERTEDIFSTACK_TABLE Table{};
    NTSTATUS Status = GetInvertedIfStackTable(&Table);
    if (NO_ERROR != Status) {

        return Status;
    }

    for (ULONG i = 0; i < Table->NumEntries; i++) {
        printf("HigherLayerInterfaceIndex:%u, LowerLayerInterfaceIndex:%u\r\n", Table->Table[i].HigherLayerInterfaceIndex, Table->Table[i].LowerLayerInterfaceIndex);
    }

    FreeMibTable(Table);

    return Status;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIpInterface()
/*
GetIpInterfaceTable 函数检索本地计算机上的 IP 接口条目。

以下示例检索 IP 接口表，然后打印表中 IP 接口条目的几个成员的值。

https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/nf-netioapi-getipinterfacetable
*/
{
    int i;
    DWORD dwRetVal = 0;
    PMIB_IPINTERFACE_TABLE pipTable = NULL;

    dwRetVal = GetIpInterfaceTable(AF_UNSPEC, &pipTable);
    if (dwRetVal != NO_ERROR) {
        printf("GetIpInterfaceTable returned error: %ld\n", dwRetVal);
        exit(1);
    }
    // Print some variables from the rows in the table
    printf("Number of table entries: %lu\n\n", pipTable->NumEntries);

    for (i = 0; i < (int)pipTable->NumEntries; i++) {
        printf("Address Family[%d]:\t\t", i);
        switch (pipTable->Table[i].Family) {
        case AF_INET:
            printf("IPv4\n");
            break;
        case AF_INET6:
            printf("IPv6\n");
            break;
        default:
            printf("Other: %d\n", pipTable->Table[i].Family);
            break;
        }

        printf("Interface LUID NetLuidIndex[%d]:\t %llu\n", i, pipTable->Table[i].InterfaceLuid.Info.NetLuidIndex);

        printf("Interface LUID IfType[%d]:\t ", i);
        switch (pipTable->Table[i].InterfaceLuid.Info.IfType) {
        case IF_TYPE_OTHER:
            printf("Other\n");
            break;
        case IF_TYPE_ETHERNET_CSMACD:
            printf("Ethernet\n");
            break;
        case IF_TYPE_ISO88025_TOKENRING:
            printf("Token ring\n");
            break;
        case IF_TYPE_PPP:
            printf("PPP\n");
            break;
        case IF_TYPE_SOFTWARE_LOOPBACK:
            printf("Software loopback\n");
            break;
        case IF_TYPE_ATM:
            printf("ATM\n");
            break;
        case IF_TYPE_IEEE80211:
            printf("802.11 wireless\n");
            break;
        case IF_TYPE_TUNNEL:
            printf("Tunnel encapsulation\n");
            break;
        case IF_TYPE_IEEE1394:
            printf("IEEE 1394 (Firewire)\n");
            break;
        default:
            printf("Unknown: %Id\n", pipTable->Table[i].InterfaceLuid.Info.IfType);
            break;
        }
        printf("Interface Index[%d]:\t\t %lu\n", i, pipTable->Table[i].InterfaceIndex);
        printf("Maximum reassembly size[%d]:\t %lu\n", i, pipTable->Table[i].MaxReassemblySize);

        printf("Advertising enabled[%d]:\t\t ", i);
        if (pipTable->Table[i].AdvertisingEnabled)
            printf("Yes\n");
        else
            printf("No\n");

        printf("Forwarding enabled[%d]:\t\t ", i);
        if (pipTable->Table[i].ForwardingEnabled)
            printf("Yes\n");
        else
            printf("No\n");

        printf("Network layer MTU[%d]:\t\t %lu\n", i, pipTable->Table[i].NlMtu);

        printf("Connected[%d]:\t\t\t ", i);
        if (pipTable->Table[i].Connected)
            printf("Yes\n");
        else
            printf("No\n");

        printf("Supports wakeup patterns[%d]:\t ", i);
        if (pipTable->Table[i].SupportsWakeUpPatterns)
            printf("Yes\n");
        else
            printf("No\n");

        printf("Supports neighbor discovery[%d]:\t ", i);
        if (pipTable->Table[i].SupportsNeighborDiscovery)
            printf("Yes\n");
        else
            printf("No\n");

        printf("Supports router discovery[%d]:\t ", i);
        if (pipTable->Table[i].SupportsRouterDiscovery)
            printf("Yes\n");
        else
            printf("No\n");

        printf("\n");
    }

    FreeMibTable(pipTable);
    pipTable = NULL;
    return dwRetVal;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumIpPath()
/*
GetIpPathTable 函数检索本地计算机上的 IP 路径表。

https://learn.microsoft.com/zh-cn/windows/win32/api/netioapi/nf-netioapi-getippathtable
*/
{
    PMIB_IPPATH_TABLE Table{};
    NTSTATUS Status = GetIpPathTable(AF_UNSPEC, &Table);
    if (NO_ERROR != Status) {

        return Status;
    }

    for (ULONG i = 0; i < Table->NumEntries; i++) {

        printf("PathMtu:%lu\r\n", Table->Table[i].PathMtu);

        printf("\r\n");
    }

    FreeMibTable(Table);

    return Status;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI SendIcmpEcho(int argc, char ** argv)
/*
以下示例将 ICMP 回显请求发送到命令行中指定的 IP 地址，并打印从第一个响应接收的信息。

https://learn.microsoft.com/zh-cn/windows/win32/api/icmpapi/nf-icmpapi-icmpsendecho
*/
{
    // Declare and initialize variables

    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s IP address\n", argv[0]);
        return 1;
    }

    ipaddr = inet_addr(argv[1]);
    if (ipaddr == INADDR_NONE) {
        printf("usage: %s IP address\n", argv[0]);
        return 1;
    }

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("\tUnable to open handle.\n");
        printf("IcmpCreatefile returned error: %ld\n", GetLastError());
        return 1;
    }

    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID *)malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        printf("\tUnable to allocate memory\n");
        return 1;
    }

#pragma warning(push)
#pragma warning(disable : 28020) //表达式“_Param_(7)>=sizeof(struct icmp_echo_reply
                                 //ICMP_ECHO_REPLY)+_Param_(4)+8”对此调用无效。
    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
#pragma warning(pop)
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        printf("\tSent icmp message to %s\n", argv[1]);
        if (dwRetVal > 1) {
            printf("\tReceived %ld icmp message responses\n", dwRetVal);
            printf("\tInformation from the first response:\n");
        } else {
            printf("\tReceived %ld icmp message response\n", dwRetVal);
            printf("\tInformation from this response:\n");
        }
        printf("\t  Received from %s\n", inet_ntoa(ReplyAddr));
        printf("\t  Status = %ld\n", pEchoReply->Status);
        printf("\t  Roundtrip time = %ld milliseconds\n", pEchoReply->RoundTripTime);
    } else {
        printf("\tCall to IcmpSendEcho failed.\n");
        printf("\tIcmpSendEcho returned error: %ld\n", GetLastError());
        return 1;
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI SendIcmpEcho2(int argc, char ** argv)
/*
以下示例同步调用 IcmpSendEcho2 函数。
该示例将 ICMP 回显请求发送到命令行中指定的 IP 地址，并输出从第一个响应接收的信息。

https://learn.microsoft.com/zh-cn/windows/win32/api/icmpapi/nf-icmpapi-icmpsendecho2
*/
{
    // Declare and initialize variables.
    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    DWORD dwError = 0;
    char SendData[] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;

    // Validate the parameters.
    if (argc != 2) {
        printf("usage: %s IP address\n", argv[0]);
        return 1;
    }

    ipaddr = inet_addr(argv[1]);
    if (ipaddr == INADDR_NONE) {
        printf("usage: %s IP address\n", argv[0]);
        return 1;
    }

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("\tUnable to open handle.\n");
        printf("IcmpCreatefile returned error: %ld\n", GetLastError());
        return 1;
    }

    // Allocate space for a single reply.
    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData) + 8;
    ReplyBuffer = (VOID *)malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        printf("\tUnable to allocate memory for reply buffer\n");
        return 1;
    }

    dwRetVal = IcmpSendEcho2(
        hIcmpFile, NULL, NULL, NULL, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        printf("\tSent icmp message to %s\n", argv[1]);
        if (dwRetVal > 1) {
            printf("\tReceived %ld icmp message responses\n", dwRetVal);
            printf("\tInformation from the first response:\n");
        } else {
            printf("\tReceived %ld icmp message response\n", dwRetVal);
            printf("\tInformation from this response:\n");
        }
        printf("\t  Received from %s\n", inet_ntoa(ReplyAddr));
        printf("\t  Status = %ld  ", pEchoReply->Status);
        switch (pEchoReply->Status) {
        case IP_DEST_HOST_UNREACHABLE:
            printf("(Destination host was unreachable)\n");
            break;
        case IP_DEST_NET_UNREACHABLE:
            printf("(Destination Network was unreachable)\n");
            break;
        case IP_REQ_TIMED_OUT:
            printf("(Request timed out)\n");
            break;
        default:
            printf("\n");
            break;
        }

        printf("\t  Roundtrip time = %ld milliseconds\n", pEchoReply->RoundTripTime);
    } else {
        printf("Call to IcmpSendEcho2 failed.\n");
        dwError = GetLastError();
        switch (dwError) {
        case IP_BUF_TOO_SMALL:
            printf("\tReplyBufferSize too small\n");
            break;
        case IP_REQ_TIMED_OUT:
            printf("\tRequest timed out\n");
            break;
        default:
            printf("\tExtended error returned: %ld\n", dwError);
            break;
        }
        return 1;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
