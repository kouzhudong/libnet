#include "pch.h"
#include "udp.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


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


//////////////////////////////////////////////////////////////////////////////////////////////////
