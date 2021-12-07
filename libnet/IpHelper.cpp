#include "pch.h"
#include "IpHelper.h"


#pragma warning(disable:4477)
#pragma warning(disable:6101)
#pragma warning(disable:6328)
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
int WINAPI EnumTcpTable()
/*
The GetTcpTable function retrieves the IPv4 TCP connection table.

The following example retrieves the TCP connection table for IPv4 and prints the state of each connection.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcptable
*/
{
    // Declare and initialize variables
    PMIB_TCPTABLE pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    char szLocalAddr[128];
    char szRemoteAddr[128];
    struct in_addr IpAddr;
    int i;

    pTcpTable = (MIB_TCPTABLE *)MALLOC(sizeof(MIB_TCPTABLE));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    dwSize = sizeof(MIB_TCPTABLE);
    // Make an initial call to GetTcpTable to
    // get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCPTABLE *)MALLOC(dwSize);
        if (pTcpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcpTable to get the actual data we require
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
        for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
            strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));

            printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
            PrintTcpConnectionState(pTcpTable->table[i].dwState);
            printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));
            printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);
            printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));
        }
    } else {
        printf("\tGetTcpTable failed with %d\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != NULL) {
        FREE(pTcpTable);
        pTcpTable = NULL;
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumTcpTable2()
/*
The following example retrieves the TCP connection table for IPv4 and prints the state of each connection.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcptable2
*/
{
    // Declare and initialize variables
    PMIB_TCPTABLE2 pTcpTable;
    ULONG ulSize = 0;
    DWORD dwRetVal = 0;
    char szLocalAddr[128];
    char szRemoteAddr[128];
    struct in_addr IpAddr;
    int i;

    pTcpTable = (MIB_TCPTABLE2 *)MALLOC(sizeof(MIB_TCPTABLE2));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    ulSize = sizeof(MIB_TCPTABLE);
    // Make an initial call to GetTcpTable2 to get the necessary size into the ulSize variable
    if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCPTABLE2 *)MALLOC(ulSize);
        if (pTcpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcpTable2 to get the actual data we require
    if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) == NO_ERROR) {
        printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
        for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
            PrintTcpConnectionState(pTcpTable->table[i].dwState);

            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
            printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
            strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
            printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);
            printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

            printf("\tTCP[%d] Owning PID: %d\n", i, pTcpTable->table[i].dwOwningPid);
            printf("\tTCP[%d] Offload State: %ld - ", i, pTcpTable->table[i].dwOffloadState);

            switch (pTcpTable->table[i].dwOffloadState) {
            case TcpConnectionOffloadStateInHost:
                printf("Owned by the network stack and not offloaded \n");
                break;
            case TcpConnectionOffloadStateOffloading:
                printf("In the process of being offloaded\n");
                break;
            case TcpConnectionOffloadStateOffloaded:
                printf("Offloaded to the network interface control\n");
                break;
            case TcpConnectionOffloadStateUploading:
                printf("In the process of being uploaded back to the network stack \n");
                break;
            default:
                printf("UNKNOWN Offload state value\n");
                break;
            }
        }
    } else {
        printf("\tGetTcpTable2 failed with %d\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != NULL) {
        FREE(pTcpTable);
        pTcpTable = NULL;
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumTcp6Table()
/*
The following example retrieves the TCP connection table for IPv6 and prints the state of each connection.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcp6table
*/
{
    // Declare and initialize variables
    PMIB_TCP6TABLE pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    wchar_t ipstringbuffer[46];
    int i;

    pTcpTable = (MIB_TCP6TABLE *)MALLOC(sizeof(MIB_TCP6TABLE));
    if (pTcpTable == NULL) {
        wprintf(L"Error allocating memory\n");
        return 1;
    }

    dwSize = sizeof(MIB_TCP6TABLE);
    // Make an initial call to GetTcp6Table to get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcp6Table(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCP6TABLE *)MALLOC(dwSize);
        if (pTcpTable == NULL) {
            wprintf(L"Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcp6Table to get the actual data we require
    if ((dwRetVal = GetTcp6Table(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        wprintf(L"\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
        for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            wprintf(L"\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].State);
            PrintTcpConnectionState(pTcpTable->table[i].State);

            if (InetNtop(AF_INET6, &pTcpTable->table[i].LocalAddr, ipstringbuffer, 46) == NULL)
                wprintf(L"  InetNtop function failed for local IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Local Addr: %s\n", i, ipstringbuffer);

            wprintf(L"\tTCP[%d] Local Scope ID: %d \n", i, ntohl(pTcpTable->table[i].dwLocalScopeId));
            wprintf(L"\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            if (InetNtop(AF_INET6, &pTcpTable->table[i].RemoteAddr, ipstringbuffer, 46) == NULL)
                wprintf(L"  InetNtop function failed for remote IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Remote Addr: %s\n", i, ipstringbuffer);

            wprintf(L"\tTCP[%d] Remote Scope ID: %d \n", i, ntohl(pTcpTable->table[i].dwRemoteScopeId));
            wprintf(L"\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));
        }
    } else {
        wprintf(L"\tGetTcp6Table failed with %d\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != NULL) {
        FREE(pTcpTable);
        pTcpTable = NULL;
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumTcp6Table2()
/*
这个是自己写的，MSDN没有例子。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcp6table2
*/
{
    // Declare and initialize variables
    PMIB_TCP6TABLE2 pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    int i;

    pTcpTable = (MIB_TCP6TABLE2 *)MALLOC(sizeof(MIB_TCP6TABLE2));
    if (pTcpTable == NULL) {
        wprintf(L"Error allocating memory\n");
        return 1;
    }

    dwSize = sizeof(MIB_TCP6TABLE2);
    // Make an initial call to GetTcp6Table to get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcp6Table2(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCP6TABLE2 *)MALLOC(dwSize);
        if (pTcpTable == NULL) {
            wprintf(L"Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcp6Table to get the actual data we require
    if ((dwRetVal = GetTcp6Table2(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        int n = 0;

        for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            if (MIB_TCP_STATE_LISTEN != pTcpTable->table[i].State) {
                continue;
            }

            wchar_t ipstringbuffer[46];

            if (InetNtop(AF_INET6, &pTcpTable->table[i].LocalAddr, ipstringbuffer, 46) == NULL)
                wprintf(L"  InetNtop function failed for local IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Local Addr: %s\n", i, ipstringbuffer);

            //wprintf(L"\tTCP[%d] Local Scope ID: %d \n", i, ntohl(pUdp6Table->table[i].dwLocalScopeId));
            wprintf(L"\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            if (InetNtop(AF_INET6, &pTcpTable->table[i].RemoteAddr, ipstringbuffer, 46) == NULL)
                wprintf(L"  InetNtop function failed for remote IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Remote Addr: %s\n", i, ipstringbuffer);

            //wprintf(L"\tTCP[%d] Remote Scope ID: %d \n", i, ntohl(pUdp6Table->table[i].dwRemoteScopeId));
            wprintf(L"\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

            wprintf(L"\tTCP[%d] OwningPid: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwOwningPid));

            //char szModName[MAX_PATH] = {0};
            //DWORD Size = _countof(szModName);
            //HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pTcpTable->table[i].dwOwningPid);
            //_ASSERTE(hProcess);
            //BOOL B = QueryFullProcessImageNameA(hProcess, 0, szModName, &Size);
            //_ASSERTE(B);
            //printf("\tTCP[%d] Owning ProcessImageName: %s\n", i, szModName);

            printf("\n");

            n++;
        }

        printf("IPV6的侦听总数:%d\n", n);
    } else {
        wprintf(L"\tGetTcp6Table failed with %d\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != NULL) {
        FREE(pTcpTable);
        pTcpTable = NULL;
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumTcpStatistics()
/*
The following example retrieves the TCP statistics for the local computer and prints some values from the returned data.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcpstatistics

类似的函数还有：
GetTcpStatisticsEx
GetTcpStatisticsEx2
*/
{
    PMIB_TCPSTATS pTCPStats;
    DWORD dwRetVal = 0;

    pTCPStats = (MIB_TCPSTATS *)MALLOC(sizeof(MIB_TCPSTATS));
    if (pTCPStats == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    if ((dwRetVal = GetTcpStatistics(pTCPStats)) == NO_ERROR) {
        printf("\tActive Opens: %ld\n", pTCPStats->dwActiveOpens);
        printf("\tPassive Opens: %ld\n", pTCPStats->dwPassiveOpens);
        printf("\tSegments Recv: %ld\n", pTCPStats->dwInSegs);
        printf("\tSegments Xmit: %ld\n", pTCPStats->dwOutSegs);
        printf("\tTotal # Conxs: %ld\n", pTCPStats->dwNumConns);
    } else {
        printf("GetTcpStatistics failed with error: %ld\n", dwRetVal);

        LPVOID lpMsgBuf;
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL,
                          dwRetVal,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                          (LPTSTR)&lpMsgBuf,
                          0,
                          NULL)) {
            printf("\tError: %s", (char *)lpMsgBuf);
        }
        LocalFree(lpMsgBuf);
    }

    if (pTCPStats)
        FREE(pTCPStats);

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumExtendedTcp4TableByPid()
/*
功能：获取本地的IPv4的TCP的带进程关联的网络信息。

注意：IPV6的及TCP的信息类似。

注释：修改自MSDN的GetTcpTable2的例子。

made in 2016.11.30

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getextendedtcptable
*/
{
    PMIB_TCPTABLE_OWNER_PID pTcpTable;
    ULONG ulSize = 0;
    DWORD dwRetVal = 0;
    char szLocalAddr[128];
    char szRemoteAddr[128];
    struct in_addr IpAddr;

    pTcpTable = (MIB_TCPTABLE_OWNER_PID *)MALLOC(sizeof(MIB_TCPTABLE_OWNER_PID));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    ulSize = sizeof(MIB_TCPTABLE_OWNER_PID);
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &ulSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCPTABLE_OWNER_PID *)MALLOC(ulSize);
        if (pTcpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

#pragma prefast( push )
#pragma prefast( disable: 28020, "表达式“*_Param_(2)>=sizeof(MIB_TCPTABLE)”对此调用无效" )
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &ulSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == NO_ERROR) {
#pragma prefast( pop )      
        printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
        for (DWORD i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
            PrintTcpConnectionState(pTcpTable->table[i].dwState);

            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
            printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);

            printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
            strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
            printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);

            printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

            printf("\tTCP[%d] Owning PID: %d\n", i, pTcpTable->table[i].dwOwningPid);
        }
    } else {
        printf("\tGetTcpTable2 failed with %d\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != NULL) {
        FREE(pTcpTable);
        pTcpTable = NULL;
    }

    return 0;
}


void DumpBasicExtendedTcp4Table(_In_ PMIB_TCPTABLE pTcpTable)
{
    char szLocalAddr[128];
    char szRemoteAddr[128];
    struct in_addr IpAddr;

    printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
        strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

        IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
        strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));
    }
}


void GetOwnerModuleFromTcp4EntryEx(_In_ PMIB_TCPROW_OWNER_MODULE pTcpEntry)
{
    TCPIP_OWNER_MODULE_BASIC_INFO Buffer = {};
    DWORD Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
    DWORD ret = GetOwnerModuleFromTcpEntry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, &Buffer, &Size);
    if (NO_ERROR == ret) {
        printf("\tModuleName: %ls\n", Buffer.pModuleName);
        printf("\tModulePath: %ls\n", Buffer.pModulePath);
        return;
    }

    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PTCPIP_OWNER_MODULE_BASIC_INFO pBuffer = (PTCPIP_OWNER_MODULE_BASIC_INFO)
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
    _ASSERTE(pBuffer);

    ret = GetOwnerModuleFromTcpEntry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, pBuffer, &Size);
    _ASSERTE(NO_ERROR == ret);

    printf("\tModuleName: %ls\n", pBuffer->pModuleName);
    printf("\tModulePath: %ls\n", pBuffer->pModulePath);

    HeapFree(GetProcessHeap(), 0, pBuffer);
}


void DumpModuleExtendedTcp4Table(_In_ PMIB_TCPTABLE_OWNER_MODULE pTcpTable)
{
    char szLocalAddr[128];
    char szRemoteAddr[128];
    struct in_addr IpAddr;

    printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
        strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

        IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
        strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

        printf("\tTCP[%d] OwningPid: %d\n", i, pTcpTable->table[i].dwOwningPid);

        WCHAR TimeString[MAX_PATH] = {0};
        GetTimeString(pTcpTable->table[i].liCreateTimestamp, TimeString);
        printf("\tTCP[%d] CreateTimestamp: %ls\n", i, TimeString);

        //OwningModuleInfo里的TCPIP_OWNING_MODULE_SIZE个数，全都是0.

        GetOwnerModuleFromTcp4EntryEx(&pTcpTable->table[i]);
    }
}


void DumpPidExtendedTcp4Table(_In_ PMIB_TCPTABLE_OWNER_PID pTcpTable)
{
    char szLocalAddr[128];
    char szRemoteAddr[128];
    struct in_addr IpAddr;

    printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
        strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

        IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
        strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

        printf("\tTCP[%d] Owning PID: %d\n", i, pTcpTable->table[i].dwOwningPid);
    }
}


void GetOwnerModuleFromTcp6EntryEx(_In_ PMIB_TCP6ROW_OWNER_MODULE pTcpEntry)
{
    TCPIP_OWNER_MODULE_BASIC_INFO Buffer = {};
    DWORD Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
    DWORD ret = GetOwnerModuleFromTcp6Entry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, &Buffer, &Size);
    if (NO_ERROR == ret) {
        printf("\tModuleName: %ls\n", Buffer.pModuleName);
        printf("\tModulePath: %ls\n", Buffer.pModulePath);
        return;
    }

    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PTCPIP_OWNER_MODULE_BASIC_INFO pBuffer = (PTCPIP_OWNER_MODULE_BASIC_INFO)
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
    _ASSERTE(pBuffer);

    ret = GetOwnerModuleFromTcp6Entry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, pBuffer, &Size);
    _ASSERTE(NO_ERROR == ret);

    printf("\tModuleName: %ls\n", pBuffer->pModuleName);
    printf("\tModulePath: %ls\n", pBuffer->pModulePath);

    HeapFree(GetProcessHeap(), 0, pBuffer);
}


void DumpModuleExtendedTcp6Table(_In_ PMIB_TCP6TABLE_OWNER_MODULE pTcpTable)
{
    char szLocalAddr[128];
    char szRemoteAddr[128];

    printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        InetNtopA(AF_INET6, pTcpTable->table[i].ucLocalAddr, szLocalAddr, _ARRAYSIZE(szLocalAddr));
        printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%d] LocalScopeId: %d \n", i, pTcpTable->table[i].dwLocalScopeId);

        printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

        InetNtopA(AF_INET6, pTcpTable->table[i].ucRemoteAddr, szRemoteAddr, _ARRAYSIZE(szRemoteAddr));
        printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%d] RemoteScopeId: %d \n", i, pTcpTable->table[i].dwRemoteScopeId);

        printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

        printf("\tTCP[%d] OwningPid: %d\n", i, pTcpTable->table[i].dwOwningPid);

        WCHAR TimeString[MAX_PATH] = {0};
        GetTimeString(pTcpTable->table[i].liCreateTimestamp, TimeString);
        printf("\tTCP[%d] CreateTimestamp: %ls\n", i, TimeString);

        for (int j = 0; j < TCPIP_OWNING_MODULE_SIZE; j++) {
            printf("\tTCP[%d] ModuleInfo: %llX\n", i, pTcpTable->table[i].OwningModuleInfo[j]);
        }

        GetOwnerModuleFromTcp6EntryEx(&pTcpTable->table[i]);
    }
}


void DumpPidExtendedTcp6Table(_In_ PMIB_TCP6TABLE_OWNER_PID pTcpTable)
{
    char szLocalAddr[128];
    char szRemoteAddr[128];

    printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        InetNtopA(AF_INET6, pTcpTable->table[i].ucLocalAddr, szLocalAddr, _ARRAYSIZE(szLocalAddr));
        printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%d] LocalScopeId: %d \n", i, pTcpTable->table[i].dwLocalScopeId);

        printf("\tTCP[%d] Local Port: %d \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

        InetNtopA(AF_INET6, pTcpTable->table[i].ucRemoteAddr, szRemoteAddr, _ARRAYSIZE(szRemoteAddr));
        printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%d] RemoteScopeId: %d \n", i, pTcpTable->table[i].dwRemoteScopeId);

        printf("\tTCP[%d] Remote Port: %d\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

        printf("\tTCP[%d] OwningPid: %d\n", i, pTcpTable->table[i].dwOwningPid);
    }
}


void DumpExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass, _In_ PVOID pTcpTable)
/*
功能：打印各种情况下的信息。

When the ulAf parameter is set to AF_INET, 
the following table indicates the TCP table type to retrieve in the structure pointed to by the pTcpTable parameter for each possible TableClass value.
TableClass value	pTcpTable structure
TCP_TABLE_BASIC_ALL	MIB_TCPTABLE
TCP_TABLE_BASIC_CONNECTIONS	MIB_TCPTABLE
TCP_TABLE_BASIC_LISTENER	MIB_TCPTABLE
TCP_TABLE_OWNER_MODULE_ALL	MIB_TCPTABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_CONNECTIONS	MIB_TCPTABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_LISTENER	MIB_TCPTABLE_OWNER_MODULE
TCP_TABLE_OWNER_PID_ALL	MIB_TCPTABLE_OWNER_PID
TCP_TABLE_OWNER_PID_CONNECTIONS	MIB_TCPTABLE_OWNER_PID
TCP_TABLE_OWNER_PID_LISTENER	MIB_TCPTABLE_OWNER_PID

When the ulAf parameter is set to AF_INET6, 
the following table indicates the TCP table type to retrieve in the structure pointed to by the pTcpTable parameter for each possible TableClass value.
TableClass value	pTcpTable structure
TCP_TABLE_OWNER_MODULE_ALL	MIB_TCP6TABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_CONNECTIONS	MIB_TCP6TABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_LISTENER	MIB_TCP6TABLE_OWNER_MODULE
TCP_TABLE_OWNER_PID_ALL	MIB_TCP6TABLE_OWNER_PID
TCP_TABLE_OWNER_PID_CONNECTIONS	MIB_TCP6TABLE_OWNER_PID
TCP_TABLE_OWNER_PID_LISTENER	MIB_TCP6TABLE_OWNER_PID

The GetExtendedTcpTable function called with the ulAf parameter set to AF_INET6 and the TableClass set to TCP_TABLE_BASIC_LISTENER,
TCP_TABLE_BASIC_CONNECTIONS, or TCP_TABLE_BASIC_ALL is not supported and returns ERROR_NOT_SUPPORTED.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getextendedtcptable
*/
{
    switch (ulAf) {
    case AF_INET:
    {
        switch (TableClass) {
        case TCP_TABLE_BASIC_ALL:
        case TCP_TABLE_BASIC_CONNECTIONS:
        case TCP_TABLE_BASIC_LISTENER:
            DumpBasicExtendedTcp4Table((PMIB_TCPTABLE)pTcpTable);
            break;
        case TCP_TABLE_OWNER_MODULE_ALL:
        case TCP_TABLE_OWNER_MODULE_CONNECTIONS:
        case TCP_TABLE_OWNER_MODULE_LISTENER:
            DumpModuleExtendedTcp4Table((PMIB_TCPTABLE_OWNER_MODULE)pTcpTable);
            break;
        case TCP_TABLE_OWNER_PID_ALL:
        case TCP_TABLE_OWNER_PID_CONNECTIONS:
        case TCP_TABLE_OWNER_PID_LISTENER:
            DumpPidExtendedTcp4Table((PMIB_TCPTABLE_OWNER_PID)pTcpTable);
            break;
        default:
            _ASSERTE(FALSE);
            break;
        }

        break;
    }
    case AF_INET6:
    {
        switch (TableClass) {
        case TCP_TABLE_BASIC_LISTENER:
        case TCP_TABLE_BASIC_CONNECTIONS:
        case TCP_TABLE_BASIC_ALL:
            _ASSERTE(FALSE);
            break;
        case TCP_TABLE_OWNER_MODULE_ALL:
        case TCP_TABLE_OWNER_MODULE_CONNECTIONS:
        case TCP_TABLE_OWNER_MODULE_LISTENER:
            DumpModuleExtendedTcp6Table((PMIB_TCP6TABLE_OWNER_MODULE)pTcpTable);
            break;
        case TCP_TABLE_OWNER_PID_ALL:
        case TCP_TABLE_OWNER_PID_CONNECTIONS:
        case TCP_TABLE_OWNER_PID_LISTENER:
            DumpPidExtendedTcp6Table((PMIB_TCP6TABLE_OWNER_PID)pTcpTable);
            break;
        default:
            _ASSERTE(FALSE);
            break;
        }

        break;
    }
    default:
        _ASSERTE(FALSE);
        break;
    }
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass)
/*
功能：获取各类的ExtendedTcpTable信息。

参数：
[in] ulAf
The version of IP used by the TCP endpoints.可选的取值有：AF_INET和AF_INET6。
[in] TableClass
This parameter can be one of the values from the TCP_TABLE_CLASS enumeration.

The GetExtendedTcpTable function called with the ulAf parameter set to AF_INET6 and the TableClass set to TCP_TABLE_BASIC_LISTENER, 
TCP_TABLE_BASIC_CONNECTIONS, or TCP_TABLE_BASIC_ALL is not supported and returns ERROR_NOT_SUPPORTED.

其实这可以设置一个回调函数。
*/
{
    PVOID pTcpTable = MALLOC(sizeof(void *));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    ULONG ulSize = sizeof(void *);
    DWORD dwRetVal = GetExtendedTcpTable(pTcpTable, &ulSize, TRUE, ulAf, TableClass, 0);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = MALLOC(ulSize);
        if (pTcpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

#pragma prefast( push )
#pragma prefast( disable: 28020, "表达式“*_Param_(2)>=sizeof(MIB_TCPTABLE)”对此调用无效" )
    dwRetVal = GetExtendedTcpTable(pTcpTable, &ulSize, TRUE, ulAf, TableClass, 0);
#pragma prefast( pop )      
    if (dwRetVal == NO_ERROR) {
        DumpExtendedTcpTable(ulAf, TableClass, pTcpTable);
    } else {
        printf("\tGetTcpTable2 failed with %d\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != NULL) {
        FREE(pTcpTable);
        pTcpTable = NULL;
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumUdpTable()
/*
这些本地端口应该全是侦听的，但也有可能是连接的通讯的。
注意：有些端口是重复的，所以要去重。包括本地地址。

这个结构里也没有进程信息。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getudptable

MSDN没有例子。
*/
{
    PMIB_UDPTABLE pUdpTable = (MIB_UDPTABLE *)MALLOC(sizeof(MIB_UDPTABLE));
    if (pUdpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_UDPTABLE);

    // Make an initial call to GetUdpTable to get the necessary size into the dwSize variable
    DWORD dwRetVal = 0;
    if ((dwRetVal = GetUdpTable(pUdpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pUdpTable);
        pUdpTable = (MIB_UDPTABLE *)MALLOC(dwSize);
        if (pUdpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetUdpTable to get the actual data we require
    if ((dwRetVal = GetUdpTable(pUdpTable, &dwSize, TRUE)) == NO_ERROR) {

        printf("\tUdpV4的侦听总数:Number of entries: %d\n", (int)pUdpTable->dwNumEntries);

        for (int i = 0; i < (int)pUdpTable->dwNumEntries; i++) {
            char szLocalAddr[128];
            struct in_addr IpAddr;

            IpAddr.S_un.S_addr = (u_long)pUdpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));

            printf("\tUdp[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tUdp[%d] Local Port: %d \n", i, ntohs((u_short)pUdpTable->table[i].dwLocalPort));
            printf("\n");
        }
    } else {
        printf("\tGetUdpTable failed with %d\n", dwRetVal);
        FREE(pUdpTable);
        return 1;
    }

    if (pUdpTable != NULL) {
        FREE(pUdpTable);
        pUdpTable = NULL;
    }

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumUdp6Table()
/*
这些本地端口应该全是侦听的，但也有可能是连接的通讯的。
注意：有些端口是重复的，所以要去重。包括本地地址。

这个结构里也没有进程信息。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getudp6table

MSDN没有例子。
*/
{
    PMIB_UDP6TABLE  pUdp6Table = (MIB_UDP6TABLE *)MALLOC(sizeof(MIB_UDP6TABLE));
    if (pUdp6Table == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_UDP6TABLE);

    // Make an initial call to GetUdpTable to get the necessary size into the dwSize variable
    DWORD dwRetVal = 0;
    if ((dwRetVal = GetUdp6Table(pUdp6Table, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pUdp6Table);
        pUdp6Table = (MIB_UDP6TABLE *)MALLOC(dwSize);
        if (pUdp6Table == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetUdpTable to get the actual data we require
    if ((dwRetVal = GetUdp6Table(pUdp6Table, &dwSize, TRUE)) == NO_ERROR) {

        printf("\tUdpV6的侦听总数:Number of entries: %d\n", (int)pUdp6Table->dwNumEntries);

        for (int i = 0; i < (int)pUdp6Table->dwNumEntries; i++) {
            wchar_t ipstringbuffer[46];

            if (InetNtop(AF_INET6, &pUdp6Table->table[i].dwLocalAddr, ipstringbuffer, 46) == NULL)
                wprintf(L"  InetNtop function failed for local IPv6 address\n");
            else
                wprintf(L"\tUdp[%d] Local Addr: %ls\n", i, ipstringbuffer);

            //printf("\tUdp[%d] LocalScopeId: %d\n", i, pUdp6Table->table[i].dwLocalScopeId);
            printf("\tUdp[%d] Local Port: %d \n", i, ntohs((u_short)pUdp6Table->table[i].dwLocalPort));
            printf("\n");
        }
    } else {
        printf("\tGetUdpTable failed with %d\n", dwRetVal);
        FREE(pUdp6Table);
        return 1;
    }

    if (pUdp6Table != NULL) {
        FREE(pUdp6Table);
        pUdp6Table = NULL;
    }

    return 0;
}


void GetOwnerModuleFromUdpEntryEx(_In_ PMIB_UDPROW_OWNER_MODULE pTcpEntry)
{
    TCPIP_OWNER_MODULE_BASIC_INFO temp = {0};
    DWORD                        Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
    DWORD ret = GetOwnerModuleFromUdpEntry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, &temp, &Size);
    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PTCPIP_OWNER_MODULE_BASIC_INFO ptombi = (PTCPIP_OWNER_MODULE_BASIC_INFO)
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
    _ASSERTE(ptombi);

    ret = GetOwnerModuleFromUdpEntry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, ptombi, &Size);
    _ASSERTE(NO_ERROR == ret);

    printf("\tModuleName: %ls, ModulePath: %ls \n", ptombi->pModuleName, ptombi->pModulePath);

    /*
    pModulePath是全路径，但并非是EXE，而有可能是DLL，这个更精细。
    其实完全可根据dwOwningPid获取EXE的全路径。
    GetOwnerModuleFromPidAndInfo与GetOwnerModuleFromUdpEntry的功能类似。
    */    

    HeapFree(GetProcessHeap(), 0, ptombi);
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumExtendedUdpTable4()
/*

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getextendedudptable

这个是自己写的，MSDN没有例子。
*/
{
    PMIB_UDPTABLE_OWNER_MODULE pUdpTable = (PMIB_UDPTABLE_OWNER_MODULE)MALLOC(sizeof(MIB_UDPTABLE_OWNER_MODULE));
    if (pUdpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_UDPTABLE_OWNER_MODULE);

    // Make an initial call to GetUdpTable to get the necessary size into the dwSize variable
    DWORD dwRetVal = 0;
    dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_MODULE, 0);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pUdpTable);
        pUdpTable = (PMIB_UDPTABLE_OWNER_MODULE)MALLOC(dwSize);
        if (pUdpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetUdpTable to get the actual data we require
    dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_MODULE, 0);
    if (dwRetVal == NO_ERROR) {

        printf("\tUdpV4的总数:Number of entries: %d\n", (int)pUdpTable->dwNumEntries);
        printf("\n");

        for (int i = 0; i < (int)pUdpTable->dwNumEntries; i++) {
            char szLocalAddr[128];
            struct in_addr IpAddr;

            IpAddr.S_un.S_addr = (u_long)pUdpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));

            printf("\tUdp[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tUdp[%d] Local Port: %d \n", i, ntohs((u_short)pUdpTable->table[i].dwLocalPort));
            printf("\tUdp[%d] OwningPid: %d \n", i, (u_short)pUdpTable->table[i].dwOwningPid);

            GetOwnerModuleFromUdpEntryEx(&pUdpTable->table[i]);

            printf("\n");
        }
    } else {
        printf("\tGetExtendedUdpTable failed with %d\n", dwRetVal);
        FREE(pUdpTable);
        return 1;
    }

    if (pUdpTable != NULL) {
        FREE(pUdpTable);
        pUdpTable = NULL;
    }

    return 0;
}


void GetOwnerModuleFromUdp6EntryEx(_In_ PMIB_UDP6ROW_OWNER_MODULE pTcpEntry)
{
    TCPIP_OWNER_MODULE_BASIC_INFO temp = {0};
    DWORD                        Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
    DWORD ret = GetOwnerModuleFromUdp6Entry(pTcpEntry,
                                            TCPIP_OWNER_MODULE_INFO_BASIC,
                                            &temp,
                                            &Size);
    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    PTCPIP_OWNER_MODULE_BASIC_INFO ptombi = (PTCPIP_OWNER_MODULE_BASIC_INFO)
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
    _ASSERTE(ptombi);

    ret = GetOwnerModuleFromUdp6Entry(pTcpEntry,
                                      TCPIP_OWNER_MODULE_INFO_BASIC,
                                      ptombi,
                                      &Size);
    _ASSERTE(NO_ERROR == ret);

    printf("\tModuleName: %ls, ModulePath: %ls \n", ptombi->pModuleName, ptombi->pModulePath);

    /*
    pModulePath是全路径，但并非是EXE，而有可能是DLL，这个更精细。
    其实完全可根据dwOwningPid获取EXE的全路径。
    GetOwnerModuleFromPidAndInfo与GetOwnerModuleFromUdpEntry的功能类似。
    */    

    HeapFree(GetProcessHeap(), 0, ptombi);
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumExtendedUdpTable6()
/*

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getextendedudptable

这个是自己写的，MSDN没有例子。
*/
{
    PMIB_UDP6TABLE_OWNER_MODULE pUdpTable = (PMIB_UDP6TABLE_OWNER_MODULE)MALLOC(sizeof(MIB_UDP6TABLE_OWNER_MODULE));
    if (pUdpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_UDP6TABLE_OWNER_MODULE);

    // Make an initial call to GetUdpTable to get the necessary size into the dwSize variable
    DWORD dwRetVal = 0;
    dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET6, UDP_TABLE_OWNER_MODULE, 0);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pUdpTable);
        pUdpTable = (PMIB_UDP6TABLE_OWNER_MODULE)MALLOC(dwSize);
        if (pUdpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetUdpTable to get the actual data we require
    dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET6, UDP_TABLE_OWNER_MODULE, 0);
    if (dwRetVal == NO_ERROR) {
        printf("\tUdpV6的总数:Number of entries: %d\n", (int)pUdpTable->dwNumEntries);
        printf("\n");

        for (int i = 0; i < (int)pUdpTable->dwNumEntries; i++) {
            char szLocalAddr[128];

            InetNtopA(AF_INET6, &pUdpTable->table[i].ucLocalAddr, szLocalAddr, 46);

            printf("\tUdp[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tUdp[%d] Local Port: %d \n", i, ntohs((u_short)pUdpTable->table[i].dwLocalPort));
            printf("\tUdp[%d] OwningPid: %d \n", i, (u_short)pUdpTable->table[i].dwOwningPid);

            GetOwnerModuleFromUdp6EntryEx(&pUdpTable->table[i]);

            printf("\n");
        }
    } else {
        printf("\tGetExtendedUdpTable failed with %d\n", dwRetVal);
        FREE(pUdpTable);
        return 1;
    }

    if (pUdpTable != NULL) {
        FREE(pUdpTable);
        pUdpTable = NULL;
    }

    return 0;
}


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

    /* Variables used to return error message */
    LPVOID lpMsgBuf;

    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(sizeof(MIB_IPADDRTABLE));

    if (pIPAddrTable) {
        // Make an initial call to GetIpAddrTable to get the
        // necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(dwSize);
        }

        if (pIPAddrTable == NULL) {
            printf("Memory allocation failed for GetIpAddrTable\n");
            exit(1);
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
        exit(1);
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

    exit(0);
}


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
int WINAPI EnumdaptersInfo()
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


EXTERN_C
__declspec(dllexport)
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway)
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
int WINAPI GetGatewayMacByIPv4(const char * IPv4, PBYTE GatewayMac)
{
    char Gateway[4 * 4];
    GetGatewayByIPv4(IPv4, Gateway);

    GetMacByIPv4(inet_addr(Gateway), GatewayMac);

    return 0;
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

        switch (IpNetTable->table[i].Type) {
        case MIB_IPNET_TYPE_OTHER:
            printf("Type: OTHER\n");
            break;
        case MIB_IPNET_TYPE_INVALID:
            printf("Type: INVALID\n");
            break;
        case MIB_IPNET_TYPE_DYNAMIC:
            printf("Type: DYNAMIC\n");
            break;
        case MIB_IPNET_TYPE_STATIC:
            printf("Type: STATIC\n");
            break;
        default:
            printf("Type: %d\n", IpNetTable->table[i].Type);
            break;
        }
    }

    HeapFree(GetProcessHeap(), 0, IpNetTable);

    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumIpv4NetTable2()
/*
The following example retrieves the IP neighbor table, 
then prints the values for IP neighbor row entries in the table.

The GetIpNetTable2 function retrieves the IP neighbor table on the local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getipnettable2
*/
{
    // Declare and initialize variables
    int i;
    unsigned int j;
    unsigned long status = 0;
    PMIB_IPNET_TABLE2 pipTable = NULL;
    //    MIB_IPNET_ROW2 ipRow;

    status = GetIpNetTable2(AF_INET, &pipTable);
    if (status != NO_ERROR) {
        printf("GetIpNetTable for IPv4 table returned error: %ld\n", status);
        exit(1);
    }

    // Print some variables from the table
    printf("Number of IPv4 table entries: %d\n\n", pipTable->NumEntries);

    for (i = 0; (unsigned)i < pipTable->NumEntries; i++) {
        //        printf("Table entry: %d\n", i);
        printf("IPv4 Address[%d]:\t %s\n", (int)i, inet_ntoa(pipTable->Table[i].Address.Ipv4.sin_addr));
        printf("Interface index[%d]:\t\t %lu\n", (int)i, pipTable->Table[i].InterfaceIndex);

        printf("Interface LUID NetLuidIndex[%d]:\t %llu\n", (int)i, pipTable->Table[i].InterfaceLuid.Info.NetLuidIndex);
        printf("Interface LUID IfType[%d]: ", (int)i);
        PrintInterfaceType(pipTable->Table[i].InterfaceLuid.Info.IfType);        

        printf("Physical Address[%d]:\t ", (int)i);
        if (pipTable->Table[i].PhysicalAddressLength == 0)
            printf("\n");
        //        for (j = 0; (unsigned) j < pipTable->Table[i].PhysicalAddressLength; j++)
        //         printf ("%c" 
        for (j = 0; j < pipTable->Table[i].PhysicalAddressLength; j++) {
            if (j == (pipTable->Table[i].PhysicalAddressLength - 1))
                printf("%.2X\n", (int)pipTable->Table[i].PhysicalAddress[j]);
            else
                printf("%.2X-", (int)pipTable->Table[i].PhysicalAddress[j]);
        }

        printf("Physical Address Length[%d]:\t %lu\n", (int)i, pipTable->Table[i].PhysicalAddressLength);

        printf("Neighbor State[%d]:\t ", (int)i);
        PrintNeighborState(pipTable->Table[i].State);

        printf("Flags[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].Flags);

        printf("ReachabilityTime[%d]:\t %lu, %lu\n\n", (int)i,
               pipTable->Table[i].ReachabilityTime.LastReachable,
               pipTable->Table[i].ReachabilityTime.LastUnreachable);
    }

    FreeMibTable(pipTable);
    pipTable = NULL;

    //exit(0);
    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumIpv6NetTable2()
/*
The following example retrieves the IP neighbor table,
then prints the values for IP neighbor row entries in the table.

The GetIpNetTable2 function retrieves the IP neighbor table on the local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getipnettable2
*/
{
    // Declare and initialize variables
    int i;
    unsigned int j;
    unsigned long status = 0;
    PMIB_IPNET_TABLE2 pipTable = NULL;
    //    MIB_IPNET_ROW2 ipRow;

    status = GetIpNetTable2(AF_INET6, &pipTable);
    if (status != NO_ERROR) {
        printf("GetIpNetTable for IPv6 table returned error: %ld\n", status);
        exit(1);
    }

    printf("Number of IPv6 table entries: %d\n\n", pipTable->NumEntries);

    for (i = 0; (unsigned)i < pipTable->NumEntries; i++) {
        //        printf("Table entry: %d\n", i);

        wchar_t IPv6[46] = {0};
        InetNtop(AF_INET6, &pipTable->Table[i].Address.Ipv6.sin6_addr, IPv6, _ARRAYSIZE(IPv6));
        printf("IPv4 Address[%d]:\t %ls\n", (int)i, IPv6);

        printf("Interface index[%d]:\t\t %lu\n", (int)i, pipTable->Table[i].InterfaceIndex);

        printf("Interface LUID NetLuidIndex[%d]:\t %llu\n", (int)i, pipTable->Table[i].InterfaceLuid.Info.NetLuidIndex);
        printf("Interface LUID IfType[%d]: ", (int)i);
        PrintInterfaceType(pipTable->Table[i].InterfaceLuid.Info.IfType);        

        printf("Physical Address[%d]:\t ", (int)i);
        if (pipTable->Table[i].PhysicalAddressLength == 0)
            printf("\n");
        //        for (j = 0; (unsigned) j < pipTable->Table[i].PhysicalAddressLength; j++)
        //         printf ("%c" 
        for (j = 0; j < pipTable->Table[i].PhysicalAddressLength; j++) {
            if (j == (pipTable->Table[i].PhysicalAddressLength - 1))
                printf("%.2X\n", (int)pipTable->Table[i].PhysicalAddress[j]);
            else
                printf("%.2X-", (int)pipTable->Table[i].PhysicalAddress[j]);
        }

        printf("Physical Address Length[%d]:\t %lu\n", (int)i, pipTable->Table[i].PhysicalAddressLength);

        printf("Neighbor State[%d]:\t ", (int)i);
        PrintNeighborState(pipTable->Table[i].State);

        printf("Flags[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].Flags);

        printf("ReachabilityTime[%d]:\t %lu, %lu\n\n", (int)i,
               pipTable->Table[i].ReachabilityTime.LastReachable,
               pipTable->Table[i].ReachabilityTime.LastUnreachable);
    }

    FreeMibTable(pipTable);
    pipTable = NULL;

    //exit(0);
    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumIpNetTable2()
/*
The following example retrieves the IP neighbor table,
then prints the values for IP neighbor row entries in the table.

The GetIpNetTable2 function retrieves the IP neighbor table on the local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-getipnettable2
*/
{
    // Declare and initialize variables
    int i;
    unsigned int j;
    unsigned long status = 0;
    PMIB_IPNET_TABLE2 pipTable = NULL;
    //    MIB_IPNET_ROW2 ipRow;

    status = GetIpNetTable2(AF_UNSPEC, &pipTable);
    if (status != NO_ERROR) {
        printf("GetIpNetTable for IP table returned error: %ld\n", status);
        exit(1);
    }

    // Print some variables from the table
    printf("Number of IP table entries: %d\n\n", pipTable->NumEntries);

    for (i = 0; (unsigned)i < pipTable->NumEntries; i++) {
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
        for (j = 0; j < pipTable->Table[i].PhysicalAddressLength; j++) {
            if (j == (pipTable->Table[i].PhysicalAddressLength - 1))
                printf("%.2X\n", (int)pipTable->Table[i].PhysicalAddress[j]);
            else
                printf("%.2X-", (int)pipTable->Table[i].PhysicalAddress[j]);
        }

        printf("Physical Address Length[%d]:\t %lu\n", (int)i, pipTable->Table[i].PhysicalAddressLength);

        printf("Neighbor State[%d]:\t ", (int)i);
        PrintNeighborState(pipTable->Table[i].State);
        
        printf("Flags[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].Flags);

        printf("ReachabilityTime[%d]:\t %lu, %lu\n\n", (int)i,
               pipTable->Table[i].ReachabilityTime.LastReachable,
               pipTable->Table[i].ReachabilityTime.LastUnreachable);
    }

    FreeMibTable(pipTable);
    pipTable = NULL;

    //exit(0);
    return 0;
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumIpForwardTable()
/*
* IPv4 routing table
*
//The GetIpForwardTable function retrieves the IPv4 routing table.

//The following example retrieves the IP routing table then prints some fields for each route in the table.

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

    /* Note that the IPv4 addresses returned in
     * GetIpForwardTable entries are in network byte order
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
            switch (pIpForwardTable->table[i].dwForwardType) {
            case MIB_IPROUTE_TYPE_OTHER:
                printf("other\n");
                break;
            case MIB_IPROUTE_TYPE_INVALID:
                printf("invalid route\n");
                break;
            case MIB_IPROUTE_TYPE_DIRECT:
                printf("local route where next hop is final destination\n");
                break;
            case MIB_IPROUTE_TYPE_INDIRECT:
                printf
                ("remote route where next hop is not final destination\n");
                break;
            default:
                printf("UNKNOWN Type value\n");
                break;
            }

            printf("\tRoute[%d] Proto: %ld - ", i, pIpForwardTable->table[i].dwForwardProto);
            switch (pIpForwardTable->table[i].dwForwardProto) {
            case MIB_IPPROTO_OTHER:
                printf("other\n");
                break;
            case MIB_IPPROTO_LOCAL:
                printf("local interface\n");
                break;
            case MIB_IPPROTO_NETMGMT:
                printf("static route set through network management \n");
                break;
            case MIB_IPPROTO_ICMP:
                printf("result of ICMP redirect\n");
                break;
            case MIB_IPPROTO_EGP:
                printf("Exterior Gateway Protocol (EGP)\n");
                break;
            case MIB_IPPROTO_GGP:
                printf("Gateway-to-Gateway Protocol (GGP)\n");
                break;
            case MIB_IPPROTO_HELLO:
                printf("Hello protocol\n");
                break;
            case MIB_IPPROTO_RIP:
                printf("Routing Information Protocol (RIP)\n");
                break;
            case MIB_IPPROTO_IS_IS:
                printf("Intermediate System-to-Intermediate System (IS-IS) protocol\n");
                break;
            case MIB_IPPROTO_ES_IS:
                printf("End System-to-Intermediate System (ES-IS) protocol\n");
                break;
            case MIB_IPPROTO_CISCO:
                printf("Cisco Interior Gateway Routing Protocol (IGRP)\n");
                break;
            case MIB_IPPROTO_BBN:
                printf("BBN Internet Gateway Protocol (IGP) using SPF\n");
                break;
            case MIB_IPPROTO_OSPF:
                printf("Open Shortest Path First (OSPF) protocol\n");
                break;
            case MIB_IPPROTO_BGP:
                printf("Border Gateway Protocol (BGP)\n");
                break;
            case MIB_IPPROTO_NT_AUTOSTATIC:
                printf("special Windows auto static route\n");
                break;
            case MIB_IPPROTO_NT_STATIC:
                printf("special Windows static route\n");
                break;
            case MIB_IPPROTO_NT_STATIC_NON_DOD:
                printf
                ("special Windows static route not based on Internet standards\n");
                break;
            default:
                printf("UNKNOWN Proto value\n");
                break;
            }

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

            switch (pIfRow->dwOperStatus) {
            case IF_OPER_STATUS_NON_OPERATIONAL:
                printf("Non Operational\n");
                break;
            case IF_OPER_STATUS_UNREACHABLE:
                printf("Unreachable\n");
                break;
            case IF_OPER_STATUS_DISCONNECTED:
                printf("Disconnected\n");
                break;
            case IF_OPER_STATUS_CONNECTING:
                printf("Connecting\n");
                break;
            case IF_OPER_STATUS_CONNECTED:
                printf("Connected\n");
                break;
            case IF_OPER_STATUS_OPERATIONAL:
                printf("Operational\n");
                break;
            default:
                printf("Unknown status %ld\n", pIfRow->dwAdminStatus);
                break;
            }

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
int WINAPI EnumIfTable2(int argc, char ** argv)
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
int WINAPI EnumIfTable2Ex(int argc, char ** argv)
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


// Need to run as administrator

// An array of name for the TCP_ESTATS_TYPE enum values
// The names values must match the enum values
const wchar_t * estatsTypeNames[] = {
    L"TcpConnectionEstatsSynOpts",
    L"TcpConnectionEstatsData",
    L"TcpConnectionEstatsSndCong",
    L"TcpConnectionEstatsPath",
    L"TcpConnectionEstatsSendBuff",
    L"TcpConnectionEstatsRec",
    L"TcpConnectionEstatsObsRec",
    L"TcpConnectionEstatsBandwidth",
    L"TcpConnectionEstatsFineRtt",
    L"TcpConnectionEstatsMaximum"
};

// Function prototypes

// Run tests for IPv4 or IPv4 TCP extended stats
DWORD RunEstatsTest(bool v6);

// Get an IPv4 TCP row entry
DWORD GetTcpRow(u_short localPort, u_short remotePort, MIB_TCP_STATE state, __out PMIB_TCPROW row);

// Get an IPv6 TCP row entry
DWORD GetTcp6Row(u_short localPort, u_short remotePort, MIB_TCP_STATE state, __out PMIB_TCP6ROW row);

// Enable or disable the supplied Estat type on a TCP connection
void ToggleEstat(PVOID row, TCP_ESTATS_TYPE type, bool enable, bool v6);

// Toggle all Estats for a TCP connection
void ToggleAllEstats(void * row, bool enable, bool v6);

// Dump the supplied Estate type data on the given TCP connection row
void GetAndOutputEstats(void * row, TCP_ESTATS_TYPE type, bool v6);

void GetAllEstats(void * row, bool v6);

// Creates a TCP server and client socket on the loopback address.
// Binds the server socket to a port.
// Establishes a client TCP connection to the server 
int CreateTcpConnection(bool v6, SOCKET * serviceSocket, SOCKET * clientSocket,
                        SOCKET * acceptSocket, u_short * serverPort,
                        u_short * clientPort);


EXTERN_C
__declspec(dllexport)
int WINAPI RunEstats()
{
    RunEstatsTest(FALSE);
    RunEstatsTest(TRUE);
    return (0);
}


DWORD RunEstatsTest(bool v6)
// Create connect and listen sockets on loopback interface and dump all Estats
// types on the created TCP connections for the supplied IP address type.
// https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getpertcp6connectionestats
{
    SOCKET serviceSocket, clientSocket, acceptSocket;
    serviceSocket = clientSocket = acceptSocket = INVALID_SOCKET;
    MIB_TCPROW server4ConnectRow, client4ConnectRow;
    MIB_TCP6ROW server6ConnectRow, client6ConnectRow;
    void * serverConnectRow, * clientConnectRow = NULL;
    bool bWSAStartup = false;

    char * buff = (char *)malloc(1000);
    if (buff == NULL) {
        wprintf(L"\nFailed to allocate memory.");
        goto bail;
    }

    if (v6) {
        serverConnectRow = &server6ConnectRow;
        clientConnectRow = &client6ConnectRow;
    } else {
        serverConnectRow = &server4ConnectRow;
        clientConnectRow = &client4ConnectRow;
    }

    UINT winStatus;
    int sockStatus;
    u_short serverPort, clientPort;

    // Initialize Winsock.
    WSADATA wsaData;
    winStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (winStatus != ERROR_SUCCESS) {
        wprintf(L"\nFailed to open winsock. Error %d", winStatus);
        goto bail;
    }

    bWSAStartup = true;

    // Create TCP connection on which Estats information will be collected.
    // Obtain port numbers of created connections.
    winStatus =
        CreateTcpConnection(v6, &serviceSocket, &clientSocket, &acceptSocket, &serverPort, &clientPort);
    if (winStatus != ERROR_SUCCESS) {
        wprintf(L"\nFailed to create TCP connection. Error %d", winStatus);
        goto bail;
    }

    // Obtain MIB_TCPROW corresponding to the TCP connection.
    winStatus = v6 ?
        GetTcp6Row(serverPort, clientPort, MIB_TCP_STATE_ESTAB, (PMIB_TCP6ROW)serverConnectRow) :
        GetTcpRow(serverPort, clientPort, MIB_TCP_STATE_ESTAB, (PMIB_TCPROW)serverConnectRow);
    if (winStatus != ERROR_SUCCESS) {
        wprintf
        (L"\nGetTcpRow failed on the server established connection with %d",
         winStatus);
        goto bail;
    }

    winStatus = v6 ?
        GetTcp6Row(clientPort, serverPort, MIB_TCP_STATE_ESTAB, (PMIB_TCP6ROW)clientConnectRow) :
        GetTcpRow(clientPort, serverPort, MIB_TCP_STATE_ESTAB, (PMIB_TCPROW)clientConnectRow);
    if (winStatus != ERROR_SUCCESS) {
        wprintf
        (L"\nGetTcpRow failed on the client established connection with %d",
         winStatus);
        goto bail;
    }

    // Enable Estats collection and dump current stats.
    ToggleAllEstats(serverConnectRow, TRUE, v6);
    ToggleAllEstats(clientConnectRow, TRUE, v6);
    wprintf(L"\n\n\nDumping Estats for server socket:\n");
    GetAllEstats(serverConnectRow, v6);
    wprintf(L"\n\n\nDumping Estats for client connect socket:\n");
    GetAllEstats(clientConnectRow, v6);

    // Initiate TCP data transfers to see effect on Estats counters.
    sockStatus = send(clientSocket, buff, (int)(1000 * sizeof(char)), 0);
    if (sockStatus == SOCKET_ERROR) {
        wprintf(L"\nFailed to send from client to server %d", WSAGetLastError());
    } else {
        sockStatus = recv(acceptSocket, buff, (int)(1000 * sizeof(char)), 0);
        if (sockStatus == SOCKET_ERROR) {
            wprintf(L"\nFailed to receive data on the server %d", WSAGetLastError());
        }
    }

    // Dump updated Estats and disable Estats collection.
    wprintf(L"\n\n\nDumping Estats for server socket after client sends data:\n");
    GetAllEstats(serverConnectRow, v6);
    wprintf(L"\n\n\nDumping Estats for client socket after client sends data:\n");
    GetAllEstats(clientConnectRow, v6);
    ToggleAllEstats(serverConnectRow, FALSE, v6);
    ToggleAllEstats(clientConnectRow, FALSE, v6);

bail:
    if (serviceSocket != INVALID_SOCKET)
        closesocket(serviceSocket);
    if (clientSocket != INVALID_SOCKET)
        closesocket(clientSocket);
    if (acceptSocket != INVALID_SOCKET)
        closesocket(acceptSocket);
    if (buff != NULL)
        free(buff);
    if (bWSAStartup)
        WSACleanup();
    return ERROR_SUCCESS;
}


int CreateTcpConnection(bool v6,
                        SOCKET * serviceSocket,
                        SOCKET * clientSocket,
                        SOCKET * acceptSocket,
                        u_short * serverPort, 
                        u_short * clientPort
)
{
    INT status;
    ADDRINFOW hints, * localhost = NULL;
    const wchar_t * loopback;
    loopback = v6 ? L"::1" : L"127.0.0.1";
    int aiFamily = v6 ? AF_INET6 : AF_INET;
    int nameLen = sizeof(SOCKADDR_STORAGE);

    *serviceSocket = INVALID_SOCKET;
    *clientSocket = INVALID_SOCKET;
    *acceptSocket = INVALID_SOCKET;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = aiFamily;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    status = GetAddrInfoW(loopback, L"", &hints, &localhost);
    if (status != ERROR_SUCCESS) {
        wprintf(L"\nFailed to open localhost. Error %d", status);
        goto bail;
    }

    *serviceSocket = socket(aiFamily, SOCK_STREAM, IPPROTO_TCP);
    if (*serviceSocket == INVALID_SOCKET) {
        wprintf(L"\nFailed to create server socket. Error %d", WSAGetLastError());
        goto bail;
    }

    *clientSocket = socket(aiFamily, SOCK_STREAM, IPPROTO_TCP);
    if (*clientSocket == INVALID_SOCKET) {
        wprintf(L"\nFailed to create client socket. Error %d", WSAGetLastError());
        goto bail;
    }

    status = bind(*serviceSocket, localhost->ai_addr, (int)localhost->ai_addrlen);
    if (status == SOCKET_ERROR) {
        wprintf(L"\nFailed to bind server socket to loopback. Error %d", WSAGetLastError());
        goto bail;
    }

    if (localhost != NULL) {
        FreeAddrInfoW(localhost);
        localhost = NULL;
    }

    SOCKADDR_STORAGE serverSockName, clientSockName;
    status = getsockname(*serviceSocket, (sockaddr *)&serverSockName, &nameLen);
    if (status == SOCKET_ERROR) {
        wprintf(L"\ngetsockname failed %d", WSAGetLastError());
        goto bail;
    }
    if (v6) {
        *serverPort = ((sockaddr_in6 *)(&serverSockName))->sin6_port;
    } else {
        *serverPort = ((sockaddr_in *)(&serverSockName))->sin_port;
    }

    status = listen(*serviceSocket, SOMAXCONN);
    if (status == SOCKET_ERROR) {
        wprintf(L"\nFailed to listen on server socket. Error %d", WSAGetLastError());
        goto bail;
    }

    status = connect(*clientSocket, (sockaddr *)&serverSockName, (int)sizeof(SOCKADDR_STORAGE));
    if (status == SOCKET_ERROR) {
        wprintf(L"\nCould not connect client and server sockets %d", WSAGetLastError());
        goto bail;
    }

    status = getsockname(*clientSocket, (sockaddr *)&clientSockName, &nameLen);
    if (status == SOCKET_ERROR) {
        wprintf(L"\ngetsockname failed %d", WSAGetLastError());
        goto bail;
    }
    if (v6) {
        *clientPort = ((sockaddr_in6 *)(&clientSockName))->sin6_port;
    } else {
        *clientPort = ((sockaddr_in *)(&clientSockName))->sin_port;
    }

    *acceptSocket = accept(*serviceSocket, NULL, NULL);
    if (*acceptSocket == INVALID_SOCKET) {
        wprintf(L"\nFailed to accept socket connection %d", WSAGetLastError());
        goto bail;
    }

    return ERROR_SUCCESS;

bail:
    if (localhost != NULL)
        FreeAddrInfoW(localhost);

    if (*serviceSocket != INVALID_SOCKET) {
        closesocket(*serviceSocket);
        *serviceSocket = INVALID_SOCKET;
    }

    if (*clientSocket != INVALID_SOCKET) {
        closesocket(*clientSocket);
        *clientSocket = INVALID_SOCKET;
    }

    if (*acceptSocket != INVALID_SOCKET) {
        closesocket(*acceptSocket);
        *acceptSocket = INVALID_SOCKET;
    }

    return status;
}


void GetAllEstats(void * row, bool v6)
{
    GetAndOutputEstats(row, TcpConnectionEstatsSynOpts, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsData, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsSndCong, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsPath, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsSendBuff, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsRec, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsObsRec, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsBandwidth, v6);
    GetAndOutputEstats(row, TcpConnectionEstatsFineRtt, v6);
}


DWORD GetTcpRow(u_short localPort, u_short remotePort, MIB_TCP_STATE state, __out PMIB_TCPROW row)
// Returns a MIB_TCPROW corresponding to the local port, remote port and state filter parameters.
{
    PMIB_TCPTABLE tcpTable = NULL;
    PMIB_TCPROW tcpRowIt = NULL;

    DWORD status, size = 0, i;
    bool connectionFound = FALSE;

    status = GetTcpTable(tcpTable, &size, TRUE);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        return status;
    }

    tcpTable = (PMIB_TCPTABLE)malloc(size);
    if (tcpTable == NULL) {
        return ERROR_OUTOFMEMORY;
    }

    status = GetTcpTable(tcpTable, &size, TRUE);
    if (status != ERROR_SUCCESS) {
        free(tcpTable);
        return status;
    }

    for (i = 0; i < tcpTable->dwNumEntries; i++) {
        tcpRowIt = &tcpTable->table[i];
        if (tcpRowIt->dwLocalPort == (DWORD)localPort &&
            tcpRowIt->dwRemotePort == (DWORD)remotePort &&
            tcpRowIt->State == state) {
            connectionFound = TRUE;
            *row = *tcpRowIt;
            break;
        }
    }

    free(tcpTable);

    if (connectionFound) {
        return ERROR_SUCCESS;
    } else {
        return ERROR_NOT_FOUND;
    }
}


DWORD GetTcp6Row(u_short localPort, u_short remotePort, MIB_TCP_STATE state, __out PMIB_TCP6ROW row)
// Returns a MIB_TCP6ROW corresponding to the local port, remote port and state
// filter parameters. This is a v6 equivalent of the GetTcpRow function.
{
    PMIB_TCP6TABLE tcp6Table = NULL;
    PMIB_TCP6ROW tcp6RowIt = NULL;

    DWORD status, size = 0, i;
    bool connectionFound = FALSE;

    status = GetTcp6Table(tcp6Table, &size, TRUE);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        return status;
    }

    tcp6Table = (PMIB_TCP6TABLE)malloc(size);
    if (tcp6Table == NULL) {
        return ERROR_OUTOFMEMORY;
    }

    status = GetTcp6Table(tcp6Table, &size, TRUE);
    if (status != ERROR_SUCCESS) {
        free(tcp6Table);
        return status;
    }

    for (i = 0; i < tcp6Table->dwNumEntries; i++) {
        tcp6RowIt = &tcp6Table->table[i];
        if (tcp6RowIt->dwLocalPort == (DWORD)localPort &&
            tcp6RowIt->dwRemotePort == (DWORD)remotePort &&
            tcp6RowIt->State == state) {
            connectionFound = TRUE;
            *row = *tcp6RowIt;
            break;
        }
    }

    free(tcp6Table);

    if (connectionFound) {
        return ERROR_SUCCESS;
    } else {
        return ERROR_NOT_FOUND;
    }
}


void ToggleEstat(PVOID row, TCP_ESTATS_TYPE type, bool enable, bool v6)
// Enable or disable the supplied Estat type on a TCP connection.
{
    TCP_BOOLEAN_OPTIONAL operation = enable ? TcpBoolOptEnabled : TcpBoolOptDisabled;
    ULONG status, size = 0;
    PUCHAR rw = NULL;
    TCP_ESTATS_DATA_RW_v0 dataRw;
    TCP_ESTATS_SND_CONG_RW_v0 sndRw;
    TCP_ESTATS_PATH_RW_v0 pathRw;
    TCP_ESTATS_SEND_BUFF_RW_v0 sendBuffRw;
    TCP_ESTATS_REC_RW_v0 recRw;
    TCP_ESTATS_OBS_REC_RW_v0 obsRecRw;
    TCP_ESTATS_BANDWIDTH_RW_v0 bandwidthRw;
    TCP_ESTATS_FINE_RTT_RW_v0 fineRttRw;

    switch (type) {
    case TcpConnectionEstatsData:
        dataRw.EnableCollection = enable;
        rw = (PUCHAR)&dataRw;
        size = sizeof(TCP_ESTATS_DATA_RW_v0);
        break;
    case TcpConnectionEstatsSndCong:
        sndRw.EnableCollection = enable;
        rw = (PUCHAR)&sndRw;
        size = sizeof(TCP_ESTATS_SND_CONG_RW_v0);
        break;
    case TcpConnectionEstatsPath:
        pathRw.EnableCollection = enable;
        rw = (PUCHAR)&pathRw;
        size = sizeof(TCP_ESTATS_PATH_RW_v0);
        break;
    case TcpConnectionEstatsSendBuff:
        sendBuffRw.EnableCollection = enable;
        rw = (PUCHAR)&sendBuffRw;
        size = sizeof(TCP_ESTATS_SEND_BUFF_RW_v0);
        break;
    case TcpConnectionEstatsRec:
        recRw.EnableCollection = enable;
        rw = (PUCHAR)&recRw;
        size = sizeof(TCP_ESTATS_REC_RW_v0);
        break;
    case TcpConnectionEstatsObsRec:
        obsRecRw.EnableCollection = enable;
        rw = (PUCHAR)&obsRecRw;
        size = sizeof(TCP_ESTATS_OBS_REC_RW_v0);
        break;
    case TcpConnectionEstatsBandwidth:
        bandwidthRw.EnableCollectionInbound = operation;
        bandwidthRw.EnableCollectionOutbound = operation;
        rw = (PUCHAR)&bandwidthRw;
        size = sizeof(TCP_ESTATS_BANDWIDTH_RW_v0);
        break;
    case TcpConnectionEstatsFineRtt:
        fineRttRw.EnableCollection = enable;
        rw = (PUCHAR)&fineRttRw;
        size = sizeof(TCP_ESTATS_FINE_RTT_RW_v0);
        break;
    default:
        return;
        break;
    }

    if (v6) {
        status = SetPerTcp6ConnectionEStats((PMIB_TCP6ROW)row, type, rw, 0, size, 0);
    } else {
        status = SetPerTcpConnectionEStats((PMIB_TCPROW)row, type, rw, 0, size, 0);
    }

    if (status != NO_ERROR) {
        if (v6)
            wprintf(L"\nSetPerTcp6ConnectionEStats %s %s failed. status = %d",
                    estatsTypeNames[type], enable ? L"enabled" : L"disabled",
                    status);
        else
            wprintf(L"\nSetPerTcpConnectionEStats %s %s failed. status = %d",
                    estatsTypeNames[type], enable ? L"enabled" : L"disabled",
                    status);
    }
}


void ToggleAllEstats(void * row, bool enable, bool v6)
// Toggle all Estats for a TCP connection.
{
    ToggleEstat(row, TcpConnectionEstatsData, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsSndCong, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsPath, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsSendBuff, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsRec, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsObsRec, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsBandwidth, enable, v6);
    ToggleEstat(row, TcpConnectionEstatsFineRtt, enable, v6);
}


ULONG GetConnectionEStats(void * row, 
                          TCP_ESTATS_TYPE type, 
                          PUCHAR rw, 
                          ULONG rwSize,
                          bool v6, 
                          PUCHAR ros, 
                          ULONG rosSize, 
                          PUCHAR rod, 
                          ULONG rodSize
)
// Call GetPerTcp6ConnectionEStats or GetPerTcpConnectionEStats.
{
    if (v6) {
        return GetPerTcp6ConnectionEStats((PMIB_TCP6ROW)row,
                                          type,
                                          rw, 0, rwSize,
                                          ros, 0, rosSize,
                                          rod, 0, rodSize);
    } else {
        return GetPerTcpConnectionEStats((PMIB_TCPROW)row,
                                         type,
                                         rw, 0, rwSize,
                                         ros, 0, rosSize,
                                         rod, 0, rodSize);
    }
}


void GetAndOutputEstats(void * row, TCP_ESTATS_TYPE type, bool v6)
// Dump the supplied Estate type on the given TCP connection row.
{
    ULONG rosSize = 0, rodSize = 0;
    ULONG winStatus;
    PUCHAR ros = NULL, rod = NULL;

    PTCP_ESTATS_SYN_OPTS_ROS_v0 synOptsRos = {0};
    PTCP_ESTATS_DATA_ROD_v0 dataRod = {0};
    PTCP_ESTATS_SND_CONG_ROD_v0 sndCongRod = {0};
    PTCP_ESTATS_SND_CONG_ROS_v0 sndCongRos = {0};
    PTCP_ESTATS_PATH_ROD_v0 pathRod = {0};
    PTCP_ESTATS_SEND_BUFF_ROD_v0 sndBuffRod = {0};
    PTCP_ESTATS_REC_ROD_v0 recRod = {0};
    PTCP_ESTATS_OBS_REC_ROD_v0 obsRecRod = {0};
    PTCP_ESTATS_BANDWIDTH_ROD_v0 bandwidthRod = {0};
    PTCP_ESTATS_FINE_RTT_ROD_v0 fineRttRod = {0};

    switch (type) {
    case TcpConnectionEstatsSynOpts:
        rosSize = sizeof(TCP_ESTATS_SYN_OPTS_ROS_v0);
        break;
    case TcpConnectionEstatsData:
        rodSize = sizeof(TCP_ESTATS_DATA_ROD_v0);
        break;
    case TcpConnectionEstatsSndCong:
        rodSize = sizeof(TCP_ESTATS_SND_CONG_ROD_v0);
        rosSize = sizeof(TCP_ESTATS_SND_CONG_ROS_v0);
        break;
    case TcpConnectionEstatsPath:
        rodSize = sizeof(TCP_ESTATS_PATH_ROD_v0);
        break;
    case TcpConnectionEstatsSendBuff:
        rodSize = sizeof(TCP_ESTATS_SEND_BUFF_ROD_v0);
        break;
    case TcpConnectionEstatsRec:
        rodSize = sizeof(TCP_ESTATS_REC_ROD_v0);
        break;
    case TcpConnectionEstatsObsRec:
        rodSize = sizeof(TCP_ESTATS_OBS_REC_ROD_v0);
        break;
    case TcpConnectionEstatsBandwidth:
        rodSize = sizeof(TCP_ESTATS_BANDWIDTH_ROD_v0);
        break;
    case TcpConnectionEstatsFineRtt:
        rodSize = sizeof(TCP_ESTATS_FINE_RTT_ROD_v0);
        break;
    default:
        wprintf(L"\nCannot get type %d", (int)type);
        return;
        break;
    }

    if (rosSize != 0) {
        ros = (PUCHAR)malloc(rosSize);
        if (ros == NULL) {
            wprintf(L"\nOut of memory");
            return;
        } else
            memset(ros, 0, rosSize); // zero the buffer
    }
    if (rodSize != 0) {
        rod = (PUCHAR)malloc(rodSize);
        if (rod == NULL) {
            free(ros);
            wprintf(L"\nOut of memory");
            return;
        } else
            memset(rod, 0, rodSize); // zero the buffer
    }

    TCP_ESTATS_DATA_RW_v0 dataRw = {0};
    TCP_ESTATS_SND_CONG_RW_v0 sndCongRw = {0};
    TCP_ESTATS_PATH_RW_v0 pathRw = {0};
    TCP_ESTATS_SEND_BUFF_RW_v0 sndBuffRw = {0};
    TCP_ESTATS_REC_RW_v0 recRw = {0};
    TCP_ESTATS_OBS_REC_RW_v0 obsRecRw = {0};
    TCP_ESTATS_BANDWIDTH_RW_v0 bandwidthRw = {0};
    TCP_ESTATS_FINE_RTT_RW_v0 fineRttRw = {0};
    BOOLEAN RwEnableCollection{FALSE};

    switch (type) {
    case TcpConnectionEstatsData:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&dataRw, sizeof(TCP_ESTATS_DATA_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = dataRw.EnableCollection;
        break;
    case TcpConnectionEstatsSndCong:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&sndCongRw, sizeof(TCP_ESTATS_SND_CONG_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = sndCongRw.EnableCollection;
        break;
    case TcpConnectionEstatsPath:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&pathRw, sizeof(TCP_ESTATS_PATH_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = pathRw.EnableCollection;
        break;
    case TcpConnectionEstatsSendBuff:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&sndBuffRw, sizeof(TCP_ESTATS_SEND_BUFF_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = sndBuffRw.EnableCollection;
        break;
    case TcpConnectionEstatsRec:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&recRw, sizeof(TCP_ESTATS_REC_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = recRw.EnableCollection;
        break;
    case TcpConnectionEstatsObsRec:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&obsRecRw, sizeof(TCP_ESTATS_OBS_REC_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = obsRecRw.EnableCollection;
        break;
    case TcpConnectionEstatsBandwidth:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&bandwidthRw, sizeof(TCP_ESTATS_BANDWIDTH_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = bandwidthRw.EnableCollectionOutbound && bandwidthRw.EnableCollectionInbound;
        break;
    case TcpConnectionEstatsFineRtt:
        winStatus = GetConnectionEStats(row, type, (PUCHAR)&fineRttRw, sizeof(TCP_ESTATS_FINE_RTT_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = fineRttRw.EnableCollection;
        break;
    default:
        winStatus = GetConnectionEStats(row, type, NULL, v6, 0, ros, rosSize, rod, rodSize);
        break;
    }

    if (!RwEnableCollection) {
        if (v6)
            wprintf(L"\nGetPerTcp6ConnectionEStats %s failed. Rw.EnableCollection == FALSE", estatsTypeNames[type]);
        else
            wprintf(L"\nGetPerTcpConnectionEStats %s failed. Rw.EnableCollection == FALSE", estatsTypeNames[type]);
        return;
    }

    if (winStatus != NO_ERROR) {
        if (v6)
            wprintf(L"\nGetPerTcp6ConnectionEStats %s failed. status = %d",
                    estatsTypeNames[type],
                    winStatus);
        else
            wprintf(L"\nGetPerTcpConnectionEStats %s failed. status = %d",
                    estatsTypeNames[type],
                    winStatus);
    } else {
        switch (type) {
        case TcpConnectionEstatsSynOpts:
            synOptsRos = (PTCP_ESTATS_SYN_OPTS_ROS_v0)ros;
            wprintf(L"\nSyn Opts");
            wprintf(L"\nActive Open:    %s", synOptsRos->ActiveOpen ? L"Yes" : L"No");
            wprintf(L"\nMss Received:   %u", synOptsRos->MssRcvd);
            wprintf(L"\nMss Sent        %u", synOptsRos->MssSent);
            break;
        case TcpConnectionEstatsData:
            dataRod = (PTCP_ESTATS_DATA_ROD_v0)rod;
            wprintf(L"\n\nData");
            wprintf(L"\nBytes Out:   %lu", dataRod->DataBytesOut);
            wprintf(L"\nSegs Out:    %lu", dataRod->DataSegsOut);
            wprintf(L"\nBytes In:    %lu", dataRod->DataBytesIn);
            wprintf(L"\nSegs In:     %lu", dataRod->DataSegsIn);
            wprintf(L"\nSegs Out:    %u", dataRod->SegsOut);
            wprintf(L"\nSegs In:     %u", dataRod->SegsIn);
            wprintf(L"\nSoft Errors: %u", dataRod->SoftErrors);
            wprintf(L"\nSoft Error Reason: %u", dataRod->SoftErrorReason);
            wprintf(L"\nSnd Una:     %u", dataRod->SndUna);
            wprintf(L"\nSnd Nxt:     %u", dataRod->SndNxt);
            wprintf(L"\nSnd Max:     %u", dataRod->SndMax);
            wprintf(L"\nBytes Acked: %lu", dataRod->ThruBytesAcked);
            wprintf(L"\nRcv Nxt:     %u", dataRod->RcvNxt);
            wprintf(L"\nBytes Rcv:   %lu", dataRod->ThruBytesReceived);
            break;
        case TcpConnectionEstatsSndCong:
            sndCongRod = (PTCP_ESTATS_SND_CONG_ROD_v0)rod;
            sndCongRos = (PTCP_ESTATS_SND_CONG_ROS_v0)ros;
            wprintf(L"\n\nSnd Cong");
            wprintf(L"\nTrans Rwin:       %u", sndCongRod->SndLimTransRwin);
            wprintf(L"\nLim Time Rwin:    %u", sndCongRod->SndLimTimeRwin);
            wprintf(L"\nLim Bytes Rwin:   %u", sndCongRod->SndLimBytesRwin);
            wprintf(L"\nLim Trans Cwnd:   %u", sndCongRod->SndLimTransCwnd);
            wprintf(L"\nLim Time Cwnd:    %u", sndCongRod->SndLimTimeCwnd);
            wprintf(L"\nLim Bytes Cwnd:   %u", sndCongRod->SndLimBytesCwnd);
            wprintf(L"\nLim Trans Snd:    %u", sndCongRod->SndLimTransSnd);
            wprintf(L"\nLim Time Snd:     %u", sndCongRod->SndLimTimeSnd);
            wprintf(L"\nLim Bytes Snd:    %u", sndCongRod->SndLimBytesSnd);
            wprintf(L"\nSlow Start:       %u", sndCongRod->SlowStart);
            wprintf(L"\nCong Avoid:       %u", sndCongRod->CongAvoid);
            wprintf(L"\nOther Reductions: %u", sndCongRod->OtherReductions);
            wprintf(L"\nCur Cwnd:         %u", sndCongRod->CurCwnd);
            wprintf(L"\nMax Ss Cwnd:      %u", sndCongRod->MaxSsCwnd);
            wprintf(L"\nMax Ca Cwnd:      %u", sndCongRod->MaxCaCwnd);
            wprintf(L"\nCur Ss Thresh:    0x%x (%u)", sndCongRod->CurSsthresh, sndCongRod->CurSsthresh);
            wprintf(L"\nMax Ss Thresh:    0x%x (%u)", sndCongRod->MaxSsthresh, sndCongRod->MaxSsthresh);
            wprintf(L"\nMin Ss Thresh:    0x%x (%u)", sndCongRod->MinSsthresh, sndCongRod->MinSsthresh);
            wprintf(L"\nLim Cwnd:         0x%x (%u)", sndCongRos->LimCwnd, sndCongRos->LimCwnd);
            break;
        case TcpConnectionEstatsPath:
            pathRod = (PTCP_ESTATS_PATH_ROD_v0)rod;
            wprintf(L"\n\nPath");
            wprintf(L"\nFast Retran:         %u", pathRod->FastRetran);
            wprintf(L"\nTimeouts:            %u", pathRod->Timeouts);
            wprintf(L"\nSubsequent Timeouts: %u", pathRod->SubsequentTimeouts);
            wprintf(L"\nCur Timeout Count:   %u", pathRod->CurTimeoutCount);
            wprintf(L"\nAbrupt Timeouts:     %u", pathRod->AbruptTimeouts);
            wprintf(L"\nPkts Retrans:        %u", pathRod->PktsRetrans);
            wprintf(L"\nBytes Retrans:       %u", pathRod->BytesRetrans);
            wprintf(L"\nDup Acks In:         %u", pathRod->DupAcksIn);
            wprintf(L"\nSacksRcvd:           %u", pathRod->SacksRcvd);
            wprintf(L"\nSack Blocks Rcvd:    %u", pathRod->SackBlocksRcvd);
            wprintf(L"\nCong Signals:        %u", pathRod->CongSignals);
            wprintf(L"\nPre Cong Sum Cwnd:   %u", pathRod->PreCongSumCwnd);
            wprintf(L"\nPre Cong Sum Rtt:    %u", pathRod->PreCongSumRtt);
            wprintf(L"\nPost Cong Sum Rtt:   %u", pathRod->PostCongSumRtt);
            wprintf(L"\nPost Cong Count Rtt: %u", pathRod->PostCongCountRtt);
            wprintf(L"\nEcn Signals:         %u", pathRod->EcnSignals);
            wprintf(L"\nEce Rcvd:            %u", pathRod->EceRcvd);
            wprintf(L"\nSend Stall:          %u", pathRod->SendStall);
            wprintf(L"\nQuench Rcvd:         %u", pathRod->QuenchRcvd);
            wprintf(L"\nRetran Thresh:       %u", pathRod->RetranThresh);
            wprintf(L"\nSnd Dup Ack Episodes:  %u", pathRod->SndDupAckEpisodes);
            wprintf(L"\nSum Bytes Reordered: %u", pathRod->SumBytesReordered);
            wprintf(L"\nNon Recov Da:        %u", pathRod->NonRecovDa);
            wprintf(L"\nNon Recov Da Episodes: %u", pathRod->NonRecovDaEpisodes);
            wprintf(L"\nAck After Fr:        %u", pathRod->AckAfterFr);
            wprintf(L"\nDsack Dups:          %u", pathRod->DsackDups);
            wprintf(L"\nSample Rtt:          0x%x (%u)", pathRod->SampleRtt, pathRod->SampleRtt);
            wprintf(L"\nSmoothed Rtt:        %u", pathRod->SmoothedRtt);
            wprintf(L"\nRtt Var:             %u", pathRod->RttVar);
            wprintf(L"\nMax Rtt:             %u", pathRod->MaxRtt);
            wprintf(L"\nMin Rtt:             0x%x (%u)", pathRod->MinRtt, pathRod->MinRtt);
            wprintf(L"\nSum Rtt:             %u", pathRod->SumRtt);
            wprintf(L"\nCount Rtt:           %u", pathRod->CountRtt);
            wprintf(L"\nCur Rto:             %u", pathRod->CurRto);
            wprintf(L"\nMax Rto:             %u", pathRod->MaxRto);
            wprintf(L"\nMin Rto:             %u", pathRod->MinRto);
            wprintf(L"\nCur Mss:             %u", pathRod->CurMss);
            wprintf(L"\nMax Mss:             %u", pathRod->MaxMss);
            wprintf(L"\nMin Mss:             %u", pathRod->MinMss);
            wprintf(L"\nSpurious Rto:        %u", pathRod->SpuriousRtoDetections);
            break;
        case TcpConnectionEstatsSendBuff:
            sndBuffRod = (PTCP_ESTATS_SEND_BUFF_ROD_v0)rod;
            wprintf(L"\n\nSend Buff");
            wprintf(L"\nCur Retx Queue:   %u", sndBuffRod->CurRetxQueue);
            wprintf(L"\nMax Retx Queue:   %u", sndBuffRod->MaxRetxQueue);
            wprintf(L"\nCur App W Queue:  %u", sndBuffRod->CurAppWQueue);
            wprintf(L"\nMax App W Queue:  %u", sndBuffRod->MaxAppWQueue);
            break;
        case TcpConnectionEstatsRec:
            recRod = (PTCP_ESTATS_REC_ROD_v0)rod;
            wprintf(L"\n\nRec");
            wprintf(L"\nCur Rwin Sent:   0x%x (%u)", recRod->CurRwinSent, recRod->CurRwinSent);
            wprintf(L"\nMax Rwin Sent:   0x%x (%u)", recRod->MaxRwinSent, recRod->MaxRwinSent);
            wprintf(L"\nMin Rwin Sent:   0x%x (%u)", recRod->MinRwinSent, recRod->MinRwinSent);
            wprintf(L"\nLim Rwin:        0x%x (%u)", recRod->LimRwin, recRod->LimRwin);
            wprintf(L"\nDup Acks:        %u", recRod->DupAckEpisodes);
            wprintf(L"\nDup Acks Out:    %u", recRod->DupAcksOut);
            wprintf(L"\nCe Rcvd:         %u", recRod->CeRcvd);
            wprintf(L"\nEcn Send:        %u", recRod->EcnSent);
            wprintf(L"\nEcn Nonces Rcvd: %u", recRod->EcnNoncesRcvd);
            wprintf(L"\nCur Reasm Queue: %u", recRod->CurReasmQueue);
            wprintf(L"\nMax Reasm Queue: %u", recRod->MaxReasmQueue);
            wprintf(L"\nCur App R Queue: %u", recRod->CurAppRQueue);
            wprintf(L"\nMax App R Queue: %u", recRod->MaxAppRQueue);
            wprintf(L"\nWin Scale Sent:  0x%.2x", recRod->WinScaleSent);
            break;
        case TcpConnectionEstatsObsRec:
            obsRecRod = (PTCP_ESTATS_OBS_REC_ROD_v0)rod;
            wprintf(L"\n\nObs Rec");
            wprintf(L"\nCur Rwin Rcvd:   0x%x (%u)", obsRecRod->CurRwinRcvd, obsRecRod->CurRwinRcvd);
            wprintf(L"\nMax Rwin Rcvd:   0x%x (%u)", obsRecRod->MaxRwinRcvd, obsRecRod->MaxRwinRcvd);
            wprintf(L"\nMin Rwin Rcvd:   0x%x (%u)", obsRecRod->MinRwinRcvd, obsRecRod->MinRwinRcvd);
            wprintf(L"\nWin Scale Rcvd:  0x%x (%u)", obsRecRod->WinScaleRcvd, obsRecRod->WinScaleRcvd);
            break;
        case TcpConnectionEstatsBandwidth:
            bandwidthRod = (PTCP_ESTATS_BANDWIDTH_ROD_v0)rod;
            wprintf(L"\n\nBandwidth");
            wprintf(L"\nOutbound Bandwidth:   %lu", bandwidthRod->OutboundBandwidth);
            wprintf(L"\nInbound Bandwidth:    %lu", bandwidthRod->InboundBandwidth);
            wprintf(L"\nOutbound Instability: %lu", bandwidthRod->OutboundInstability);
            wprintf(L"\nInbound Instability:  %lu", bandwidthRod->InboundInstability);
            wprintf(L"\nOutbound Bandwidth Peaked: %s", 
                    bandwidthRod->OutboundBandwidthPeaked ? L"Yes" : L"No");
            wprintf(L"\nInbound Bandwidth Peaked:  %s",
                    bandwidthRod->InboundBandwidthPeaked ? L"Yes" : L"No");
            break;
        case TcpConnectionEstatsFineRtt:
            fineRttRod = (PTCP_ESTATS_FINE_RTT_ROD_v0)rod;
            wprintf(L"\n\nFine RTT");
            wprintf(L"\nRtt Var: %u", fineRttRod->RttVar);
            wprintf(L"\nMax Rtt: %u", fineRttRod->MaxRtt);
            wprintf(L"\nMin Rtt: 0x%x (%u) ", fineRttRod->MinRtt, fineRttRod->MinRtt);
            wprintf(L"\nSum Rtt: %u", fineRttRod->SumRtt);
            break;
        default:
            wprintf(L"\nCannot get type %d", type);
            break;
        }
    }

    free(ros);
    free(rod);
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
        exit(1);
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

        wprintf(L"Interface LUID NetLuidIndex[%d]:  %lu\n",
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

        wprintf(L"OnLink PrefixLength[%d]:\t\t %lu\n", i,
                pipTable->Table[i].OnLinkPrefixLength);

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

    //exit(0);
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI Icmp6Test()
/*
功能：演示Icmp6CreateFile+Icmp6SendEcho2+Icmp6ParseReplies的用法。

说明：实验暂时没成功，参数传递的还不对。

https://docs.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmp6createfile
https://docs.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmp6sendecho2
https://docs.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmp6parsereplies
*/
{
    HANDLE hIcmpFile = Icmp6CreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("\tUnable to open handle.\n");
        printf("Icmp6Createfile returned error: %ld\n", GetLastError());
        return;
    }

    struct sockaddr_in6 SourceAddress = {0};
    InetPtonA(AF_INET6, "240e:471:800:23d2:384d:f664:3c3:e9", &SourceAddress);

    struct sockaddr_in6 DestinationAddress = {0};
    InetPtonA(AF_INET6, "2001:4860:4860::6464", &DestinationAddress);

    LPVOID                   RequestData = NULL;
    WORD                     RequestSize = 0;

    /*
    A pointer to the IPv6 header options for the request, in the form of an IP_OPTION_INFORMATION structure.
    On a 64-bit platform, this parameter is in the form for an IP_OPTION_INFORMATION32 structure.

    This parameter may be NULL if no IP header options need to be specified.

    Note  On Windows Server 2003 and Windows XP, 
    the RequestOptions parameter is not optional and must not be NULL and only the Ttl and Flags members are used.
    */
    IP_OPTION_INFORMATION32  RequestOptions = {30, 0, 0, 0, NULL};

    PVOID                    ReplyBuffer = NULL;

    /*
    The size, in bytes, of the reply buffer pointed to by the ReplyBuffer parameter. 
    This buffer should be large enough to hold at least one ICMPV6_ECHO_REPLY structure plus RequestSize bytes of data. 
    This buffer should also be large enough to also hold 8 more bytes of data (the size of an ICMP error message) plus space for an IO_STATUS_BLOCK structure.
    */
    DWORD ReplySize = sizeof(ICMPV6_ECHO_REPLY) + RequestSize + 8 + sizeof(IO_STATUS_BLOCK);

    /*
    The time, in milliseconds, to wait for replies. 
    This parameter is only used if the Icmp6SendEcho2 function is called synchronously. 
    So this parameter is not used if either the ApcRoutine or Event parameter are not NULL.
    */
    DWORD                    Timeout = 0;

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
                               NULL, //(PIP_OPTION_INFORMATION)&RequestOptions,
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


    HeapFree(GetProcessHeap(), 0, ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
