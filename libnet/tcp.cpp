#include "pch.h"
#include "tcp.h"


#pragma warning(disable : 4477)
#pragma warning(disable : 6101)
#pragma warning(disable : 6328)


void GetOwnerModuleFromTcp6EntryEx(_In_ PMIB_TCP6ROW_OWNER_MODULE pTcpEntry);
void GetOwnerModuleFromTcp4EntryEx(_In_ PMIB_TCPROW_OWNER_MODULE pTcpEntry);


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI EnumTcpTable()
/*
The GetTcpTable function retrieves the IPv4 TCP connection table.

The following example retrieves the TCP connection table for IPv4 and prints the state of each connection.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcptable
*/
{
    // Declare and initialize variables
    DWORD dwRetVal = 0;
    char szLocalAddr[128]{};
    char szRemoteAddr[128]{};
    in_addr IpAddr{};

    PMIB_TCPTABLE pTcpTable = (MIB_TCPTABLE *)MALLOC(sizeof(MIB_TCPTABLE));
    if (pTcpTable == nullptr) {
        printf("Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_TCPTABLE);
    // Make an initial call to GetTcpTable to get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCPTABLE *)MALLOC(dwSize);
        if (pTcpTable == nullptr) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcpTable to get the actual data we require
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        printf("\tNumber of entries: %u\n", pTcpTable->dwNumEntries);
        for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
            strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));

            printf("\n\tTCP[%d] State: %lu - ", i, pTcpTable->table[i].dwState);
            PrintTcpConnectionState(pTcpTable->table[i].dwState);
            printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tTCP[%d] Local Port: %u \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));
            printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);
            printf("\tTCP[%d] Remote Port: %u\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));
        }
    } else {
        printf("\tGetTcpTable failed with %lu\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != nullptr) {
        FREE(pTcpTable);
        pTcpTable = nullptr;
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumTcpTable2()
/*
The following example retrieves the TCP connection table for IPv4 and prints the state of each connection.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcptable2
*/
{
    PMIB_TCPTABLE2 pTcpTable = (MIB_TCPTABLE2 *)MALLOC(sizeof(MIB_TCPTABLE2));
    if (pTcpTable == nullptr) {
        printf("Error allocating memory\n");
        return 1;
    }

    // Make an initial call to GetTcpTable2 to get the necessary size into the ulSize variable
    ULONG ulSize = sizeof(MIB_TCPTABLE);
    DWORD dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCPTABLE2 *)MALLOC(ulSize);
        if (pTcpTable == nullptr) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcpTable2 to get the actual data we require
    if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) == NO_ERROR) {
        printf("\tNumber of entries: %d\n", (int)pTcpTable->dwNumEntries);
        for (int i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            char szLocalAddr[128]{};
            char szRemoteAddr[128]{};
            in_addr IpAddr{};

            printf("\n\tTCP[%d] State: %lu - ", i, pTcpTable->table[i].dwState);
            PrintTcpConnectionState(pTcpTable->table[i].dwState);

            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
            printf("\tTCP[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tTCP[%d] Local Port: %u \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
            strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
            printf("\tTCP[%d] Remote Addr: %s\n", i, szRemoteAddr);
            printf("\tTCP[%d] Remote Port: %u\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

            printf("\tTCP[%d] Owning PID: %lu\n", i, pTcpTable->table[i].dwOwningPid);

            char szModName[MAX_PATH] = {0};
            DWORD Size = _countof(szModName);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pTcpTable->table[i].dwOwningPid);
            if (hProcess) {
                if (4 == pTcpTable->table[i].dwOwningPid) {
                    lstrcpyA(szModName, "System");
                } else {
                    BOOL B = QueryFullProcessImageNameA(hProcess, 0, szModName, &Size);
                    if (!B) {
                        printf("警告：pid:%d, LastError:%d\n", pTcpTable->table[i].dwOwningPid, GetLastError());
                    } else {
                        printf("\tTCP[%d] Owning FullProcessImageName: %s\n", i, szModName);
                    }
                }
                CloseHandle(hProcess);
            } else {
                printf("警告：pid:%d, LastError:%d\n", pTcpTable->table[i].dwOwningPid, GetLastError());
            }

            // MIB_TCPROW_OWNER_MODULE Tcp4Entry{};
            // Tcp4Entry.dwState = pTcpTable->table[i].dwState;
            // Tcp4Entry.dwLocalAddr = pTcpTable->table[i].dwLocalAddr;
            // Tcp4Entry.dwLocalPort = pTcpTable->table[i].dwLocalPort;
            // Tcp4Entry.dwRemoteAddr = pTcpTable->table[i].dwRemoteAddr;
            // Tcp4Entry.dwRemotePort = pTcpTable->table[i].dwRemotePort;
            // Tcp4Entry.dwOwningPid = pTcpTable->table[i].dwOwningPid;//估计这个必不可少。
            ////liCreateTimestamp和OwningModuleInfo都不赋值了。
            //// pModulePath == C:\Windows\System32\svchost.exe时， pModuleName == svchost.exe，这是不正确的。
            //
            ////GetOwnerModuleFromTcp4EntryEx((PMIB_TCPROW_OWNER_MODULE)&pTcpTable->table[i]);//不行。
            // GetOwnerModuleFromTcp4EntryEx(&Tcp4Entry);

            printf("\tTCP[%d] Offload State: %d - ", i, pTcpTable->table[i].dwOffloadState);
            PrintOffloadStateOfTcpConnection(pTcpTable->table[i].dwOffloadState);
        }
    } else {
        printf("\tGetTcpTable2 failed with %lu\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != nullptr) {
        FREE(pTcpTable);
        pTcpTable = nullptr;
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumTcp6Table()
/*
The following example retrieves the TCP connection table for IPv6 and prints the state of each connection.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcp6table
*/
{
    // Declare and initialize variables
    DWORD dwRetVal = 0;
    wchar_t ipstringbuffer[46]{};

    PMIB_TCP6TABLE pTcpTable = (MIB_TCP6TABLE *)MALLOC(sizeof(MIB_TCP6TABLE));
    if (pTcpTable == nullptr) {
        wprintf(L"Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_TCP6TABLE);
    // Make an initial call to GetTcp6Table to get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcp6Table(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCP6TABLE *)MALLOC(dwSize);
        if (pTcpTable == nullptr) {
            wprintf(L"Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcp6Table to get the actual data we require
    if ((dwRetVal = GetTcp6Table(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        wprintf(L"\tNumber of entries: %u\n", pTcpTable->dwNumEntries);
        for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
            wprintf(L"\n\tTCP[%d] State: %ld - ", i, pTcpTable->table[i].State);
            PrintTcpConnectionState(pTcpTable->table[i].State);

            if (InetNtop(AF_INET6, &pTcpTable->table[i].LocalAddr, ipstringbuffer, 46) == nullptr)
                wprintf(L"  InetNtop function failed for local IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Local Addr: %s\n", i, ipstringbuffer);

            wprintf(L"\tTCP[%d] Local Scope ID: %u \n", i, ntohl(pTcpTable->table[i].dwLocalScopeId));
            wprintf(L"\tTCP[%d] Local Port: %u \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            if (InetNtop(AF_INET6, &pTcpTable->table[i].RemoteAddr, ipstringbuffer, 46) == nullptr)
                wprintf(L"  InetNtop function failed for remote IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Remote Addr: %s\n", i, ipstringbuffer);

            wprintf(L"\tTCP[%d] Remote Scope ID: %u \n", i, ntohl(pTcpTable->table[i].dwRemoteScopeId));
            wprintf(L"\tTCP[%d] Remote Port: %u\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));
        }
    } else {
        wprintf(L"\tGetTcp6Table failed with %u\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != nullptr) {
        FREE(pTcpTable);
        pTcpTable = nullptr;
    }

    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI EnumTcp6Table2()
/*
这个是自己写的，MSDN没有例子。

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-gettcp6table2
*/
{
    PMIB_TCP6TABLE2 pTcpTable = (MIB_TCP6TABLE2 *)MALLOC(sizeof(MIB_TCP6TABLE2));
    if (pTcpTable == nullptr) {
        wprintf(L"Error allocating memory\n");
        return 1;
    }

    DWORD dwSize = sizeof(MIB_TCP6TABLE2);
    // Make an initial call to GetTcp6Table to get the necessary size into the dwSize variable
    DWORD dwRetVal = GetTcp6Table2(pTcpTable, &dwSize, TRUE);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = (MIB_TCP6TABLE2 *)MALLOC(dwSize);
        if (pTcpTable == nullptr) {
            wprintf(L"Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetTcp6Table to get the actual data we require
    if ((dwRetVal = GetTcp6Table2(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        int n = 0;

        for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
            if (MIB_TCP_STATE_LISTEN != pTcpTable->table[i].State) {
                continue;
            }

            wchar_t ipstringbuffer[46];
            if (InetNtop(AF_INET6, &pTcpTable->table[i].LocalAddr, ipstringbuffer, 46) == nullptr)
                wprintf(L"  InetNtop function failed for local IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Local Addr: %s\n", i, ipstringbuffer);

            // wprintf(L"\tTCP[%d] Local Scope ID: %d \n", i, ntohl(pUdp6Table->table[i].dwLocalScopeId));
            wprintf(L"\tTCP[%d] Local Port: %u \n", i, ntohs((u_short)pTcpTable->table[i].dwLocalPort));

            if (InetNtop(AF_INET6, &pTcpTable->table[i].RemoteAddr, ipstringbuffer, 46) == nullptr)
                wprintf(L"  InetNtop function failed for remote IPv6 address\n");
            else
                wprintf(L"\tTCP[%d] Remote Addr: %s\n", i, ipstringbuffer);

            // wprintf(L"\tTCP[%d] Remote Scope ID: %d \n", i, ntohl(pUdp6Table->table[i].dwRemoteScopeId));
            wprintf(L"\tTCP[%d] Remote Port: %u\n", i, ntohs((u_short)pTcpTable->table[i].dwRemotePort));

            wprintf(L"\tTCP[%d] OwningPid: %u\n", i, ntohs((u_short)pTcpTable->table[i].dwOwningPid));

            // char szModName[MAX_PATH] = {0};
            // DWORD Size = _countof(szModName);
            // HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pTcpTable->table[i].dwOwningPid);
            //_ASSERTE(hProcess);
            // BOOL B = QueryFullProcessImageNameA(hProcess, 0, szModName, &Size);
            //_ASSERTE(B);
            // printf("\tTCP[%d] Owning ProcessImageName: %s\n", i, szModName);

            // MIB_TCP6ROW2是MIB_TCP6ROW_OWNER_MODULE的子集，少了后面的两个成员。
            //经测试，这个强转的代码可以正确运行。
            //尽管可以运行，但是获取svchost的不正确。
            // GetOwnerModuleFromTcp6EntryEx((PMIB_TCP6ROW_OWNER_MODULE)&pTcpTable->table[i]);

            printf("\n");

            n++;
        }

        printf("IPV6的侦听总数:%d\n", n);
    } else {
        wprintf(L"\tGetTcp6Table failed with %u\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != nullptr) {
        FREE(pTcpTable);
        pTcpTable = nullptr;
    }

    return 0;
}


EXTERN_C
DLLEXPORT
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
    DWORD dwRetVal = 0;

    PMIB_TCPSTATS pTCPStats = (MIB_TCPSTATS *)MALLOC(sizeof(MIB_TCPSTATS));
    if (pTCPStats == nullptr) {
        printf("Error allocating memory\n");
        return 1;
    }

    if ((dwRetVal = GetTcpStatistics(pTCPStats)) == NO_ERROR) {
        printf("\tActive Opens: %lu\n", pTCPStats->dwActiveOpens);
        printf("\tPassive Opens: %lu\n", pTCPStats->dwPassiveOpens);
        printf("\tSegments Recv: %lu\n", pTCPStats->dwInSegs);
        printf("\tSegments Xmit: %lu\n", pTCPStats->dwOutSegs);
        printf("\tTotal # Conxs: %lu\n", pTCPStats->dwNumConns);
    } else {
        printf("GetTcpStatistics failed with error: %lu\n", dwRetVal);
        DisplayError(dwRetVal);
    }

    if (pTCPStats)
        FREE(pTCPStats);

    return 0;
}


void DumpBasicExtendedTcp4Table(_In_ PMIB_TCPTABLE pTcpTable)
{
    char szLocalAddr[128]{};
    char szRemoteAddr[128]{};
    in_addr IpAddr{};

    printf("\tNumber of entries: %lu\n", pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%lu] State: %lu - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        IpAddr.S_un.S_addr = pTcpTable->table[i].dwLocalAddr;
        strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%lu] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%lu] Local Port: %u \n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwLocalPort)));

        IpAddr.S_un.S_addr = pTcpTable->table[i].dwRemoteAddr;
        strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%lu] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%lu] Remote Port: %u\n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwRemotePort)));
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

    //_ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    if (ERROR_NOT_FOUND == ret) {
        return;
    }

    auto pBuffer = reinterpret_cast<PTCPIP_OWNER_MODULE_BASIC_INFO>(MALLOC(Size));
    _ASSERTE(pBuffer);

    ret = GetOwnerModuleFromTcpEntry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, pBuffer, &Size);
    _ASSERTE(NO_ERROR == ret);

    printf("\tModuleName: %ls\n", pBuffer->pModuleName);
    printf("\tModulePath: %ls\n", pBuffer->pModulePath);

    FREE(pBuffer);
}


void DumpModuleExtendedTcp4Table(_In_ PMIB_TCPTABLE_OWNER_MODULE pTcpTable)
{
    char szLocalAddr[128]{};
    char szRemoteAddr[128]{};
    in_addr IpAddr{};

    printf("\tNumber of entries: %lu\n", pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%lu] State: %lu - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        IpAddr.S_un.S_addr = pTcpTable->table[i].dwLocalAddr;
        strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%lu] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%lu] Local Port: %u \n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwLocalPort)));

        IpAddr.S_un.S_addr = pTcpTable->table[i].dwRemoteAddr;
        strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%lu] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%lu] Remote Port: %u\n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwRemotePort)));

        printf("\tTCP[%lu] OwningPid: %lu\n", i, pTcpTable->table[i].dwOwningPid);

        WCHAR TimeString[MAX_PATH] = {0};
        GetTimeString(pTcpTable->table[i].liCreateTimestamp, TimeString);
        printf("\tTCP[%lu] CreateTimestamp: %ls\n", i, TimeString);

        // OwningModuleInfo里的TCPIP_OWNING_MODULE_SIZE个数，全都是0.

        GetOwnerModuleFromTcp4EntryEx(&pTcpTable->table[i]);
    }
}


void DumpPidExtendedTcp4Table(_In_ PMIB_TCPTABLE_OWNER_PID pTcpTable)
{
    char szLocalAddr[128]{};
    char szRemoteAddr[128]{};
    in_addr IpAddr{};

    printf("\tNumber of entries: %lu\n", pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%lu] State: %lu - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        IpAddr.S_un.S_addr = pTcpTable->table[i].dwLocalAddr;
        strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%lu] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%lu] Local Port: %u \n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwLocalPort)));

        IpAddr.S_un.S_addr = pTcpTable->table[i].dwRemoteAddr;
        strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
        printf("\tTCP[%lu] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%lu] Remote Port: %u\n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwRemotePort)));

        printf("\tTCP[%lu] Owning PID: %lu\n", i, pTcpTable->table[i].dwOwningPid);
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

    if (ERROR_NOT_FOUND == ret) {
        return;
    }

    _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

    auto pBuffer = reinterpret_cast<PTCPIP_OWNER_MODULE_BASIC_INFO>(MALLOC(Size));
    _ASSERTE(pBuffer);

    ret = GetOwnerModuleFromTcp6Entry(pTcpEntry, TCPIP_OWNER_MODULE_INFO_BASIC, pBuffer, &Size);
    _ASSERTE(NO_ERROR == ret);

    printf("\tModuleName: %ls\n", pBuffer->pModuleName);
    printf("\tModulePath: %ls\n", pBuffer->pModulePath);

    FREE(pBuffer);
}


void DumpModuleExtendedTcp6Table(_In_ PMIB_TCP6TABLE_OWNER_MODULE pTcpTable)
{
    char szLocalAddr[128]{};
    char szRemoteAddr[128]{};

    printf("\tNumber of entries: %lu\n", pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%lu] State: %lu - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        InetNtopA(AF_INET6, pTcpTable->table[i].ucLocalAddr, szLocalAddr, _ARRAYSIZE(szLocalAddr));
        printf("\tTCP[%lu] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%lu] LocalScopeId: %lu \n", i, pTcpTable->table[i].dwLocalScopeId);

        printf("\tTCP[%lu] Local Port: %u \n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwLocalPort)));

        InetNtopA(AF_INET6, pTcpTable->table[i].ucRemoteAddr, szRemoteAddr, _ARRAYSIZE(szRemoteAddr));
        printf("\tTCP[%lu] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%lu] RemoteScopeId: %lu \n", i, pTcpTable->table[i].dwRemoteScopeId);

        printf("\tTCP[%lu] Remote Port: %u\n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwRemotePort)));

        printf("\tTCP[%lu] OwningPid: %lu\n", i, pTcpTable->table[i].dwOwningPid);

        WCHAR TimeString[MAX_PATH] = {0};
        GetTimeString(pTcpTable->table[i].liCreateTimestamp, TimeString);
        printf("\tTCP[%lu] CreateTimestamp: %ls\n", i, TimeString);

        for (int j = 0; j < TCPIP_OWNING_MODULE_SIZE; j++) {
            printf("\tTCP[%lu] ModuleInfo: %llX\n", i, pTcpTable->table[i].OwningModuleInfo[j]);
        }

        GetOwnerModuleFromTcp6EntryEx(&pTcpTable->table[i]);
    }
}


void DumpPidExtendedTcp6Table(_In_ PMIB_TCP6TABLE_OWNER_PID pTcpTable)
{
    char szLocalAddr[128]{};
    char szRemoteAddr[128]{};

    printf("\tNumber of entries: %lu\n", pTcpTable->dwNumEntries);
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        printf("\n\tTCP[%lu] State: %lu - ", i, pTcpTable->table[i].dwState);
        PrintTcpConnectionState(pTcpTable->table[i].dwState);

        InetNtopA(AF_INET6, pTcpTable->table[i].ucLocalAddr, szLocalAddr, _ARRAYSIZE(szLocalAddr));
        printf("\tTCP[%lu] Local Addr: %s\n", i, szLocalAddr);

        printf("\tTCP[%lu] LocalScopeId: %lu \n", i, pTcpTable->table[i].dwLocalScopeId);

        printf("\tTCP[%lu] Local Port: %u \n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwLocalPort)));

        InetNtopA(AF_INET6, pTcpTable->table[i].ucRemoteAddr, szRemoteAddr, _ARRAYSIZE(szRemoteAddr));
        printf("\tTCP[%lu] Remote Addr: %s\n", i, szRemoteAddr);

        printf("\tTCP[%lu] RemoteScopeId: %lu \n", i, pTcpTable->table[i].dwRemoteScopeId);

        printf("\tTCP[%lu] Remote Port: %u\n", i, ntohs(static_cast<u_short>(pTcpTable->table[i].dwRemotePort)));

        printf("\tTCP[%lu] OwningPid: %lu\n", i, pTcpTable->table[i].dwOwningPid);
    }
}


void DumpExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass, _In_ PVOID pTcpTable)
/*
功能：打印各种情况下的信息。

When the ulAf parameter is set to AF_INET,
the following table indicates the TCP table type to retrieve in the structure pointed to by the pTcpTable parameter
for each possible TableClass value. TableClass value	pTcpTable structure TCP_TABLE_BASIC_ALL	MIB_TCPTABLE
TCP_TABLE_BASIC_CONNECTIONS	MIB_TCPTABLE
TCP_TABLE_BASIC_LISTENER	MIB_TCPTABLE
TCP_TABLE_OWNER_MODULE_ALL	MIB_TCPTABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_CONNECTIONS	MIB_TCPTABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_LISTENER	MIB_TCPTABLE_OWNER_MODULE
TCP_TABLE_OWNER_PID_ALL	MIB_TCPTABLE_OWNER_PID
TCP_TABLE_OWNER_PID_CONNECTIONS	MIB_TCPTABLE_OWNER_PID
TCP_TABLE_OWNER_PID_LISTENER	MIB_TCPTABLE_OWNER_PID

When the ulAf parameter is set to AF_INET6,
the following table indicates the TCP table type to retrieve in the structure pointed to by the pTcpTable parameter
for each possible TableClass value. TableClass value	pTcpTable structure TCP_TABLE_OWNER_MODULE_ALL
MIB_TCP6TABLE_OWNER_MODULE TCP_TABLE_OWNER_MODULE_CONNECTIONS	MIB_TCP6TABLE_OWNER_MODULE
TCP_TABLE_OWNER_MODULE_LISTENER	MIB_TCP6TABLE_OWNER_MODULE
TCP_TABLE_OWNER_PID_ALL	MIB_TCP6TABLE_OWNER_PID
TCP_TABLE_OWNER_PID_CONNECTIONS	MIB_TCP6TABLE_OWNER_PID
TCP_TABLE_OWNER_PID_LISTENER	MIB_TCP6TABLE_OWNER_PID

The GetExtendedTcpTable function called with the ulAf parameter set to AF_INET6 and the TableClass set to
TCP_TABLE_BASIC_LISTENER, TCP_TABLE_BASIC_CONNECTIONS, or TCP_TABLE_BASIC_ALL is not supported and returns
ERROR_NOT_SUPPORTED.

https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getextendedtcptable
*/
{
    switch (ulAf) {
    case AF_INET: {
        switch (TableClass) {
        case TCP_TABLE_BASIC_ALL:
        case TCP_TABLE_BASIC_CONNECTIONS:
        case TCP_TABLE_BASIC_LISTENER:
            DumpBasicExtendedTcp4Table(reinterpret_cast<PMIB_TCPTABLE>(pTcpTable));
            break;
        case TCP_TABLE_OWNER_MODULE_ALL:
        case TCP_TABLE_OWNER_MODULE_CONNECTIONS:
        case TCP_TABLE_OWNER_MODULE_LISTENER:
            DumpModuleExtendedTcp4Table(reinterpret_cast<PMIB_TCPTABLE_OWNER_MODULE>(pTcpTable));
            break;
        case TCP_TABLE_OWNER_PID_ALL:
        case TCP_TABLE_OWNER_PID_CONNECTIONS:
        case TCP_TABLE_OWNER_PID_LISTENER:
            DumpPidExtendedTcp4Table(reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(pTcpTable));
            break;
        default:
            _ASSERTE(FALSE);
            break;
        }

        break;
    }
    case AF_INET6: {
        switch (TableClass) {
        case TCP_TABLE_BASIC_LISTENER:
        case TCP_TABLE_BASIC_CONNECTIONS:
        case TCP_TABLE_BASIC_ALL:
            _ASSERTE(FALSE);
            break;
        case TCP_TABLE_OWNER_MODULE_ALL:
        case TCP_TABLE_OWNER_MODULE_CONNECTIONS:
        case TCP_TABLE_OWNER_MODULE_LISTENER:
            DumpModuleExtendedTcp6Table(reinterpret_cast<PMIB_TCP6TABLE_OWNER_MODULE>(pTcpTable));
            break;
        case TCP_TABLE_OWNER_PID_ALL:
        case TCP_TABLE_OWNER_PID_CONNECTIONS:
        case TCP_TABLE_OWNER_PID_LISTENER:
            DumpPidExtendedTcp6Table(reinterpret_cast<PMIB_TCP6TABLE_OWNER_PID>(pTcpTable));
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
DLLEXPORT
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass)
/*
功能：获取各类的ExtendedTcpTable信息。

参数：
[in] ulAf
The version of IP used by the TCP endpoints.可选的取值有：AF_INET和AF_INET6。
[in] TableClass
This parameter can be one of the values from the TCP_TABLE_CLASS enumeration.

The GetExtendedTcpTable function called with the ulAf parameter set to AF_INET6 and the TableClass set to TCP_TABLE_BASIC_LISTENER, TCP_TABLE_BASIC_CONNECTIONS,
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
    if (pTcpTable == nullptr) {
        printf("Error allocating memory\n");
        return 1;
    }

    ULONG ulSize = sizeof(MIB_TCPTABLE_OWNER_PID);
    DWORD dwRetVal = GetExtendedTcpTable(pTcpTable, &ulSize, TRUE, ulAf, TableClass, 0);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pTcpTable);
        pTcpTable = MALLOC(ulSize);
        if (pTcpTable == nullptr) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    dwRetVal = GetExtendedTcpTable(pTcpTable, &ulSize, TRUE, ulAf, TableClass, 0);
    if (dwRetVal == NO_ERROR) {
        DumpExtendedTcpTable(ulAf, TableClass, pTcpTable);
    } else {
        printf("\tGetExtendedTcpTable failed with %lu\n", dwRetVal);
        FREE(pTcpTable);
        return 1;
    }

    if (pTcpTable != nullptr) {
        FREE(pTcpTable);
        pTcpTable = nullptr;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


// Need to run as administrator

// An array of name for the TCP_ESTATS_TYPE enum values
// The names values must match the enum values
const wchar_t * estatsTypeNames[] = {L"TcpConnectionEstatsSynOpts",
                                     L"TcpConnectionEstatsData",
                                     L"TcpConnectionEstatsSndCong",
                                     L"TcpConnectionEstatsPath",
                                     L"TcpConnectionEstatsSendBuff",
                                     L"TcpConnectionEstatsRec",
                                     L"TcpConnectionEstatsObsRec",
                                     L"TcpConnectionEstatsBandwidth",
                                     L"TcpConnectionEstatsFineRtt",
                                     L"TcpConnectionEstatsMaximum"};

// Function prototypes


DWORD RunEstatsTest(bool v6);// Run tests for IPv4 or IPv4 TCP extended stats
DWORD GetTcpRow(u_short localPort, u_short remotePort, MIB_TCP_STATE state, __out PMIB_TCPROW row);// Get an IPv4 TCP row entry
DWORD GetTcp6Row(u_short localPort, u_short remotePort, MIB_TCP_STATE state, __out PMIB_TCP6ROW row);// Get an IPv6 TCP row entry
void ToggleEstat(PVOID row, TCP_ESTATS_TYPE type, bool enable, bool v6);// Enable or disable the supplied Estat type on a TCP connection
void ToggleAllEstats(void * row, bool enable, bool v6);// Toggle all Estats for a TCP connection
void GetAndOutputEstats(void * row, TCP_ESTATS_TYPE type, bool v6);// Dump the supplied Estate type data on the given TCP connection row
void GetAllEstats(void * row, bool v6);

// Creates a TCP server and client socket on the loopback address.
// Binds the server socket to a port.
// Establishes a client TCP connection to the server
int CreateTcpConnection(bool v6, SOCKET * serviceSocket, SOCKET * clientSocket, SOCKET * acceptSocket, u_short * serverPort, u_short * clientPort);


EXTERN_C
DLLEXPORT
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
    SOCKET serviceSocket{}, clientSocket{}, acceptSocket{};
    serviceSocket = clientSocket = acceptSocket = INVALID_SOCKET;
    MIB_TCPROW server4ConnectRow{}, client4ConnectRow{};
    MIB_TCP6ROW server6ConnectRow{}, client6ConnectRow{};
    void *serverConnectRow{}, *clientConnectRow = nullptr;
    bool bWSAStartup = false;
    UINT winStatus{};
    int sockStatus{};
    u_short serverPort{}, clientPort{};
    WSADATA wsaData{};

    char * buff = reinterpret_cast<char *>(malloc(1000));
    if (buff == nullptr) {
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
    
    winStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);// Initialize Winsock.
    if (winStatus != ERROR_SUCCESS) {
        wprintf(L"\nFailed to open winsock. Error %u", winStatus);
        goto bail;
    }

    bWSAStartup = true;

    // Create TCP connection on which Estats information will be collected.
    // Obtain port numbers of created connections.
    winStatus = CreateTcpConnection(v6, &serviceSocket, &clientSocket, &acceptSocket, &serverPort, &clientPort);
    if (winStatus != ERROR_SUCCESS) {
        wprintf(L"\nFailed to create TCP connection. Error %u", winStatus);
        goto bail;
    }

    // Obtain MIB_TCPROW corresponding to the TCP connection.
    winStatus = v6 ? GetTcp6Row(serverPort, clientPort, MIB_TCP_STATE_ESTAB, reinterpret_cast<PMIB_TCP6ROW>(serverConnectRow)) :
             GetTcpRow(serverPort, clientPort, MIB_TCP_STATE_ESTAB, reinterpret_cast<PMIB_TCPROW>(serverConnectRow));
    if (winStatus != ERROR_SUCCESS) {
        wprintf(L"\nGetTcpRow failed on the server established connection with %u", winStatus);
        goto bail;
    }

    winStatus = v6 ? GetTcp6Row(clientPort, serverPort, MIB_TCP_STATE_ESTAB, reinterpret_cast<PMIB_TCP6ROW>(clientConnectRow)) :
             GetTcpRow(clientPort, serverPort, MIB_TCP_STATE_ESTAB, reinterpret_cast<PMIB_TCPROW>(clientConnectRow));
    if (winStatus != ERROR_SUCCESS) {
        wprintf(L"\nGetTcpRow failed on the client established connection with %u", winStatus);
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
    sockStatus = send(clientSocket, buff, (1000 * sizeof(char)), 0);
    if (sockStatus == SOCKET_ERROR) {
        wprintf(L"\nFailed to send from client to server %d", WSAGetLastError());
    } else {
        sockStatus = recv(acceptSocket, buff, (1000 * sizeof(char)), 0);
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
    if (buff != nullptr)
        free(buff);
    if (bWSAStartup)
        WSACleanup();
    return ERROR_SUCCESS;
}


int CreateTcpConnection(bool v6, SOCKET * serviceSocket, SOCKET * clientSocket, SOCKET * acceptSocket, u_short * serverPort, u_short * clientPort)
{
    INT status{};
    ADDRINFOW hints{}, *localhost = nullptr;
    const wchar_t * loopback{};
    loopback = v6 ? L"::1" : L"127.0.0.1";
    int aiFamily = v6 ? AF_INET6 : AF_INET;
    int nameLen = sizeof(SOCKADDR_STORAGE);
    SOCKADDR_STORAGE serverSockName{}, clientSockName{};

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

    status = bind(*serviceSocket, localhost->ai_addr, static_cast<int>(localhost->ai_addrlen));
    if (status == SOCKET_ERROR) {
        wprintf(L"\nFailed to bind server socket to loopback. Error %d", WSAGetLastError());
        goto bail;
    }

    if (localhost != nullptr) {
        FreeAddrInfoW(localhost);
        localhost = nullptr;
    }

    status = getsockname(*serviceSocket, reinterpret_cast<sockaddr *>(&serverSockName), &nameLen);
    if (status == SOCKET_ERROR) {
        wprintf(L"\ngetsockname failed %d", WSAGetLastError());
        goto bail;
    }
    if (v6) {
        *serverPort = (reinterpret_cast<sockaddr_in6 *>(&serverSockName))->sin6_port;
    } else {
        *serverPort = (reinterpret_cast<sockaddr_in *>(&serverSockName))->sin_port;
    }

    status = listen(*serviceSocket, SOMAXCONN);
    if (status == SOCKET_ERROR) {
        wprintf(L"\nFailed to listen on server socket. Error %d", WSAGetLastError());
        goto bail;
    }

    status = connect(*clientSocket, reinterpret_cast<sockaddr *>(&serverSockName), sizeof(SOCKADDR_STORAGE));
    if (status == SOCKET_ERROR) {
        wprintf(L"\nCould not connect client and server sockets %d", WSAGetLastError());
        goto bail;
    }

    status = getsockname(*clientSocket, reinterpret_cast<sockaddr *>(&clientSockName), &nameLen);
    if (status == SOCKET_ERROR) {
        wprintf(L"\ngetsockname failed %d", WSAGetLastError());
        goto bail;
    }
    if (v6) {
        *clientPort = (reinterpret_cast<sockaddr_in6 *>(&clientSockName))->sin6_port;
    } else {
        *clientPort = (reinterpret_cast<sockaddr_in *>(&clientSockName))->sin_port;
    }

    *acceptSocket = accept(*serviceSocket, nullptr, nullptr);
    if (*acceptSocket == INVALID_SOCKET) {
        wprintf(L"\nFailed to accept socket connection %d", WSAGetLastError());
        goto bail;
    }

    return ERROR_SUCCESS;

bail:
    if (localhost != nullptr)
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
    PMIB_TCPTABLE tcpTable = nullptr;
    PMIB_TCPROW tcpRowIt = nullptr;

    DWORD status{}, size = 0, i{};
    bool connectionFound = FALSE;

    status = GetTcpTable(tcpTable, &size, TRUE);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        return status;
    }

    tcpTable = reinterpret_cast<PMIB_TCPTABLE>(malloc(size));
    if (tcpTable == nullptr) {
        return ERROR_OUTOFMEMORY;
    }

    status = GetTcpTable(tcpTable, &size, TRUE);
    if (status != ERROR_SUCCESS) {
        free(tcpTable);
        return status;
    }

    for (i = 0; i < tcpTable->dwNumEntries; i++) {
        tcpRowIt = &tcpTable->table[i];
        if (tcpRowIt->dwLocalPort == static_cast<DWORD>(localPort) && tcpRowIt->dwRemotePort == static_cast<DWORD>(remotePort) && tcpRowIt->State == state) {
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
    PMIB_TCP6TABLE tcp6Table = nullptr;
    PMIB_TCP6ROW tcp6RowIt = nullptr;

    DWORD status{}, size = 0, i{};
    bool connectionFound = FALSE;

    status = GetTcp6Table(tcp6Table, &size, TRUE);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        return status;
    }

    tcp6Table = reinterpret_cast<PMIB_TCP6TABLE>(malloc(size));
    if (tcp6Table == nullptr) {
        return ERROR_OUTOFMEMORY;
    }

    status = GetTcp6Table(tcp6Table, &size, TRUE);
    if (status != ERROR_SUCCESS) {
        free(tcp6Table);
        return status;
    }

    for (i = 0; i < tcp6Table->dwNumEntries; i++) {
        tcp6RowIt = &tcp6Table->table[i];
        if (tcp6RowIt->dwLocalPort == static_cast<DWORD>(localPort) && tcp6RowIt->dwRemotePort == static_cast<DWORD>(remotePort) && tcp6RowIt->State == state) {
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
    ULONG status{}, size = 0;
    PUCHAR rw = nullptr;
    TCP_ESTATS_DATA_RW_v0 dataRw{};
    TCP_ESTATS_SND_CONG_RW_v0 sndRw{};
    TCP_ESTATS_PATH_RW_v0 pathRw{};
    TCP_ESTATS_SEND_BUFF_RW_v0 sendBuffRw{};
    TCP_ESTATS_REC_RW_v0 recRw{};
    TCP_ESTATS_OBS_REC_RW_v0 obsRecRw{};
    TCP_ESTATS_BANDWIDTH_RW_v0 bandwidthRw{};
    TCP_ESTATS_FINE_RTT_RW_v0 fineRttRw{};

    switch (type) {
    case TcpConnectionEstatsData:
        dataRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&dataRw);
        size = sizeof(TCP_ESTATS_DATA_RW_v0);
        break;
    case TcpConnectionEstatsSndCong:
        sndRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&sndRw);
        size = sizeof(TCP_ESTATS_SND_CONG_RW_v0);
        break;
    case TcpConnectionEstatsPath:
        pathRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&pathRw);
        size = sizeof(TCP_ESTATS_PATH_RW_v0);
        break;
    case TcpConnectionEstatsSendBuff:
        sendBuffRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&sendBuffRw);
        size = sizeof(TCP_ESTATS_SEND_BUFF_RW_v0);
        break;
    case TcpConnectionEstatsRec:
        recRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&recRw);
        size = sizeof(TCP_ESTATS_REC_RW_v0);
        break;
    case TcpConnectionEstatsObsRec:
        obsRecRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&obsRecRw);
        size = sizeof(TCP_ESTATS_OBS_REC_RW_v0);
        break;
    case TcpConnectionEstatsBandwidth:
        bandwidthRw.EnableCollectionInbound = operation;
        bandwidthRw.EnableCollectionOutbound = operation;
        rw = reinterpret_cast<PUCHAR>(&bandwidthRw);
        size = sizeof(TCP_ESTATS_BANDWIDTH_RW_v0);
        break;
    case TcpConnectionEstatsFineRtt:
        fineRttRw.EnableCollection = enable;
        rw = reinterpret_cast<PUCHAR>(&fineRttRw);
        size = sizeof(TCP_ESTATS_FINE_RTT_RW_v0);
        break;
    default:
        return;
        break;
    }

    if (v6) {
        status = SetPerTcp6ConnectionEStats(reinterpret_cast<PMIB_TCP6ROW>(row), type, rw, 0, size, 0);
    } else {
        status = SetPerTcpConnectionEStats(reinterpret_cast<PMIB_TCPROW>(row), type, rw, 0, size, 0);
    }

    if (status != NO_ERROR) {
        if (v6)
            wprintf(L"\nSetPerTcp6ConnectionEStats %ws %s failed. status = %u", estatsTypeNames[type], enable ? L"enabled" : L"disabled", status);
        else
            wprintf(L"\nSetPerTcpConnectionEStats %ws %s failed. status = %u", estatsTypeNames[type], enable ? L"enabled" : L"disabled", status);
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


ULONG GetConnectionEStats(void * row, TCP_ESTATS_TYPE type, PUCHAR rw, ULONG rwSize, bool v6, PUCHAR ros,
                          ULONG rosSize, PUCHAR rod, ULONG rodSize)
// Call GetPerTcp6ConnectionEStats or GetPerTcpConnectionEStats.
{
    if (v6) {
        return GetPerTcp6ConnectionEStats(reinterpret_cast<PMIB_TCP6ROW>(row), type, rw, 0, rwSize, ros, 0, rosSize, rod, 0, rodSize);
    } else {
        return GetPerTcpConnectionEStats(reinterpret_cast<PMIB_TCPROW>(row), type, rw, 0, rwSize, ros, 0, rosSize, rod, 0, rodSize);
    }
}


void GetAndOutputEstats(void * row, TCP_ESTATS_TYPE type, bool v6)
// Dump the supplied Estate type on the given TCP connection row.
{
    ULONG rosSize = 0, rodSize = 0;
    ULONG winStatus{};
    PUCHAR ros = nullptr, rod = nullptr;

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
        wprintf(L"\nCannot get type %d", type);
        return;
        break;
    }

    if (rosSize != 0) {
        // Use calloc instead of malloc+memset for better performance
        ros = reinterpret_cast<PUCHAR>(calloc(1, rosSize));
        if (ros == nullptr) {
            wprintf(L"\nOut of memory");
            return;
        }
    }
    if (rodSize != 0) {
        // Use calloc instead of malloc+memset for better performance
        rod = reinterpret_cast<PUCHAR>(calloc(1, rodSize));
        if (rod == nullptr) {
            free(ros);
            wprintf(L"\nOut of memory");
            return;
        }
    }

    TCP_ESTATS_DATA_RW_v0 dataRw{};
    TCP_ESTATS_SND_CONG_RW_v0 sndCongRw{};
    TCP_ESTATS_PATH_RW_v0 pathRw{};
    TCP_ESTATS_SEND_BUFF_RW_v0 sndBuffRw{};
    TCP_ESTATS_REC_RW_v0 recRw{};
    TCP_ESTATS_OBS_REC_RW_v0 obsRecRw{};
    TCP_ESTATS_BANDWIDTH_RW_v0 bandwidthRw{};
    TCP_ESTATS_FINE_RTT_RW_v0 fineRttRw{};
    BOOLEAN RwEnableCollection{FALSE};

    switch (type) {
    case TcpConnectionEstatsData:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&dataRw), sizeof(TCP_ESTATS_DATA_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = dataRw.EnableCollection;
        break;
    case TcpConnectionEstatsSndCong:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&sndCongRw), sizeof(TCP_ESTATS_SND_CONG_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = sndCongRw.EnableCollection;
        break;
    case TcpConnectionEstatsPath:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&pathRw), sizeof(TCP_ESTATS_PATH_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = pathRw.EnableCollection;
        break;
    case TcpConnectionEstatsSendBuff:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&sndBuffRw), sizeof(TCP_ESTATS_SEND_BUFF_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = sndBuffRw.EnableCollection;
        break;
    case TcpConnectionEstatsRec:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&recRw), sizeof(TCP_ESTATS_REC_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = recRw.EnableCollection;
        break;
    case TcpConnectionEstatsObsRec:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&obsRecRw), sizeof(TCP_ESTATS_OBS_REC_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = obsRecRw.EnableCollection;
        break;
    case TcpConnectionEstatsBandwidth:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&bandwidthRw), sizeof(TCP_ESTATS_BANDWIDTH_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = bandwidthRw.EnableCollectionOutbound && bandwidthRw.EnableCollectionInbound;
        break;
    case TcpConnectionEstatsFineRtt:
        winStatus = GetConnectionEStats(row, type, reinterpret_cast<PUCHAR>(&fineRttRw), sizeof(TCP_ESTATS_FINE_RTT_RW_v0), v6, ros, rosSize, rod, rodSize);
        RwEnableCollection = fineRttRw.EnableCollection;
        break;
    default:
        winStatus = GetConnectionEStats(row, type, nullptr, v6, 0, ros, rosSize, rod, rodSize);
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
            wprintf(L"\nGetPerTcp6ConnectionEStats %ls failed. status = %u", estatsTypeNames[type], winStatus);
        else
            wprintf(L"\nGetPerTcpConnectionEStats %ls failed. status = %u", estatsTypeNames[type], winStatus);
    } else {
        switch (type) {
        case TcpConnectionEstatsSynOpts:
            synOptsRos = reinterpret_cast<PTCP_ESTATS_SYN_OPTS_ROS_v0>(ros);
            wprintf(L"\nSyn Opts");
            wprintf(L"\nActive Open:    %s", synOptsRos->ActiveOpen ? L"Yes" : L"No");
            wprintf(L"\nMss Received:   %u", synOptsRos->MssRcvd);
            wprintf(L"\nMss Sent        %u", synOptsRos->MssSent);
            break;
        case TcpConnectionEstatsData:
            dataRod = reinterpret_cast<PTCP_ESTATS_DATA_ROD_v0>(rod);
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
            sndCongRod = reinterpret_cast<PTCP_ESTATS_SND_CONG_ROD_v0>(rod);
            sndCongRos = reinterpret_cast<PTCP_ESTATS_SND_CONG_ROS_v0>(ros);
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
            pathRod = reinterpret_cast<PTCP_ESTATS_PATH_ROD_v0>(rod);
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
            sndBuffRod = reinterpret_cast<PTCP_ESTATS_SEND_BUFF_ROD_v0>(rod);
            wprintf(L"\n\nSend Buff");
            wprintf(L"\nCur Retx Queue:   %u", sndBuffRod->CurRetxQueue);
            wprintf(L"\nMax Retx Queue:   %u", sndBuffRod->MaxRetxQueue);
            wprintf(L"\nCur App W Queue:  %u", sndBuffRod->CurAppWQueue);
            wprintf(L"\nMax App W Queue:  %u", sndBuffRod->MaxAppWQueue);
            break;
        case TcpConnectionEstatsRec:
            recRod = reinterpret_cast<PTCP_ESTATS_REC_ROD_v0>(rod);
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
            obsRecRod = reinterpret_cast<PTCP_ESTATS_OBS_REC_ROD_v0>(rod);
            wprintf(L"\n\nObs Rec");
            wprintf(L"\nCur Rwin Rcvd:   0x%x (%u)", obsRecRod->CurRwinRcvd, obsRecRod->CurRwinRcvd);
            wprintf(L"\nMax Rwin Rcvd:   0x%x (%u)", obsRecRod->MaxRwinRcvd, obsRecRod->MaxRwinRcvd);
            wprintf(L"\nMin Rwin Rcvd:   0x%x (%u)", obsRecRod->MinRwinRcvd, obsRecRod->MinRwinRcvd);
            wprintf(L"\nWin Scale Rcvd:  0x%x (%u)", obsRecRod->WinScaleRcvd, obsRecRod->WinScaleRcvd);
            break;
        case TcpConnectionEstatsBandwidth:
            bandwidthRod = reinterpret_cast<PTCP_ESTATS_BANDWIDTH_ROD_v0>(rod);
            wprintf(L"\n\nBandwidth");
            wprintf(L"\nOutbound Bandwidth:   %lu", bandwidthRod->OutboundBandwidth);
            wprintf(L"\nInbound Bandwidth:    %lu", bandwidthRod->InboundBandwidth);
            wprintf(L"\nOutbound Instability: %lu", bandwidthRod->OutboundInstability);
            wprintf(L"\nInbound Instability:  %lu", bandwidthRod->InboundInstability);
            wprintf(L"\nOutbound Bandwidth Peaked: %s", bandwidthRod->OutboundBandwidthPeaked ? L"Yes" : L"No");
            wprintf(L"\nInbound Bandwidth Peaked:  %s", bandwidthRod->InboundBandwidthPeaked ? L"Yes" : L"No");
            break;
        case TcpConnectionEstatsFineRtt:
            fineRttRod = reinterpret_cast<PTCP_ESTATS_FINE_RTT_ROD_v0>(rod);
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
