#pragma once

#include "framework.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


//https://msdn.microsoft.com/en-us/library/windows/desktop/ms738518%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
//其实：IPV4的只需This buffer should be large enough to hold at least 16 characters.
#define MAX_ADDRESS_STRING_LENGTH   64


#define MALLOC(x) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3


#ifndef DLLEXPORT
#define DLLEXPORT __declspec( dllexport ) //之所以定义这个，是因为.clang-format下，这个宏可以保持换行。
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////


void DisplayError(_In_ DWORD dwMessageId);
void GetTimeString(LARGE_INTEGER UtcTime, _Out_ LPWSTR TimeString);
void PrintTcpConnectionState(_In_ DWORD dwState);
void PrintInterfaceType(_In_ WORD Type);
void PrintNeighborState(_In_ NL_NEIGHBOR_STATE State);
void PrintProtocol(IPPROTO protocol);
void PrintSocketType(int SocketType);
void PrintAddressFamily(_In_ ADDRESS_FAMILY Family);
void PrintProviderNamespace(DWORD NameSpace);
void PrintOffloadStateOfTcpConnection(_In_ TCP_CONNECTION_OFFLOAD_STATE OffloadState);
void PrintArpType(_In_ MIB_IPNET_TYPE Type);
void PrintRouteType(_In_ DWORD dwForwardType);
void PrintRoutingProtocols(_In_ DWORD dwForwardProto);
void PrintInterfaceOperationalStatus(_In_ INTERNAL_IF_OPER_STATUS dwOperStatus);
void PrintAdapterType(INT Type);
void PrintPrefixOrigin(NL_PREFIX_ORIGIN PrefixOrigin);
void PrintSuffixOrigin(NL_SUFFIX_ORIGIN SuffixOrigin);
void PrintDadState(NL_DAD_STATE DadState);
void PrintNodeType(UINT NodeType);


//////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions to reduce code duplication


inline void PrintIpv4AddrAndPort(DWORD dwAddr, DWORD dwPort, DWORD index, const char* prefix)
{
    char szAddr[128]{};
    in_addr IpAddr{};
    IpAddr.S_un.S_addr = dwAddr;
    strcpy_s(szAddr, sizeof(szAddr), inet_ntoa(IpAddr));
    printf("\t%s[%lu] Addr: %s\n", prefix, index, szAddr);
    printf("\t%s[%lu] Port: %u\n", prefix, index, ntohs(static_cast<u_short>(dwPort)));
}


inline void PrintIpv6AddrAndPort(const BYTE* ucAddr, DWORD dwPort, DWORD index, const char* prefix)
{
    char szAddr[128]{};
    InetNtopA(AF_INET6, ucAddr, szAddr, _ARRAYSIZE(szAddr));
    printf("\t%s[%lu] Addr: %s\n", prefix, index, szAddr);
    printf("\t%s[%lu] Port: %u\n", prefix, index, ntohs(static_cast<u_short>(dwPort)));
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Helper template for GetOwnerModule functions to reduce code duplication


template<typename TRow, typename TGetFunc>
inline void GetOwnerModuleFromEntryEx(_In_ TRow pEntry, TGetFunc getFunc)
{
    TCPIP_OWNER_MODULE_BASIC_INFO Buffer = {};
    DWORD Size = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
    DWORD ret = getFunc(pEntry, TCPIP_OWNER_MODULE_INFO_BASIC, &Buffer, &Size);
    if (NO_ERROR == ret) {
        printf("\tModuleName: %ls\n", Buffer.pModuleName);
        printf("\tModulePath: %ls\n", Buffer.pModulePath);
        return;
    }

    if (ERROR_NOT_FOUND == ret) {
        return;
    }

    auto pBuffer = reinterpret_cast<PTCPIP_OWNER_MODULE_BASIC_INFO>(MALLOC(Size));
    _ASSERTE(pBuffer);
    if (!pBuffer) {
        return;
    }

    ret = getFunc(pEntry, TCPIP_OWNER_MODULE_INFO_BASIC, pBuffer, &Size);
    _ASSERTE(NO_ERROR == ret);
    if (NO_ERROR == ret) {
        printf("\tModuleName: %ls\n", pBuffer->pModuleName);
        printf("\tModulePath: %ls\n", pBuffer->pModulePath);
    }

    FREE(pBuffer);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Helper template for allocating table with ERROR_INSUFFICIENT_BUFFER retry pattern


template<typename TTable>
inline TTable* AllocateTableWithRetry(DWORD initialSize, DWORD* pActualSize, int* pErrorCode)
{
    *pErrorCode = 0;
    
    TTable* pTable = reinterpret_cast<TTable*>(MALLOC(initialSize));
    if (pTable == nullptr) {
        *pErrorCode = ERROR_OUTOFMEMORY;
        return nullptr;
    }
    
    *pActualSize = initialSize;
    return pTable;
}


template<typename TTable>
inline TTable* ReallocateTable(TTable* pOldTable, DWORD newSize, int* pErrorCode)
{
    *pErrorCode = 0;
    
    if (pOldTable) {
        FREE(pOldTable);
    }
    
    TTable* pTable = reinterpret_cast<TTable*>(MALLOC(newSize));
    if (pTable == nullptr) {
        *pErrorCode = ERROR_OUTOFMEMORY;
        return nullptr;
    }
    
    return pTable;
}
