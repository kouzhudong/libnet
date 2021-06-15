#include "pch.h"
#include "IpHelper.h"


/*
有待实现的函数有：
GetAnycastIpAddressTable
GetIfStackTable
GetInvertedIfStackTable
GetIpInterfaceTable
GetIpPathTable
GetMulticastIpAddressTable
GetUnicastIpAddressTable
NotifyStableUnicastIpAddressTable
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
            switch (pTcpTable->table[i].dwState) {
            case MIB_TCP_STATE_CLOSED:
                printf("CLOSED\n");
                break;
            case MIB_TCP_STATE_LISTEN:
                printf("LISTEN\n");
                break;
            case MIB_TCP_STATE_SYN_SENT:
                printf("SYN-SENT\n");
                break;
            case MIB_TCP_STATE_SYN_RCVD:
                printf("SYN-RECEIVED\n");
                break;
            case MIB_TCP_STATE_ESTAB:
                printf("ESTABLISHED\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
                printf("FIN-WAIT-1\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
                printf("FIN-WAIT-2 \n");
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
                printf("CLOSE-WAIT\n");
                break;
            case MIB_TCP_STATE_CLOSING:
                printf("CLOSING\n");
                break;
            case MIB_TCP_STATE_LAST_ACK:
                printf("LAST-ACK\n");
                break;
            case MIB_TCP_STATE_TIME_WAIT:
                printf("TIME-WAIT\n");
                break;
            case MIB_TCP_STATE_DELETE_TCB:
                printf("DELETE-TCB\n");
                break;
            default:
                printf("UNKNOWN dwState value\n");
                break;
            }
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
            switch (pTcpTable->table[i].dwState) {
            case MIB_TCP_STATE_CLOSED:
                printf("CLOSED\n");
                break;
            case MIB_TCP_STATE_LISTEN:
                printf("LISTEN\n");
                break;
            case MIB_TCP_STATE_SYN_SENT:
                printf("SYN-SENT\n");
                break;
            case MIB_TCP_STATE_SYN_RCVD:
                printf("SYN-RECEIVED\n");
                break;
            case MIB_TCP_STATE_ESTAB:
                printf("ESTABLISHED\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
                printf("FIN-WAIT-1\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
                printf("FIN-WAIT-2 \n");
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
                printf("CLOSE-WAIT\n");
                break;
            case MIB_TCP_STATE_CLOSING:
                printf("CLOSING\n");
                break;
            case MIB_TCP_STATE_LAST_ACK:
                printf("LAST-ACK\n");
                break;
            case MIB_TCP_STATE_TIME_WAIT:
                printf("TIME-WAIT\n");
                break;
            case MIB_TCP_STATE_DELETE_TCB:
                printf("DELETE-TCB\n");
                break;
            default:
                printf("UNKNOWN dwState value\n");
                break;
            }

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
            switch (pTcpTable->table[i].State) {
            case MIB_TCP_STATE_CLOSED:
                wprintf(L"CLOSED\n");
                break;
            case MIB_TCP_STATE_LISTEN:
                wprintf(L"LISTEN\n");
                break;
            case MIB_TCP_STATE_SYN_SENT:
                wprintf(L"SYN-SENT\n");
                break;
            case MIB_TCP_STATE_SYN_RCVD:
                wprintf(L"SYN-RECEIVED\n");
                break;
            case MIB_TCP_STATE_ESTAB:
                wprintf(L"ESTABLISHED\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
                wprintf(L"FIN-WAIT-1\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
                wprintf(L"FIN-WAIT-2 \n");
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
                wprintf(L"CLOSE-WAIT\n");
                break;
            case MIB_TCP_STATE_CLOSING:
                wprintf(L"CLOSING\n");
                break;
            case MIB_TCP_STATE_LAST_ACK:
                wprintf(L"LAST-ACK\n");
                break;
            case MIB_TCP_STATE_TIME_WAIT:
                wprintf(L"TIME-WAIT\n");
                break;
            case MIB_TCP_STATE_DELETE_TCB:
                wprintf(L"DELETE-TCB\n");
                break;
            default:
                wprintf(L"UNKNOWN dwState value\n");
                break;
            }

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
int WINAPI EnumExtendedTcpTable()
/*
文件名：GetExtendedTcpTable.Cpp
功能：获取本地的IPv4的TCP的带进程关联的网络信息。

扩展：IPV6的及UDP的信息类似。

注释：修改自MSDN的GetTcpTable2的例子。

made by correy
made at 2016.11.30
http://correy.webs.com

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
            switch (pTcpTable->table[i].dwState) {
            case MIB_TCP_STATE_CLOSED:
                printf("CLOSED\n");
                break;
            case MIB_TCP_STATE_LISTEN:
                printf("LISTEN\n");
                break;
            case MIB_TCP_STATE_SYN_SENT:
                printf("SYN-SENT\n");
                break;
            case MIB_TCP_STATE_SYN_RCVD:
                printf("SYN-RECEIVED\n");
                break;
            case MIB_TCP_STATE_ESTAB:
                printf("ESTABLISHED\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
                printf("FIN-WAIT-1\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
                printf("FIN-WAIT-2 \n");
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
                printf("CLOSE-WAIT\n");
                break;
            case MIB_TCP_STATE_CLOSING:
                printf("CLOSING\n");
                break;
            case MIB_TCP_STATE_LAST_ACK:
                printf("LAST-ACK\n");
                break;
            case MIB_TCP_STATE_TIME_WAIT:
                printf("TIME-WAIT\n");
                break;
            case MIB_TCP_STATE_DELETE_TCB:
                printf("DELETE-TCB\n");
                break;
            default:
                wprintf(L"UNKNOWN dwState value: %d\n", pTcpTable->table[i].dwState);
                break;
            }

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
    if ((dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_MODULE, 0)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pUdpTable);
        pUdpTable = (PMIB_UDPTABLE_OWNER_MODULE)MALLOC(dwSize);
        if (pUdpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetUdpTable to get the actual data we require
    if ((dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_MODULE, 0)) == NO_ERROR) {

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

            TCPIP_OWNER_MODULE_BASIC_INFO temp = {0};
            DWORD                        Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
            DWORD ret = GetOwnerModuleFromUdpEntry(&pUdpTable->table[i], TCPIP_OWNER_MODULE_INFO_BASIC, &temp, &Size);
            _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

            PTCPIP_OWNER_MODULE_BASIC_INFO ptombi = (PTCPIP_OWNER_MODULE_BASIC_INFO)
                HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
            _ASSERTE(ptombi);

            ret = GetOwnerModuleFromUdpEntry(&pUdpTable->table[i], TCPIP_OWNER_MODULE_INFO_BASIC, ptombi, &Size);
            _ASSERTE(NO_ERROR == ret);

            printf("\tUdp[%d] ModuleName: %ls, ModulePath: %ls \n", i, ptombi->pModuleName, ptombi->pModulePath);

            /*
            pModulePath是全路径，但并非是EXE，而有可能是DLL，这个更精细。
            其实完全可根据dwOwningPid获取EXE的全路径。
            GetOwnerModuleFromPidAndInfo与GetOwnerModuleFromUdpEntry的功能类似。
            */

            printf("\n");

            HeapFree(GetProcessHeap(), 0, ptombi);
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
    if ((dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET6, UDP_TABLE_OWNER_MODULE, 0)) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pUdpTable);
        pUdpTable = (PMIB_UDP6TABLE_OWNER_MODULE)MALLOC(dwSize);
        if (pUdpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }

    // Make a second call to GetUdpTable to get the actual data we require
    if ((dwRetVal = GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET6, UDP_TABLE_OWNER_MODULE, 0)) == NO_ERROR) {

        printf("\tUdpV6的总数:Number of entries: %d\n", (int)pUdpTable->dwNumEntries);
        printf("\n");

        for (int i = 0; i < (int)pUdpTable->dwNumEntries; i++) {
            char szLocalAddr[128];

            InetNtopA(AF_INET6, &pUdpTable->table[i].ucLocalAddr, szLocalAddr, 46);

            printf("\tUdp[%d] Local Addr: %s\n", i, szLocalAddr);
            printf("\tUdp[%d] Local Port: %d \n", i, ntohs((u_short)pUdpTable->table[i].dwLocalPort));
            printf("\tUdp[%d] OwningPid: %d \n", i, (u_short)pUdpTable->table[i].dwOwningPid);

            TCPIP_OWNER_MODULE_BASIC_INFO temp = {0};
            DWORD                        Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
            DWORD ret = GetOwnerModuleFromUdp6Entry(&pUdpTable->table[i],
                                                    TCPIP_OWNER_MODULE_INFO_BASIC,
                                                    &temp,
                                                    &Size);
            _ASSERTE(ERROR_INSUFFICIENT_BUFFER == ret);

            PTCPIP_OWNER_MODULE_BASIC_INFO ptombi = (PTCPIP_OWNER_MODULE_BASIC_INFO)
                HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
            _ASSERTE(ptombi);

            ret = GetOwnerModuleFromUdp6Entry(&pUdpTable->table[i],
                                              TCPIP_OWNER_MODULE_INFO_BASIC,
                                              ptombi,
                                              &Size);
            _ASSERTE(NO_ERROR == ret);

            printf("\tUdp[%d] ModuleName: %ls, ModulePath: %ls \n", i, ptombi->pModuleName, ptombi->pModulePath);

            /*
            pModulePath是全路径，但并非是EXE，而有可能是DLL，这个更精细。
            其实完全可根据dwOwningPid获取EXE的全路径。
            GetOwnerModuleFromPidAndInfo与GetOwnerModuleFromUdpEntry的功能类似。
            */

            printf("\n");

            HeapFree(GetProcessHeap(), 0, ptombi);
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
The following example retrieves the IP address table, then prints some members of the IP address entries in the table.

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
addresses associated with the adapters

This example retrieves the IP_ADAPTER_ADDRESSES structure for the adapters associated with the system and prints some members for each adapter interface.

The GetAdaptersAddresses function retrieves the addresses associated with the adapters on the local computer.
https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
*/
{
    /* Declare and initialize variables */
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

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

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf
            ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
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
                for (i = 0; pUnicast != NULL; i++)
                    pUnicast = pUnicast->Next;
                printf("\tNumber of Unicast Addresses: %d\n", i);
            } else
                printf("\tNo Unicast Addresses\n");

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != NULL; i++)
                    pAnycast = pAnycast->Next;
                printf("\tNumber of Anycast Addresses: %d\n", i);
            } else
                printf("\tNo Anycast Addresses\n");

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != NULL; i++)
                    pMulticast = pMulticast->Next;
                printf("\tNumber of Multicast Addresses: %d\n", i);
            } else
                printf("\tNo Multicast Addresses\n");

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != NULL; i++)
                    pDnServer = pDnServer->Next;
                printf("\tNumber of DNS Server Addresses: %d\n", i);
            } else
                printf("\tNo DNS Server Addresses\n");

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
                for (i = 0; pPrefix != NULL; i++)
                    pPrefix = pPrefix->Next;
                printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
            } else
                printf("\tNumber of IP Adapter Prefix entries: 0\n");

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
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              // Default language
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

// It is possible for an adapter to have multiple
// IPv4 addresses, gateways, and secondary WINS servers
// assigned to the adapter. 
//
// Note that this sample code only prints out the 
// first entry for the IP address/mask, and gateway, and
// the primary and secondary WINS server for each adapter. 

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
int WINAPI EnumIpNetTable2()
/*
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
            printf("Unknown: %lld\n", pipTable->Table[i].InterfaceLuid.Info.IfType);
            break;
        }

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
        switch (pipTable->Table[i].State) {
        case NlnsUnreachable:
            printf("NlnsUnreachable\n");
            break;
        case NlnsIncomplete:
            printf("NlnsIncomplete\n");
            break;
        case NlnsProbe:
            printf("NlnsProbe\n");
            break;
        case NlnsDelay:
            printf("NlnsDelay\n");
            break;
        case NlnsStale:
            printf("NlnsStale\n");
            break;
        case NlnsReachable:
            printf("NlnsReachable\n");
            break;
        case NlnsPermanent:
            printf("NlnsPermanent\n");
            break;
        default:
            printf("Unknown: %d\n", pipTable->Table[i].State);
            break;
        }

        printf("Flags[%d]:\t\t %u\n", (int)i, (unsigned char)pipTable->Table[i].Flags);

        printf("ReachabilityTime[%d]:\t %lu, %lu\n\n", (int)i,
               pipTable->Table[i].ReachabilityTime.LastReachable,
               pipTable->Table[i].ReachabilityTime.LastUnreachable);
    }

    FreeMibTable(pipTable);
    pipTable = NULL;

    exit(0);
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
            switch (pIfRow->dwType) {
            case IF_TYPE_OTHER:
                printf("Other\n");
                break;
            case IF_TYPE_ETHERNET_CSMACD:
                printf("Ethernet\n");
                break;
            case IF_TYPE_ISO88025_TOKENRING:
                printf("Token Ring\n");
                break;
            case IF_TYPE_PPP:
                printf("PPP\n");
                break;
            case IF_TYPE_SOFTWARE_LOOPBACK:
                printf("Software Lookback\n");
                break;
            case IF_TYPE_ATM:
                printf("ATM\n");
                break;
            case IF_TYPE_IEEE80211:
                printf("IEEE 802.11 Wireless\n");
                break;
            case IF_TYPE_TUNNEL:
                printf("Tunnel type encapsulation\n");
                break;
            case IF_TYPE_IEEE1394:
                printf("IEEE 1394 Firewire\n");
                break;
            default:
                printf("Unknown type %ld\n", pIfRow->dwType);
                break;
            }

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


HANDLE NotificationHandle = NULL;


VOID NETIOAPI_API_ IpInterfaceChange(_In_ PVOID CallerContext,
                                     _In_ PMIB_IPINTERFACE_ROW Row OPTIONAL,
                                     _In_ MIB_NOTIFICATION_TYPE NotificationType
)
/*
注册的过程中会被调用几次，NotificationType == MibInitialNotification。

如何触发？使这里被运行呢？
1.关闭和开启网卡也不行。
2.改变IP地址也不行。
*/
{
    switch (NotificationType) {
    case MibParameterNotification:

        break;
    case MibAddInstance:

        break;
    case MibDeleteInstance:

        break;
    case MibInitialNotification:

        break;
    default:
        break;
    }
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyIpInterfaceChange()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotificationHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyIpInterfaceChange()
/*
The NotifyIpInterfaceChange function registers to be notified for changes to all IP interfaces,
IPv4 interfaces, or IPv6 interfaces on a local computer.

netsh interface ipv4 show interface

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyipinterfacechange
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

    NTSTATUS Status = NotifyIpInterfaceChange(Family,
                                              IpInterfaceChange,
                                              CallerContext,
                                              InitialNotification,
                                              &NotificationHandle);

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID NETIOAPI_API_ NetworkConnectivityHintChange(_In_ PVOID CallerContext,
                                                 _In_ NL_NETWORK_CONNECTIVITY_HINT ConnectivityHint
)
/*
注册的过程中会被调用1次。

如何触发？使这里被运行呢？
1.关闭和开启网卡也不行。
2.改变IP地址也不行。
*/
{

}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyNetworkConnectivityHintChange()
/*
Registers an application-defined callback function, to be called when the aggregate network connectivity level and cost hints change.

这个也没有反注册的函数？没找到。

Minimum supported client	Windows 10, version 2004 (10.0; Build 19041)
Minimum supported server	Windows Server, version 2004 (10.0; Build 19041)

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifynetworkconnectivityhintchange
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;
    HANDLE NotificationHandle = NULL;

    NTSTATUS Status = NotifyNetworkConnectivityHintChange(NetworkConnectivityHintChange,
                                                          CallerContext,
                                                          InitialNotification,
                                                          &NotificationHandle);

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE RouteChange2Handle = NULL;


VOID NETIOAPI_API_ RouteChange2(_In_ PVOID CallerContext,
                                _In_opt_ PMIB_IPFORWARD_ROW2 Row,
                                _In_ MIB_NOTIFICATION_TYPE NotificationType
)
/*
注册的过程中会被调用几次。

route  -f命令会走这里。
*/
{
    switch (NotificationType) {
    case MibParameterNotification:

        break;
    case MibAddInstance:

        break;
    case MibDeleteInstance:

        break;
    case MibInitialNotification:

        break;
    default:
        break;
    }
}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyRouteChange2()
{
    NTSTATUS Status = CancelMibChangeNotify2(RouteChange2Handle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyRouteChange2()
/*
The NotifyRouteChange2 function registers to be notified for changes to IP route entries on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyroutechange2
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

#pragma prefast(push)
#pragma prefast(disable: 6387, "“_Param_(3)”可能是“0”: 这不符合函数“NotifyRouteChange2”的规范")
    NTSTATUS Status = NotifyRouteChange2(Family,
                                         RouteChange2,
                                         CallerContext,
                                         InitialNotification,
                                         &RouteChange2Handle);
#pragma prefast(pop)

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotifyStableUnicastIpAddressTableHandle = NULL;


VOID NETIOAPI_API_ StableUnicastIpAddressTableCallerCallback(_In_ PVOID CallerContext,
                                                             _In_ PMIB_UNICASTIPADDRESS_TABLE AddressTable
)
/*

*/
{

}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyStableUnicastIpAddressTable()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotifyStableUnicastIpAddressTableHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyStableUnicastIpAddressTable()
/*
The NotifyStableUnicastIpAddressTable function retrieves the stable unicast IP address table on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifystableunicastipaddresstable
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PMIB_UNICASTIPADDRESS_TABLE * Table = NULL;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

#pragma prefast(push)
#pragma prefast(disable: 6387, "“Table”可能是“0”: 这不符合函数“NotifyStableUnicastIpAddressTable”的规范")
    NTSTATUS Status = NotifyStableUnicastIpAddressTable(Family,
                                                        Table,
                                                        StableUnicastIpAddressTableCallerCallback,
                                                        CallerContext,
                                                        &NotifyStableUnicastIpAddressTableHandle);
#pragma prefast(pop)

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE TeredoPortChangeHandle = NULL;


VOID NETIOAPI_API_ TeredoPortChange(_In_ PVOID CallerContext,
                                    _In_ USHORT Port,
                                    _Inout_ MIB_NOTIFICATION_TYPE NotificationType
)
/*
注册时，这里被调用了。
*/
{

}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyTeredoPortChange()
{
    NTSTATUS Status = CancelMibChangeNotify2(TeredoPortChangeHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyTeredoPortChange()
/*
The NotifyTeredoPortChange function registers to be notified for changes to the UDP port number used by the Teredo client for the Teredo service port on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyteredoportchange
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PMIB_UNICASTIPADDRESS_TABLE * Table = NULL;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

#pragma prefast(push)
#pragma prefast(disable: 6387, "“CallerContext”可能是“0”: 这不符合函数“NotifyTeredoPortChange”的规范")
#pragma warning(disable:4995)
    NTSTATUS Status = NotifyTeredoPortChange(TeredoPortChange,
                                             CallerContext,
                                             InitialNotification,
                                             &TeredoPortChangeHandle);
#pragma prefast(pop)

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


HANDLE NotifyUnicastIpAddressChangeHandle = NULL;


VOID NETIOAPI_API_ UnicastIpAddressChange(_In_ PVOID CallerContext,
                                          _In_opt_ PMIB_UNICASTIPADDRESS_ROW Row,
                                          _In_ MIB_NOTIFICATION_TYPE NotificationType
)
/*

*/
{

}


EXTERN_C
__declspec(dllexport)
void WINAPI DeregisterNotifyUnicastIpAddressChange()
{
    NTSTATUS Status = CancelMibChangeNotify2(NotifyUnicastIpAddressChangeHandle);
}


EXTERN_C
__declspec(dllexport)
void WINAPI RegistersNotifyUnicastIpAddressChange()
/*
The NotifyUnicastIpAddressChange function registers to be notified for changes to all unicast IP interfaces, unicast IPv4 addresses,
or unicast IPv6 addresses on a local computer.

https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-notifyunicastipaddresschange
*/
{
    ADDRESS_FAMILY Family = AF_UNSPEC;
    PMIB_UNICASTIPADDRESS_TABLE * Table = NULL;
    PVOID CallerContext = NULL;
    BOOLEAN InitialNotification = TRUE;

    NTSTATUS Status = NotifyUnicastIpAddressChange(Family,
                                                   UnicastIpAddressChange,
                                                   CallerContext,
                                                   InitialNotification,
                                                   &NotifyUnicastIpAddressChangeHandle);

    (void)getchar();
}


//////////////////////////////////////////////////////////////////////////////////////////////////
