#include "pch.h"
#include "IpHelper.h"
#include "Adapter.h"


#pragma warning(disable:6387)


/*
有待实现的函数有：
GetAnycastIpAddressTable
GetIfStackTable
GetInvertedIfStackTable
GetIpInterfaceTable
GetIpPathTable
GetMulticastIpAddressTable
...
*/


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
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
    int i;

    /* Variables used by GetIpAddrTable */
    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IN_ADDR IPAddr;
    LPVOID lpMsgBuf;/* Variables used to return error message */

    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(sizeof(MIB_IPADDRTABLE));
    if (pIPAddrTable) {
        // Make an initial call to GetIpAddrTable to get the necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(dwSize);
        }

        if (pIPAddrTable == NULL) {
            printf("Memory allocation failed for GetIpAddrTable\n");
            return(1);
        }
    }

    // Make a second call to GetIpAddrTable to get the actual data we want
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
        printf("GetIpAddrTable failed with error %d\n", dwRetVal);
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL,
                          dwRetVal,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
                          (LPTSTR)&lpMsgBuf,
                          0,
                          NULL)) {
            printf("\tError: %s", (char *)lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        return(1);
    }

    printf("\tNum Entries: %ld\n", pIPAddrTable->dwNumEntries);
    for (i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
        printf("\n\tInterface Index[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwIndex);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
        printf("\tIP Address[%d]:     \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwMask;
        printf("\tSubnet Mask[%d]:    \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwBCastAddr;

#pragma warning(push)
#pragma warning(disable:4476)//"printf": 格式说明符中的类型字段字符“)”未知
        printf("\tBroadCast[%d]:      \t%s (%ld%)\n", i, inet_ntoa(IPAddr), pIPAddrTable->table[i].dwBCastAddr);
#pragma warning(pop)

        printf("\tReassembly size[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwReasmSize);
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
        pIPAddrTable = NULL;
    }

    return(0);
}


EXTERN_C
__declspec(dllexport)
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
    ULONG ret = GetIpNetTable(NULL, &SizePointer, TRUE);
    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PMIB_IPNETTABLE IpNetTable = (PMIB_IPNETTABLE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, SizePointer);
    _ASSERTE(IpNetTable);

    ret = GetIpNetTable(IpNetTable, &SizePointer, TRUE);
    _ASSERTE(NO_ERROR == ret);//ERROR_NO_DATA

    printf("Number of IPv4 table entries: %d\n\n", IpNetTable->dwNumEntries);

    for (DWORD i = 0; i < IpNetTable->dwNumEntries; i++) {
        printf("Index: %02d\t", IpNetTable->table[i].dwIndex);//类似于arp -a的接口。

        in_addr in;
        in.S_un.S_addr = IpNetTable->table[i].dwAddr;
        printf("IPv4:%-16s\t", inet_ntoa(in));

        printf("MAC:");//Physical Address
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
__declspec(dllexport)
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
    PMIB_IPNET_TABLE2 pipTable = NULL;
    unsigned long status = GetIpNetTable2(Family, &pipTable);
    if (status != NO_ERROR) {
        printf("GetIpNetTable for IP table returned error: %ld\n", status);
        return(1);
    }

    // Print some variables from the table
    printf("Number of IP table entries: %d\n\n", pipTable->NumEntries);

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

        printf("ReachabilityTime[%d]:\t %lu, %lu\n\n", (int)i,
               pipTable->Table[i].ReachabilityTime.LastReachable,
               pipTable->Table[i].ReachabilityTime.LastUnreachable);

        status = ResolveIpNetEntry2(&pipTable->Table[i], NULL);//此函数会改变LastUnreachable的值。
    }

    FreeMibTable(pipTable);
    pipTable = NULL;
    return 0;
}


EXTERN_C
__declspec(dllexport)
void WINAPI GetMacByIPv6(const char * IPv6, PBYTE Mac)
{
    MIB_IPNET_ROW2 Row = {0};

    Row.Address.si_family = AF_INET6;
    InetPtonA(AF_INET6, IPv6, &Row.Address.Ipv6.sin6_addr);

    NTSTATUS status = ResolveIpNetEntry2(&Row, NULL);
    if (NO_ERROR != status) {
        printf("%d\n", status);
        return;
    }

    RtlCopyMemory(Mac, Row.PhysicalAddress, Row.PhysicalAddressLength);
}


EXTERN_C
__declspec(dllexport)
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

The ResolveIpNetEntry2 function resolves the physical address for a neighbor IP address entry on the local computer.

The ResolveIpNetEntry2 function is used to resolve the physical address for a neighbor IP address entry on a local computer. 
This function flushes any existing neighbor entry that matches the IP address on the interface and 
then resolves the physical address (MAC) address by sending ARP requests for an IPv4 address or 
neighbor solicitation requests for an IPv6 address.

In addition, at least one of the following members in the MIB_IPNET_ROW2 structure pointed to the Row parameter must be initialized to the interface: the InterfaceLuid or InterfaceIndex.

The fields are used in the order listed above. So if the InterfaceLuid is specified, 
then this member is used to determine the interface on which to add the unicast IP address. 
If no value was set for the InterfaceLuid member (the values of this member was set to zero),
then the InterfaceIndex member is next used to determine the interface.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-resolveipnetentry2
*/
{
    MIB_IPNET_ROW2 Row = {0};

    IN6_ADDR sin6_addr = {0};
    IN_ADDR sin_addr = {0};

    if (InetPtonA(AF_INET6, ip, &sin6_addr)) {
        Row.Address.si_family = AF_INET6;
        RtlCopyMemory(&Row.Address.Ipv6.sin6_addr, &sin6_addr, sizeof(IN6_ADDR));
    }

    if (InetPtonA(AF_INET, ip, &sin_addr)) {
        Row.Address.si_family = AF_INET;
        Row.Address.Ipv4.sin_addr.S_un.S_addr = sin_addr.S_un.S_addr;
    }

    NTSTATUS status = ResolveIpNetEntry2(&Row, NULL);
    if (NO_ERROR != status) {
        printf("%d\n", status);
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
__declspec(dllexport)
bool WINAPI GetMacByGatewayIPv6(const char * ipv6, PBYTE mac)
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

    PMIB_IPNET_TABLE2 pipTable = NULL;
    unsigned long status = GetIpNetTable2(AF_INET6, &pipTable);
    if (status != NO_ERROR) {
        return ret;
    }

    IN6_ADDR sin6_addr = {0};
    InetPtonA(AF_INET6, ipv6, &sin6_addr);

    for (int i = 0; (unsigned)i < pipTable->NumEntries; i++) {
        if (IN6_ADDR_EQUAL(&sin6_addr, &pipTable->Table[i].Address.Ipv6.sin6_addr)) {
            if (pipTable->Table[i].IsRouter) { // && (pipTable->Table[i].State == NlnsStale)
                if (6 == pipTable->Table[i].PhysicalAddressLength) {
                    RtlCopyMemory(mac, pipTable->Table[i].PhysicalAddress, pipTable->Table[i].PhysicalAddressLength);
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
__declspec(dllexport)
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
    PMIB_IPFORWARDTABLE pIpForwardTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    char szDestIp[128];
    char szMaskIp[128];
    char szGatewayIp[128];
    struct in_addr IpAddr;
    int i;

    pIpForwardTable = (MIB_IPFORWARDTABLE *)MALLOC(sizeof(MIB_IPFORWARDTABLE));
    if (pIpForwardTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pIpForwardTable);
        pIpForwardTable = (MIB_IPFORWARDTABLE *)MALLOC(dwSize);
        if (pIpForwardTable == NULL) {
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
            printf("\tRoute[%d] If Index: %ld\n", i, pIpForwardTable->table[i].dwForwardIfIndex);
            printf("\tRoute[%d] Type: %ld - ", i, pIpForwardTable->table[i].dwForwardType);
            PrintRouteType(pIpForwardTable->table[i].dwForwardType);

            printf("\tRoute[%d] Proto: %ld - ", i, pIpForwardTable->table[i].dwForwardProto);
            PrintRoutingProtocols(pIpForwardTable->table[i].dwForwardProto);

            printf("\tRoute[%d] Age: %ld\n", i, pIpForwardTable->table[i].dwForwardAge);
            printf("\tRoute[%d] Metric1: %ld\n", i, pIpForwardTable->table[i].dwForwardMetric1);
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
__declspec(dllexport)
int WINAPI EnumIpForwardTable2()
/*
The GetIpForwardTable2 function retrieves the IP route entries on the local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getipforwardtable2
https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipforwardtable

微软没有例子，这个是自己写的。
*/
{
    PMIB_IPFORWARD_TABLE2  pIpForwardTable;
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
__declspec(dllexport)
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
    unsigned int i, j;

    /* variables used for GetIfTable and GetIfEntry */
    MIB_IFTABLE * pIfTable;
    MIB_IFROW * pIfRow;

    // Allocate memory for our pointers.
    pIfTable = (MIB_IFTABLE *)MALLOC(sizeof(MIB_IFTABLE));
    if (pIfTable == NULL) {
        printf("Error allocating memory needed to call GetIfTable\n");
        return 1;
    }

    // Make an initial call to GetIfTable to get the necessary size into dwSize
    dwSize = sizeof(MIB_IFTABLE);
    if (GetIfTable(pIfTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pIfTable);
        pIfTable = (MIB_IFTABLE *)MALLOC(dwSize);
        if (pIfTable == NULL) {
            printf("Error allocating memory needed to call GetIfTable\n");
            return 1;
        }
    }

    // Make a second call to GetIfTable to get the actual data we want.
    if ((dwRetVal = GetIfTable(pIfTable, &dwSize, FALSE)) == NO_ERROR) {
        printf("\tNum Entries: %ld\n\n", pIfTable->dwNumEntries);
        for (i = 0; i < pIfTable->dwNumEntries; i++) {
            pIfRow = (MIB_IFROW *)&pIfTable->table[i];
            printf("\tIndex[%d]:\t %ld\n", i, pIfRow->dwIndex);
            printf("\tInterfaceName[%d]:\t %ws", i, pIfRow->wszName);
            printf("\n");
            printf("\tDescription[%d]:\t ", i);

            for (j = 0; j < pIfRow->dwDescrLen; j++)
                printf("%c", pIfRow->bDescr[j]);

            printf("\n");
            printf("\tType[%d]:\t ", i);
            PrintInterfaceType((WORD)pIfRow->dwType);

            printf("\tMtu[%d]:\t\t %ld\n", i, pIfRow->dwMtu);
            printf("\tSpeed[%d]:\t %ld\n", i, pIfRow->dwSpeed);
            printf("\tPhysical Addr:\t ");

            if (pIfRow->dwPhysAddrLen == 0)
                printf("\n");

            for (j = 0; j < pIfRow->dwPhysAddrLen; j++) {
                if (j == (pIfRow->dwPhysAddrLen - 1))
                    printf("%.2X\n", (int)pIfRow->bPhysAddr[j]);
                else
                    printf("%.2X-", (int)pIfRow->bPhysAddr[j]);
            }

            printf("\tAdmin Status[%d]:\t %ld\n", i, pIfRow->dwAdminStatus);
            printf("\tOper Status[%d]:\t ", i);

            PrintInterfaceOperationalStatus(pIfRow->dwOperStatus);

            printf("\n");
        }
    } else {
        printf("GetIfTable failed with error:%d \n", dwRetVal);//MSDN少写%了。
        if (pIfTable != NULL) {
            FREE(pIfTable);
            pIfTable = NULL;
        }

        return 1;
        // Here you can use FormatMessage to find out why it failed.
    }

    if (pIfTable != NULL) {
        FREE(pIfTable);
        pIfTable = NULL;
    }
    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumIfTable2()
/*
微软没有例子，这个是自己写的。
*/
{
    PMIB_IF_TABLE2 table;
    if (GetIfTable2(&table) != ERROR_SUCCESS) {

        return GetLastError();
    }

    printf("Number of entries: %d\n", table->NumEntries);

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
__declspec(dllexport)
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

    printf("Number of entries: %d\n", table->NumEntries);

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
__declspec(dllexport)
int WINAPI GetMacByIPv4Test(int argc, char ** argv)
/*
功能：获取(局域网，不能是互联网)IPv4对应的MAC地址。

The SendARP function sends an Address Resolution Protocol (ARP) request to
obtain the physical address that corresponds to the specified destination IPv4 address.

The following code demonstrates how to obtain the hardware or media access control (MAC) address associated with a specified IPv4 address.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-sendarp
https://msdn.microsoft.com/en-us/library/aa366358(VS.85).aspx
*/
{
    DWORD dwRetVal;
    IPAddr DestIp = 0;
    IPAddr SrcIp = 0;       /* default for src ip */
    ULONG MacAddr[2];       /* for 6-byte hardware addresses */
    ULONG PhysAddrLen = 6;  /* default to length of six bytes */

    char * DestIpString = NULL;
    char * SrcIpString = NULL;

    BYTE * bPhysAddr;
    unsigned int i;

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
                }               /* end switch */
            } else {
                DestIpString = argv[i];
            }
        }                       /* end for */
    } else {
        usage(argv[0]);
    }

    if (DestIpString == NULL || DestIpString[0] == '\0')
        usage(argv[0]);

    DestIp = inet_addr(DestIpString);

    memset(&MacAddr, 0xff, sizeof(MacAddr));

    printf("Sending ARP request for IP address: %s\n", DestIpString);

    dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen);
    if (dwRetVal == NO_ERROR) {
        bPhysAddr = (BYTE *)&MacAddr;
        if (PhysAddrLen) {
            for (i = 0; i < (int)PhysAddrLen; i++) {
                if (i == (PhysAddrLen - 1))
                    printf("%.2X\n", (int)bPhysAddr[i]);
                else
                    printf("%.2X-", (int)bPhysAddr[i]);
            }
        } else {
            printf("Warning: SendArp completed successfully, but returned length=0\n");
        }
    } else {
        printf("Error: SendArp failed with error: %d", dwRetVal);
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
__declspec(dllexport)
int WINAPI GetMacByIPv4(IPAddr DestIp, PBYTE MacAddr)
/*
功能：获取(局域网，不能是互联网)IPv4对应的MAC地址。

用法实例：
BYTE MacAddr[6] = {0};
GetMacByIPv4(inet_addr("192.168.5.1"), MacAddr);
*/
{
    //if (DestIpString == NULL || DestIpString[0] == '\0') {
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
        printf("Error: SendArp failed with error: %d", dwRetVal);
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI GetGatewayMacByIPv4(const char * IPv4, PBYTE GatewayMac)
{
    char Gateway[4 * 4];
    GetGatewayByIPv4(IPv4, Gateway);

    GetMacByIPv4(inet_addr(Gateway), GatewayMac);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
int WINAPI EnumUnicastIpAddressTable()
/*
The following example retrieves a unicast IP address table and
prints some values from each of the retrieved MIB_UNICASTIPADDRESS_ROW structures.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getunicastipaddresstable
*/
{
    // Declare and initialize variables
    unsigned int i;
    DWORD Result = 0;
    WCHAR Ipv4String[16] = {0};
    WCHAR Ipv6String[46] = {0};
    PMIB_UNICASTIPADDRESS_TABLE pipTable = NULL;

    Result = GetUnicastIpAddressTable(AF_UNSPEC, &pipTable);
    if (Result != NO_ERROR) {
        wprintf(L"GetUnicastIpAddressTable returned error: %ld\n", Result);
        return(1);
    }
    // Print some variables from the rows in the table
    wprintf(L"Number of table entries: %d\n\n", pipTable->NumEntries);

    for (i = 0; i < pipTable->NumEntries; i++) {
        wprintf(L"AddressFamily[%d]:\t\t ", i);

        switch (pipTable->Table[i].Address.si_family) {
        case AF_INET:
            wprintf(L"IPv4\n");
            if (InetNtopW(AF_INET, &pipTable->Table[i].Address.Ipv4.sin_addr, Ipv4String, 16) != NULL)
                wprintf(L"IPv4 Address:\t\t\t %ws\n", Ipv4String);
            break;
        case AF_INET6:
            wprintf(L"IPv6\n");
            if (InetNtopW(AF_INET6, &pipTable->Table[i].Address.Ipv6.sin6_addr, Ipv6String, 46) != NULL)
                wprintf(L"IPv6 Address:\t\t\t %ws\n", Ipv6String);
            break;
        default:
            wprintf(L"Other: %d\n", pipTable->Table[i].Address.si_family);
            break;
        }

        wprintf(L"Interface LUID NetLuidIndex[%d]:  %I64u\n",
                i, pipTable->Table[i].InterfaceLuid.Info.NetLuidIndex);
        wprintf(L"Interface LUID IfType[%d]:\t ", i);
        PrintInterfaceType(pipTable->Table[i].InterfaceLuid.Info.IfType);

        wprintf(L"Interface Index[%d]:\t\t %lu\n", i, pipTable->Table[i].InterfaceIndex);

        wprintf(L"Prefix Origin[%d]:\t\t ", i);
        switch (pipTable->Table[i].PrefixOrigin) {
        case IpPrefixOriginOther:
            wprintf(L"IpPrefixOriginOther\n");
            break;
        case IpPrefixOriginManual:
            wprintf(L"IpPrefixOriginManual\n");
            break;
        case IpPrefixOriginWellKnown:
            wprintf(L"IpPrefixOriginWellKnown\n");
            break;
        case IpPrefixOriginDhcp:
            wprintf(L"IpPrefixOriginDhcp\n");
            break;
        case IpPrefixOriginRouterAdvertisement:
            wprintf(L"IpPrefixOriginRouterAdvertisement\n");
            break;
        case IpPrefixOriginUnchanged:
            wprintf(L"IpPrefixOriginUnchanged\n");
            break;
        default:
            wprintf(L"Unknown: %d\n", pipTable->Table[i].PrefixOrigin);
            break;
        }

        wprintf(L"Suffix Origin[%d]:\t\t ", i);
        switch (pipTable->Table[i].SuffixOrigin) {
        case IpSuffixOriginOther:
            wprintf(L"IpSuffixOriginOther\n");
            break;
        case IpSuffixOriginManual:
            wprintf(L"IpSuffixOriginManual\n");
            break;
        case IpSuffixOriginWellKnown:
            wprintf(L"IpSuffixOriginWellKnown\n");
            break;
        case IpSuffixOriginDhcp:
            wprintf(L"IpSuffixOriginDhcp\n");
            break;
        case IpSuffixOriginLinkLayerAddress:
            wprintf(L"IpSuffixOriginLinkLayerAddress\n");
            break;
        case IpSuffixOriginRandom:
            wprintf(L"IpSuffixOriginRandom\n");
            break;
        case IpSuffixOriginUnchanged:
            wprintf(L"IpSuffixOriginUnchanged\n");
            break;
        default:
            wprintf(L"Unknown: %d\n", pipTable->Table[i].SuffixOrigin);
            break;
        }

        wprintf(L"Valid Lifetime[%d]:\t\t 0x%x (%u)\n", i,
                pipTable->Table[i].ValidLifetime,
                pipTable->Table[i].ValidLifetime);

        wprintf(L"Preferred Lifetime[%d]:\t\t 0x%x (%u)\n", i,
                pipTable->Table[i].PreferredLifetime,
                pipTable->Table[i].PreferredLifetime);

        wprintf(L"OnLink PrefixLength[%d]:\t\t %lu\n", i, pipTable->Table[i].OnLinkPrefixLength);

        wprintf(L"Skip As Source[%d]:\t\t ", i);
        if (pipTable->Table[i].SkipAsSource)
            wprintf(L"Yes\n");
        else
            wprintf(L"No\n");

        wprintf(L"Dad State[%d]:\t\t\t ", i);
        switch (pipTable->Table[i].DadState) {
        case IpDadStateInvalid:
            wprintf(L"IpDadStateInvalid\n");
            break;
        case IpDadStateTentative:
            wprintf(L"IpDadStateTentative\n");
            break;
        case IpDadStateDuplicate:
            wprintf(L"IpDadStateDuplicate\n");
            break;
        case IpDadStateDeprecated:
            wprintf(L"IpDadStateDeprecated\n");
            break;
        case IpDadStatePreferred:
            wprintf(L"IpDadStatePreferred\n");
            break;
        default:
            wprintf(L"Unknown: %d\n", pipTable->Table[i].DadState);
            break;
        }

        wprintf(L"\n");
    }

    if (pipTable != NULL) {
        FreeMibTable(pipTable);
        pipTable = NULL;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
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
        printf("Icmp6Createfile returned error: %ld\n", GetLastError());
        return;
    }

    struct sockaddr_in6 SourceAddress = {0};
    InetPtonA(AF_INET6, "fe80::10bb:f0a9:744b:aac2", &SourceAddress.sin6_addr);

    //SourceAddress.sin6_addr = in6addr_any;//这个更通用。
    SourceAddress.sin6_family = AF_INET6;

    struct sockaddr_in6 DestinationAddress = {0};
    InetPtonA(AF_INET6, "fe80::ec0f:f6:4a6d:89d2", &DestinationAddress.sin6_addr);

    DestinationAddress.sin6_family = AF_INET6;

    WORD                     RequestSize = 0;

    /*
    A pointer to the IPv6 header options for the request, 
    in the form of an IP_OPTION_INFORMATION structure.
    On a 64-bit platform, this parameter is in the form for an IP_OPTION_INFORMATION32 structure.

    This parameter may be NULL if no IP header options need to be specified.

    Note  On Windows Server 2003 and Windows XP,
    the RequestOptions parameter is not optional and must not be NULL and 
    only the Ttl and Flags members are used.
    */
    IP_OPTION_INFORMATION  RequestOptions = {30, 0, 0, 0, NULL};

    PVOID                    ReplyBuffer = NULL;

    /*
    The size, in bytes, of the reply buffer pointed to by the ReplyBuffer parameter.
    This buffer should be large enough to hold at least one ICMPV6_ECHO_REPLY structure plus RequestSize bytes of data.
    This buffer should also be large enough to also hold 8 more bytes of data (the size of an ICMP error message) plus space for an IO_STATUS_BLOCK structure.
    */
    DWORD ReplySize = sizeof(ICMPV6_ECHO_REPLY) + RequestSize + 8;// + sizeof(IO_STATUS_BLOCK)

    /*
    The time, in milliseconds, to wait for replies.
    This parameter is only used if the Icmp6SendEcho2 function is called synchronously.
    So this parameter is not used if either the ApcRoutine or Event parameter are not NULL.
    （谷歌翻译：因此，如果 ApcRoutine 或 Event 参数不为 NULL，则不使用此参数。）
    反过来，ApcRoutine 和 Event 参数为 NULL，则使用此参数。

    这个必须设置，不是可选的，且不能为0，你看是_In_，否则，返回ERROR_INVALID_PARAMETER。
    */
    DWORD                    Timeout = 1000;

    ReplyBuffer = (PVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ReplySize);;
    _ASSERTE(ReplyBuffer);

    DWORD ret = Icmp6SendEcho2(hIcmpFile,
                               NULL,
                               NULL,
                               NULL,
                               &SourceAddress,
                               &DestinationAddress,
                               NULL, //RequestData,
                               0, //RequestSize,
                               &RequestOptions,
                               ReplyBuffer,
                               ReplySize,
                               Timeout);
    if (0 == ret) {
        printf("LastError:%ld\n", GetLastError());//ERROR_INVALID_PARAMETER
        IcmpCloseHandle(hIcmpFile);
        return;
    }

#pragma prefast( push )
#pragma prefast( disable: 28020, "表达式“_Param_(2)>sizeof(struct icmpv6_echo_reply_lh ICMPV6_ECHO_REPLY)+8”对此调用无效。" )
    ret = Icmp6ParseReplies(ReplyBuffer, ReplySize);
#pragma prefast( pop )     
    if (0 == ret) {
        printf("LastError:%ld\n", GetLastError());
        IcmpCloseHandle(hIcmpFile);
        return;
    }

    PICMPV6_ECHO_REPLY Reply = static_cast<PICMPV6_ECHO_REPLY>(ReplyBuffer);
    printf("Status:%ld\n", Reply->Status);

    HeapFree(GetProcessHeap(), 0, ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
