#include "pch.h"
#include "spi.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void PrintProviderNamespace(DWORD NameSpace)
{
    switch (NameSpace) {
    case NS_ALL:
        wprintf(L"NS_ALL\n");
        break;
    case NS_SAP:
        wprintf(L"NS_SAP\n");
        break;
    case NS_NDS:
        wprintf(L"NS_NDS\n");
        break;
    case NS_PEER_BROWSE:
        wprintf(L"NS_PEER_BROWSE\n");
        break;
    case NS_SLP:
        wprintf(L"NS_SLP\n");
        break;
    case NS_DHCP:
        wprintf(L"NS_DHCP\n");
        break;
    case NS_TCPIP_LOCAL:
        wprintf(L"NS_TCPIP_LOCAL\n");
        break;
    case NS_TCPIP_HOSTS:
        wprintf(L"NS_TCPIP_HOSTS\n");
        break;
    case NS_DNS:
        wprintf(L"Domain Name System (NS_DNS)\n");
        break;
    case NS_NETBT:
        wprintf(L"NetBIOS (NS_NETBT)\n");
        break;
    case NS_WINS:
        wprintf(L"Windows Internet Naming Service (NS_WINS)\n");
        break;
    case NS_NLA:
        wprintf(L"Network Location Awareness (NS_NLA)\n");
        break;
    case NS_BTH:
        wprintf(L"Bluetooth (NS_BTH)\n");
        break;
    case NS_LOCALNAME:
        wprintf(L"NS_LOCALNAME\n");
        break;
    case NS_NBP:
        wprintf(L"NS_NBP\n");
        break;
    case NS_MS:
        wprintf(L"NS_MS\n");
        break;
    case NS_STDA:
        wprintf(L"NS_STDA\n");
        break;
    case NS_NTDS:
        wprintf(L"Windows NT Directory Services (NS_NTDS)\n");
        break;
    case NS_EMAIL:
        wprintf(L"Email (NS_EMAIL)\n");
        break;
    case NS_PNRPNAME:
        wprintf(L"Peer-to-peer (NS_PNRPNAME)\n");
        break;
    case NS_PNRPCLOUD:
        wprintf(L"Peer-to-peer collection (NS_PNRPCLOUD)\n");
        break;
    case NS_X500:
        wprintf(L"NS_X500\n");
        break;
    case NS_NIS:
        wprintf(L"NS_NIS\n");
        break;
    case NS_NISPLUS:
        wprintf(L"NS_NISPLUS\n");
        break;
    case NS_WRQ:
        wprintf(L"NS_WRQ\n");
        break;
    case NS_NETDES: /* Network Designers Limited */
        wprintf(L"NS_NETDES\n");
        break;
    default:
        wprintf(L"Other value (%u)\n", NameSpace);
        break;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int EnumNameSpaceProviders()
/*
https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumnamespaceprovidersa
https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumnamespaceprovidersw
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    int iResult{};
    int iError = 0;
    INT iNuminfo = 0;
    int i{};

    // Allocate a 4K buffer to retrieve all the namespace providers
    DWORD dwInitialBufferLen = 4096;
    DWORD dwBufferLen{};
    LPWSANAMESPACE_INFO lpProviderInfo{};

    // variables needed for converting provider GUID to a string
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    // Set dwBufferLen to the initial buffer length
    dwBufferLen = dwInitialBufferLen;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    lpProviderInfo = reinterpret_cast<LPWSANAMESPACE_INFO>(MALLOC(dwBufferLen));
    if (lpProviderInfo == nullptr) {
        wprintf(L"Memory allocation for providers buffer failed\n");
        WSACleanup();
        return 1;
    }

    iNuminfo = WSAEnumNameSpaceProviders(&dwBufferLen, lpProviderInfo);
    if (iNuminfo == SOCKET_ERROR) {
        iError = WSAGetLastError();
        if (iError == WSAEFAULT && dwBufferLen != dwInitialBufferLen) {
            wprintf(L"WSAEnumNameSpaceProviders failed with too small a buffer\n");
            wprintf(L"  Increasing the buffer to %u\n\n", dwBufferLen);
            if (lpProviderInfo) {
                FREE(lpProviderInfo);
                lpProviderInfo = nullptr;
            }

            lpProviderInfo = reinterpret_cast<LPWSANAMESPACE_INFO>(MALLOC(dwBufferLen));
            if (lpProviderInfo == nullptr) {
                wprintf(L"Memory allocation for providers buffer failed\n");
                WSACleanup();
                return 1;
            }

            iNuminfo = WSAEnumNameSpaceProviders(&dwBufferLen, lpProviderInfo);
            if (iNuminfo == SOCKET_ERROR) {
                wprintf(L"WSAEnumNameSpaceProviders failed with error: %d\n", WSAGetLastError());
                if (lpProviderInfo) {
                    FREE(lpProviderInfo);
                    lpProviderInfo = nullptr;
                }
                WSACleanup();
                return 1;
            } else
                wprintf(L"\n");
        } else {
            wprintf(L"WSAEnumNameSpaceProviders failed with error: %d\n", WSAGetLastError());
            if (lpProviderInfo) {
                FREE(lpProviderInfo);
                lpProviderInfo = nullptr;
            }
            WSACleanup();
            return 1;
        }
    }

    wprintf(L"WSAEnumNameSpaceProviders succeeded with provider data count = %d\n\n", iNuminfo);
    for (i = 0; i < iNuminfo; i++) {
        iRet = StringFromGUID2(lpProviderInfo[i].NSProviderId, reinterpret_cast<LPOLESTR>(&GuidString), 39);
        if (iRet == 0)
            wprintf(L"StringFromGUID2 failed\n");
        else
            wprintf(L"NameSpace ProviderId[%d] = %ws\n", i, GuidString);

        wprintf(L"NameSpace[%d] = ", i);
        PrintProviderNamespace(lpProviderInfo[i].dwNameSpace);

        if (lpProviderInfo[i].fActive)
            wprintf(L"Namespace[%d] is active\n", i);
        else
            wprintf(L"Namespace[%d] is inactive\n", i);

        wprintf(L"NameSpace Version[%d] = %u\n", i, lpProviderInfo[i].dwVersion);
        wprintf(L"Namespace Identifier[%d] = %ws\n\n", i, lpProviderInfo[i].lpszIdentifier);
    }

    if (lpProviderInfo) {
        FREE(lpProviderInfo);
        lpProviderInfo = nullptr;
    }
    WSACleanup();
    return 0;
}


int EnumNetworkEvents()
/*
https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumnetworkevents
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    int iResult{};
    SOCKET SocketArray[WSA_MAXIMUM_WAIT_EVENTS]{}, ListenSocket{};
    WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS]{};
    WSANETWORKEVENTS NetworkEvents{};
    sockaddr_in InetAddr{};
    DWORD EventTotal = 0;
    DWORD Index{};
    DWORD i{};
    HANDLE NewEvent = nullptr;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Create a listening socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    InetAddr.sin_family = AF_INET;
    InetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InetAddr.sin_port = htons(27015);

    // Bind the listening socket
    iResult = bind(ListenSocket, reinterpret_cast<SOCKADDR *>(&InetAddr), sizeof(InetAddr));
    if (iResult != 0) {
        wprintf(L"bind failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a new event
    NewEvent = WSACreateEvent();
    if (NewEvent == nullptr) {
        wprintf(L"WSACreateEvent failed with error: %u\n", GetLastError());
        return 1;
    }

    // Associate event types FD_ACCEPT and FD_CLOSE
    // with the listening socket and NewEvent
    iResult = WSAEventSelect(ListenSocket, NewEvent, FD_ACCEPT | FD_CLOSE);
    if (iResult != 0) {
        wprintf(L"WSAEventSelect failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Start listening on the socket
    iResult = listen(ListenSocket, 10);
    if (iResult != 0) {
        wprintf(L"listen failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Add the socket and event to the arrays, increment number of events
    SocketArray[EventTotal] = ListenSocket;
    EventArray[EventTotal] = NewEvent;
    EventTotal++;

    // Wait for network events on all sockets
    Index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE);
    Index = Index - WSA_WAIT_EVENT_0;

    // Iterate through all events and enumerate
    // if the wait does not fail.
    for (i = Index; i < EventTotal; i++) {
        Index = WSAWaitForMultipleEvents(1, &EventArray[i], TRUE, 1000, FALSE);
        if ((Index != WSA_WAIT_FAILED) && (Index != WSA_WAIT_TIMEOUT)) {
            WSAEnumNetworkEvents(SocketArray[i], EventArray[i], &NetworkEvents);
        }
    }

    return 0;
}


int EnumWsaProtocols()
/*
https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumprotocolsa
https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumprotocolsw
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    int iResult = 0;
    int iError = 0;
    INT iNuminfo = 0;
    int i{};

    // Allocate a 16K buffer to retrieve all the protocol providers
    DWORD dwBufferLen = 16384;

    LPWSAPROTOCOL_INFO lpProtocolInfo = nullptr;

    // variables needed for converting provider GUID to a string
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    lpProtocolInfo = reinterpret_cast<LPWSAPROTOCOL_INFO>(MALLOC(dwBufferLen));
    if (lpProtocolInfo == nullptr) {
        wprintf(L"Memory allocation for providers buffer failed\n");
        WSACleanup();
        return 1;
    }

    iNuminfo = WSAEnumProtocols(nullptr, lpProtocolInfo, &dwBufferLen);
    if (iNuminfo == SOCKET_ERROR) {
        iError = WSAGetLastError();
        if (iError != WSAENOBUFS) {
            wprintf(L"WSAEnumProtocols failed with error: %d\n", iError);
            if (lpProtocolInfo) {
                FREE(lpProtocolInfo);
                lpProtocolInfo = nullptr;
            }
            WSACleanup();
            return 1;
        } else {
            wprintf(L"WSAEnumProtocols failed with error: WSAENOBUFS (%d)\n", iError);
            wprintf(L"  Increasing buffer size to %u\n\n", dwBufferLen);
            if (lpProtocolInfo) {
                FREE(lpProtocolInfo);
                lpProtocolInfo = nullptr;
            }
            lpProtocolInfo = reinterpret_cast<LPWSAPROTOCOL_INFO>(MALLOC(dwBufferLen));
            if (lpProtocolInfo == nullptr) {
                wprintf(L"Memory allocation increase for buffer failed\n");
                WSACleanup();
                return 1;
            }
            iNuminfo = WSAEnumProtocols(nullptr, lpProtocolInfo, &dwBufferLen);
            if (iNuminfo == SOCKET_ERROR) {
                iError = WSAGetLastError();
                wprintf(L"WSAEnumProtocols failed with error: %d\n", iError);
                if (lpProtocolInfo) {
                    FREE(lpProtocolInfo);
                    lpProtocolInfo = nullptr;
                }
                WSACleanup();
                return 1;
            }
        }
    }

    wprintf(L"WSAEnumProtocols succeeded with protocol count = %d\n\n", iNuminfo);
    for (i = 0; i < iNuminfo; i++) {
        wprintf(L"Winsock Catalog Provider Entry #%d\n", i);
        wprintf(L"----------------------------------------------------------\n");
        wprintf(L"Entry type:\t\t\t ");
        if (lpProtocolInfo[i].ProtocolChain.ChainLen == 1)
            wprintf(L"Base Service Provider\n");
        else
            wprintf(L"Layered Chain Entry\n");

        wprintf(L"Protocol:\t\t\t %ws\n", lpProtocolInfo[i].szProtocol);

        iRet = StringFromGUID2(lpProtocolInfo[i].ProviderId, reinterpret_cast<LPOLESTR>(&GuidString), 39);
        if (iRet == 0)
            wprintf(L"StringFromGUID2 failed\n");
        else
            wprintf(L"Provider ID:\t\t\t %ws\n", GuidString);

        wprintf(L"Catalog Entry ID:\t\t %u\n", lpProtocolInfo[i].dwCatalogEntryId);

        wprintf(L"Version:\t\t\t %d\n", lpProtocolInfo[i].iVersion);

        wprintf(L"Address Family:\t\t\t %d\n", lpProtocolInfo[i].iAddressFamily);
        wprintf(L"Max Socket Address Length:\t %d\n", lpProtocolInfo[i].iMaxSockAddr);
        wprintf(L"Min Socket Address Length:\t %d\n", lpProtocolInfo[i].iMinSockAddr);

        wprintf(L"Socket Type:\t\t\t %d\n", lpProtocolInfo[i].iSocketType);
        wprintf(L"Socket Protocol:\t\t %d\n", lpProtocolInfo[i].iProtocol);
        wprintf(L"Socket Protocol Max Offset:\t %d\n", lpProtocolInfo[i].iProtocolMaxOffset);

        wprintf(L"Network Byte Order:\t\t %d\n", lpProtocolInfo[i].iNetworkByteOrder);
        wprintf(L"Security Scheme:\t\t %d\n", lpProtocolInfo[i].iSecurityScheme);
        wprintf(L"Max Message Size:\t\t %u\n", lpProtocolInfo[i].dwMessageSize);

        wprintf(L"ServiceFlags1:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags1);
        wprintf(L"ServiceFlags2:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags2);
        wprintf(L"ServiceFlags3:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags3);
        wprintf(L"ServiceFlags4:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags4);
        wprintf(L"ProviderFlags:\t\t\t 0x%x\n", lpProtocolInfo[i].dwProviderFlags);

        wprintf(L"Protocol Chain length:\t\t %d\n", lpProtocolInfo[i].ProtocolChain.ChainLen);

        wprintf(L"\n");
    }

    if (lpProtocolInfo) {
        FREE(lpProtocolInfo);
        lpProtocolInfo = nullptr;
    }
    WSACleanup();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int EnumWscProtocols()
/*
https://docs.microsoft.com/en-us/windows/win32/api/ws2spi/nf-ws2spi-wscenumprotocols
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    int iResult = 0;
    INT iNuminfo = 0;
    int i{};

    // Allocate a 16K buffer to retrieve all the protocol providers
    DWORD dwBufferLen = 16384;
    LPWSAPROTOCOL_INFOW lpProtocolInfo = nullptr;
    int iErrno = 0;

    // variables needed for converting provider GUID to a string
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    lpProtocolInfo = (LPWSAPROTOCOL_INFOW)MALLOC(dwBufferLen);
    if (lpProtocolInfo == nullptr) {
        wprintf(L"Memory allocation for providers buffer failed\n");
        WSACleanup();
        return 1;
    }

    iNuminfo = WSCEnumProtocols(nullptr, lpProtocolInfo, &dwBufferLen, &iErrno);
    if (iNuminfo == SOCKET_ERROR) {
        if (iErrno != WSAENOBUFS) {
            wprintf(L"WSCEnumProtocols failed with error: %d\n", iErrno);
            if (lpProtocolInfo) {
                FREE(lpProtocolInfo);
                lpProtocolInfo = nullptr;
            }
            WSACleanup();
            return 1;
        } else {
            wprintf(L"WSCEnumProtocols failed with error: WSAENOBUFS (%d)\n", iErrno);
            wprintf(L"  Increasing buffer size to %u\n\n", dwBufferLen);
            if (lpProtocolInfo) {
                FREE(lpProtocolInfo);
                lpProtocolInfo = nullptr;
            }
            lpProtocolInfo = (LPWSAPROTOCOL_INFOW)MALLOC(dwBufferLen);
            if (lpProtocolInfo == nullptr) {
                wprintf(L"Memory allocation increase for buffer failed\n");
                WSACleanup();
                return 1;
            }
            iNuminfo = WSCEnumProtocols(nullptr, lpProtocolInfo, &dwBufferLen, &iErrno);
            if (iNuminfo == SOCKET_ERROR) {
                wprintf(L"WSCEnumProtocols failed with error: %d\n", iErrno);
                if (lpProtocolInfo) {
                    FREE(lpProtocolInfo);
                    lpProtocolInfo = nullptr;
                }
                WSACleanup();
                return 1;
            }
        }
    }

    wprintf(L"WSCEnumProtocols succeeded with protocol count = %d\n\n", iNuminfo);
    for (i = 0; i < iNuminfo; i++) {
        wprintf(L"Winsock Catalog Provider Entry #%d\n", i);
        wprintf(L"----------------------------------------------------------\n");
        wprintf(L"Entry type:\t\t\t ");
        if (lpProtocolInfo[i].ProtocolChain.ChainLen == 1)
            wprintf(L"Base Service Provider\n");
        else
            wprintf(L"Layered Chain Entry\n");

        wprintf(L"Protocol:\t\t\t %ws\n", lpProtocolInfo[i].szProtocol);

        iRet = StringFromGUID2(lpProtocolInfo[i].ProviderId, (LPOLESTR)&GuidString, 39);
        if (iRet == 0)
            wprintf(L"StringFromGUID2 failed\n");
        else
            wprintf(L"Provider ID:\t\t\t %ws\n", GuidString);

        WCHAR ProviderDllPath[MAX_PATH]{};
        INT ProviderDllPathLen = _ARRAYSIZE(ProviderDllPath);
        INT Errno;
        WSCGetProviderPath(&lpProtocolInfo[i].ProviderId, ProviderDllPath, &ProviderDllPathLen, &Errno);
        wprintf(L"ProviderDllPath:\t\t %ls\n", ProviderDllPath);

        wprintf(L"Catalog Entry ID:\t\t %u\n", lpProtocolInfo[i].dwCatalogEntryId);

        wprintf(L"Version:\t\t\t %d\n", lpProtocolInfo[i].iVersion);

        wprintf(L"Address Family:\t\t\t %d\n", lpProtocolInfo[i].iAddressFamily);
        wprintf(L"Max Socket Address Length:\t %d\n", lpProtocolInfo[i].iMaxSockAddr);
        wprintf(L"Min Socket Address Length:\t %d\n", lpProtocolInfo[i].iMinSockAddr);

        wprintf(L"Socket Type:\t\t\t %d\n", lpProtocolInfo[i].iSocketType);
        wprintf(L"Socket Protocol:\t\t %d\n", lpProtocolInfo[i].iProtocol);
        wprintf(L"Socket Protocol Max Offset:\t %d\n", lpProtocolInfo[i].iProtocolMaxOffset);

        wprintf(L"Network Byte Order:\t\t %d\n", lpProtocolInfo[i].iNetworkByteOrder);
        wprintf(L"Security Scheme:\t\t %d\n", lpProtocolInfo[i].iSecurityScheme);
        wprintf(L"Max Message Size:\t\t %u\n", lpProtocolInfo[i].dwMessageSize);

        wprintf(L"ServiceFlags1:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags1);
        wprintf(L"ServiceFlags2:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags2);
        wprintf(L"ServiceFlags3:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags3);
        wprintf(L"ServiceFlags4:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags4);
        wprintf(L"ProviderFlags:\t\t\t 0x%x\n", lpProtocolInfo[i].dwProviderFlags);

        wprintf(L"Protocol Chain length:\t\t %d\n", lpProtocolInfo[i].ProtocolChain.ChainLen);

        wprintf(L"\n");
    }

    if (lpProtocolInfo) {
        FREE(lpProtocolInfo);
        lpProtocolInfo = nullptr;
    }
    WSACleanup();
    return 0;
}


int EnumWscProtocols32()
/*
https://msdn.microsoft.com/zh-cn/ms742236
https://docs.microsoft.com/en-us/windows/win32/api/ws2spi/nf-ws2spi-wscenumprotocols32
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    int iResult = 0;
    INT iNuminfo = 0;
    int i{};

    // Allocate a 16K buffer to retrieve all the protocol providers
    DWORD dwBufferLen = 16384;
    LPWSAPROTOCOL_INFOW lpProtocolInfo = nullptr;
    int iErrno = 0;

    // variables needed for converting provider GUID to a string
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    lpProtocolInfo = (LPWSAPROTOCOL_INFOW)MALLOC(dwBufferLen);
    if (lpProtocolInfo == nullptr) {
        wprintf(L"Memory allocation for providers buffer failed\n");
        WSACleanup();
        return 1;
    }

#if (defined(_WIN64) && (_WIN32_WINNT >= 0x0501))
    iNuminfo = WSCEnumProtocols32(nullptr, lpProtocolInfo, &dwBufferLen, &iErrno);
    if (iNuminfo == SOCKET_ERROR) {
        if (iErrno != WSAENOBUFS) {
            wprintf(L"WSCEnumProtocols32 failed with error: %d\n", iErrno);
            if (lpProtocolInfo) {
                FREE(lpProtocolInfo);
                lpProtocolInfo = nullptr;
            }
            WSACleanup();
            return 1;
        } else {
            wprintf(L"WSCEnumProtocols32 failed with error: WSAENOBUFS (%d)\n", iErrno);
            wprintf(L"  Increasing buffer size to %u\n\n", dwBufferLen);
            if (lpProtocolInfo) {
                FREE(lpProtocolInfo);
                lpProtocolInfo = nullptr;
            }
            lpProtocolInfo = (LPWSAPROTOCOL_INFOW)MALLOC(dwBufferLen);
            if (lpProtocolInfo == nullptr) {
                wprintf(L"Memory allocation increase for buffer failed\n");
                WSACleanup();
                return 1;
            }
            iNuminfo = WSCEnumProtocols32(nullptr, lpProtocolInfo, &dwBufferLen, &iErrno);
            if (iNuminfo == SOCKET_ERROR) {
                wprintf(L"WSCEnumProtocols32 failed with error: %d\n", iErrno);
                if (lpProtocolInfo) {
                    FREE(lpProtocolInfo);
                    lpProtocolInfo = nullptr;
                }
                WSACleanup();
                return 1;
            }
        }
    }
#else if
    return 0;
#endif //(defined(_WIN64) && _WIN32_WINNT >= 0x0501)

    wprintf(L"WSCEnumProtocols32 succeeded with protocol count = %d\n\n", iNuminfo);
    for (i = 0; i < iNuminfo; i++) {
        wprintf(L"Winsock Catalog Provider Entry #%d\n", i);
        wprintf(L"----------------------------------------------------------\n");
        wprintf(L"Entry type:\t\t\t ");

    #pragma prefast(push)
    #pragma prefast(disable : 6385, "从“lpProtocolInfo”中读取的数据无效: 可读大小为“dwBufferLen”个字节，但可能读取了“1256”个字节。")
        if (lpProtocolInfo[i].ProtocolChain.ChainLen == 1) //MSDN写错了，少写个=。
            wprintf(L"Base Service Provider\n");
        else
            wprintf(L"Layered Chain Entry\n");
    #pragma prefast(pop)

        wprintf(L"Protocol:\t\t\t %ws\n", lpProtocolInfo[i].szProtocol);

        iRet = StringFromGUID2(lpProtocolInfo[i].ProviderId, (LPOLESTR)&GuidString, 39);
        if (iRet == 0)
            wprintf(L"StringFromGUID2 failed\n");
        else
            wprintf(L"Provider ID:\t\t\t %ws\n", GuidString);

        WCHAR ProviderDllPath[MAX_PATH]{};
        INT ProviderDllPathLen = _ARRAYSIZE(ProviderDllPath);
        INT Errno;
        WSCGetProviderPath32(&lpProtocolInfo[i].ProviderId, ProviderDllPath, &ProviderDllPathLen, &Errno);
        wprintf(L"ProviderDllPath:\t\t %ls\n", ProviderDllPath);

        wprintf(L"Catalog Entry ID:\t\t %u\n", lpProtocolInfo[i].dwCatalogEntryId);
        wprintf(L"Version:\t\t\t %d\n", lpProtocolInfo[i].iVersion);
        wprintf(L"Address Family:\t\t\t %d\n", lpProtocolInfo[i].iAddressFamily);
        wprintf(L"Max Socket Address Length:\t %d\n", lpProtocolInfo[i].iMaxSockAddr);
        wprintf(L"Min Socket Address Length:\t %d\n", lpProtocolInfo[i].iMinSockAddr);
        wprintf(L"Socket Type:\t\t\t %d\n", lpProtocolInfo[i].iSocketType);
        wprintf(L"Socket Protocol:\t\t %d\n", lpProtocolInfo[i].iProtocol);
        wprintf(L"Socket Protocol Max Offset:\t %d\n", lpProtocolInfo[i].iProtocolMaxOffset);
        wprintf(L"Network Byte Order:\t\t %d\n", lpProtocolInfo[i].iNetworkByteOrder);
        wprintf(L"Security Scheme:\t\t %d\n", lpProtocolInfo[i].iSecurityScheme);
        wprintf(L"Max Message Size:\t\t %u\n", lpProtocolInfo[i].dwMessageSize);
        wprintf(L"ServiceFlags1:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags1);
        wprintf(L"ServiceFlags2:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags2);
        wprintf(L"ServiceFlags3:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags3);
        wprintf(L"ServiceFlags4:\t\t\t 0x%x\n", lpProtocolInfo[i].dwServiceFlags4);
        wprintf(L"ProviderFlags:\t\t\t 0x%x\n", lpProtocolInfo[i].dwProviderFlags);
        wprintf(L"Protocol Chain length:\t\t %d\n", lpProtocolInfo[i].ProtocolChain.ChainLen);
        wprintf(L"\n");
    }

    if (lpProtocolInfo) {
        FREE(lpProtocolInfo);
        lpProtocolInfo = nullptr;
    }
    WSACleanup();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int EnumSpiInfo(int argc, wchar_t * argv[])
{
    int ret = ERROR_SUCCESS;

    if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumNameSpaceProviders")) == 0) {
        EnumNameSpaceProviders();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumNetworkEvents")) == 0) {
        EnumNetworkEvents();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWsaProtocols")) == 0) {
        EnumWsaProtocols();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWscProtocols")) == 0) {
        EnumWscProtocols();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWscProtocols32")) == 0) {
        EnumWscProtocols32();
    } else {
        //Usage(argv[0]);
    }

    return ret;
}
