#include "pch.h"
#include "tcp.h"


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
    // Make an initial call to GetTcpTable to get the necessary size into the dwSize variable
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

            PrintOffloadStateOfTcpConnection(pTcpTable->table[i].dwOffloadState);
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
The following example retrieves the TCP statistics for the local computer and
prints some values from the returned data.

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

The GetExtendedTcpTable function called with the ulAf parameter set to AF_INET6 and
the TableClass set to TCP_TABLE_BASIC_LISTENER, TCP_TABLE_BASIC_CONNECTIONS,
or TCP_TABLE_BASIC_ALL is not supported and returns ERROR_NOT_SUPPORTED.

调用示例：
EnumExtendedTcpTable(AF_INET, TCP_TABLE_OWNER_PID_ALL);
...
等等15种组合。

不支持一下3个调用：
EnumExtendedTcpTable(AF_INET6, TCP_TABLE_BASIC_LISTENER);
EnumExtendedTcpTable(AF_INET6, TCP_TABLE_BASIC_CONNECTIONS);
EnumExtendedTcpTable(AF_INET6, TCP_TABLE_BASIC_ALL);

其实这可以设置一个回调函数。

注释：修改自MSDN的GetTcpTable2的例子。

made in 2016.11.30

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getextendedtcptable
*/
{
    PVOID pTcpTable = MALLOC(sizeof(MIB_TCPTABLE_OWNER_PID));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    ULONG ulSize = sizeof(MIB_TCPTABLE_OWNER_PID);
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


//////////////////////////////////////////////////////////////////////////////////////////////////
