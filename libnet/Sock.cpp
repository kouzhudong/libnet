#include "pch.h"
#include "Sock.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI getaddrinfo_test(int argc, char ** argv)
/*
The following code example shows how to use the getaddrinfo function.
https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo

最后一个参数不建议填写0，而是空字符串："".
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    INT iRetval{};
    DWORD dwRetval{};
    int i = 1;
    struct addrinfo * result = nullptr;
    struct addrinfo * ptr = nullptr;
    struct sockaddr_in * sockaddr_ipv4{};
    //    struct sockaddr_in6 *sockaddr_ipv6;
    LPSOCKADDR sockaddr_ip{};
    char ipstringbuffer[46]{};
    DWORD ipbufferlength = 46;

    if (argc != 3) { // Validate the parameters
        printf("usage: %s <hostname> <servicename>\n", argv[0]);
        printf("getaddrinfo provides protocol-independent translation\n");
        printf("   from an ANSI host name to an IP address\n");
        printf("%s example usage\n", argv[0]);
        printf("   %s www.contoso.com 0\n", argv[0]);
        return 1;
    }

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Setup the hints address info structure which is passed to the getaddrinfo() function
    addrinfo hints{};
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    printf("Calling getaddrinfo with following parameters:\n");
    printf("\tnodename = %s\n", argv[1]);
    printf("\tservname (or port) = %s\n\n", argv[2]);

    // Call getaddrinfo(). If the call succeeds, the result variable will hold a linked list
    // of addrinfo structures containing response information
    dwRetval = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (dwRetval != 0) {
        printf("getaddrinfo failed with error: %lu\n", dwRetval);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo returned success\n");

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        printf("getaddrinfo response %d\n", i++);
        printf("\tFlags: 0x%x\n", ptr->ai_flags);
        printf("\tFamily: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            printf("Unspecified\n");
            break;
        case AF_INET:
            printf("AF_INET (IPv4)\n");
            sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(ptr->ai_addr);
            printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr));
            break;
        case AF_INET6:
            printf("AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = reinterpret_cast<LPSOCKADDR>(ptr->ai_addr);
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            iRetval = WSAAddressToStringA(
                sockaddr_ip, static_cast<DWORD>(ptr->ai_addrlen), nullptr, ipstringbuffer, &ipbufferlength);
            if (iRetval)
                printf("WSAAddressToString failed with %d\n", WSAGetLastError());
            else
                printf("\tIPv6 address %s\n", ipstringbuffer);
            break;
        case AF_NETBIOS:
            printf("AF_NETBIOS (NetBIOS)\n");
            break;
        default:
            printf("Other %d\n", ptr->ai_family);
            break;
        }
        printf("\tSocket type: ");
        PrintSocketType(ptr->ai_socktype);
        printf("\tProtocol: ");
        PrintProtocol(static_cast<IPPROTO>(ptr->ai_protocol));
        printf("\tLength of this sockaddr: %zd\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
    }

    freeaddrinfo(result);
    WSACleanup();
    return 0;
}


int __cdecl getaddrinfo2(int argc, char ** argv)
/*
Example code using AI_NUMERICHOST

The following code example shows how to use the getaddrinfo function to convert a text string representation of
an IP address to an addrinfostructure that contains a sockaddr structure for the IP address and other information.

https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo

1.gethostbyaddr is no longer recommended for use as of Windows Sockets 2. Instead, use getnameinfo.
2.unix/linux上的书上也是这么说的。
所以...

made by correy
made at 2016.12.16
http://correy.webs.com
*/
{
    if (argc != 2) { // Validate the parameters
        printf("usage: %s <IP Address String>\n", argv[0]);
        printf("  getaddrinfo determines the IP binary network address\n");
        printf("       %s 207.46.197.32\n", argv[0]); /* www.contoso.com */
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Setup the hints address info structure which is passed to the getaddrinfo() function
    addrinfo hints{};
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    //    hints.ai_socktype = SOCK_STREAM;
    //    hints.ai_protocol = IPPROTO_TCP;

    // Call getaddrinfo().
    // If the call succeeds, the result variable will hold a linked list of addrinfo structures containing response
    // information
    struct addrinfo * result = nullptr;
    DWORD dwRetval = getaddrinfo( //感觉这个函数没啥大的意义。
        argv[1],                  //只能是数字，不能是名字。这也符合了函数的名字。
        nullptr,                  //可有可无，数字和名字无所谓。
        &hints,                   //必须有，而且4个成员必须为0.
        &result);
    if (dwRetval != 0) {
        printf("getaddrinfo failed with error: %lu\n", dwRetval);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo returned success\n");

    // Retrieve each address and print out the hex bytes
    int i = 1;
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        printf("getaddrinfo response %d\n", i++);
        printf("\tFlags: 0x%x\n", ptr->ai_flags);
        printf("\tFamily: ");
        PrintAddressFamily(static_cast<ADDRESS_FAMILY>(ptr->ai_family));
        printf("\tSocket type: ");
        PrintSocketType(ptr->ai_socktype);
        printf("\tProtocol: ");
        PrintProtocol(static_cast<IPPROTO>(ptr->ai_protocol));
        printf("\tLength of this sockaddr: %zd\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
    }

    freeaddrinfo(result);
    WSACleanup();
    return 0;
}


EXTERN_C
DLLEXPORT
int WINAPI GetAddrInfoW_test(int argc, wchar_t ** argv)
/*
The following code example shows how to use the GetAddrInfoW function.
https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfow

最后一个参数不建议填写0，而是空字符串："".
*/
{
    if (argc != 3) { // Validate the parameters
        wprintf(L"usage: %ws <hostname> <servicename>\n", argv[0]);
        wprintf(L"getaddrinfow provides protocol-independent translation\n");
        wprintf(L"   from an Unicode host name to an IP address\n");
        wprintf(L"%ws example usage\n", argv[0]);
        wprintf(L"   %ws www.contoso.com 0\n", argv[0]);
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Setup the hints address info structure which is passed to the getaddrinfo() function
    ADDRINFOW hints{};
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    wprintf(L"Calling getaddrinfow with following parameters:\n");
    wprintf(L"\tnodename = %ws\n", argv[1]);
    wprintf(L"\tservname (or port) = %ws\n\n", argv[2]);

    // Call GetAddrinfoW(). If the call succeeds,
    // the result variable will hold a linked list of addrinfow structures containing response information
    ADDRINFOW * result = nullptr;
    DWORD dwRetval = GetAddrInfoW(argv[1], argv[2], &hints, &result);
    if (dwRetval != 0) {
        wprintf(L"GetAddrInfoW failed with error: %u\n", dwRetval);
        WSACleanup();
        return 1;
    }

    wprintf(L"GetAddrInfoW returned success\n");

    // Declare and initialize variables
    INT iRetval{};
    int i = 1;

    //    struct sockaddr_in6 *sockaddr_ipv6;
    LPSOCKADDR sockaddr_ip{};
    wchar_t ipstringbuffer[46]{};
    DWORD ipbufferlength = 46;

    // Retrieve each address and print out the hex bytes
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        wprintf(L"GetAddrInfoW response %d\n", i++);
        wprintf(L"\tFlags: 0x%x\n", ptr->ai_flags);
        wprintf(L"\tFamily: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            wprintf(L"Unspecified\n");
            break;
        case AF_INET:
            wprintf(L"AF_INET (IPv4)\n");
            sockaddr_ip = reinterpret_cast<LPSOCKADDR>(ptr->ai_addr);
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            iRetval = WSAAddressToString(
                sockaddr_ip, static_cast<DWORD>(ptr->ai_addrlen), nullptr, ipstringbuffer, &ipbufferlength);
            if (iRetval)
                wprintf(L"WSAAddressToString failed with %d\n", WSAGetLastError());
            else
                wprintf(L"\tIPv4 address %ws\n", ipstringbuffer);
            break;
        case AF_INET6:
            wprintf(L"AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = reinterpret_cast<LPSOCKADDR>(ptr->ai_addr);
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            iRetval = WSAAddressToString(
                sockaddr_ip, static_cast<DWORD>(ptr->ai_addrlen), nullptr, ipstringbuffer, &ipbufferlength);
            if (iRetval)
                wprintf(L"WSAAddressToString failed with %d\n", WSAGetLastError());
            else
                wprintf(L"\tIPv6 address %ws\n", ipstringbuffer);
            break;
        default:
            wprintf(L"Other %ld\n", ptr->ai_family);
            break;
        }
        wprintf(L"\tSocket type: ");
        PrintSocketType(ptr->ai_socktype);
        wprintf(L"\tProtocol: ");
        PrintProtocol(static_cast<IPPROTO>(ptr->ai_protocol));
        wprintf(L"\tLength of this sockaddr: %zd\n", ptr->ai_addrlen);
        wprintf(L"\tCanonical name: %s\n", ptr->ai_canonname);
    }

    FreeAddrInfoW(result);
    WSACleanup();
    return 0;
}


int __cdecl GetAddrInfoExA(int argc, wchar_t ** argv)
/*
The following example demonstrates the use of the GetAddrInfoEx function.
https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfoexa
*/
{
    if (argc != 4) { // Validate the parameters
        wprintf(L"usage: %ws <hostname> <servicename> <namespace>\n", argv[0]);
        wprintf(L"getaddrinfoex provides protocol-independent translation\n");
        wprintf(L"   from a host name to an IP address\n");
        wprintf(L"%ws example usage\n", argv[0]);
        wprintf(L"   %ws www.contoso.com 0 12\n", argv[0]);
        wprintf(L"   looks up the www.contoso.com in the NS_DNS namespace\n"); //, argv[0] 这个原始的写错了。
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Setup the hints address info structure which is passed to the getaddrinfo() function
    ADDRINFOEX hints{};
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    DWORD dwNamespace = static_cast<DWORD>(_wtoi(argv[3]));

    wprintf(L"Calling GetAddrInfoEx with following parameters:\n");
    wprintf(L"\tName = %ws\n", argv[1]);
    wprintf(L"\tServiceName (or port) = %ws\n", argv[2]);
    wprintf(L"\tNamespace = %s (", argv[3]);
    PrintProviderNamespace(dwNamespace);
    wprintf(L")\n\n");

    // Declare and initialize variables
    DWORD dwRetval{};
    int i = 1;
    LPGUID lpNspid = nullptr;
    ADDRINFOEX * result = nullptr;
    ADDRINFOEX * ptr = nullptr;

    // LPSOCKADDR sockaddr_ip;
    struct sockaddr_in * sockaddr_ipv4{};
    struct sockaddr_in6 * sockaddr_ipv6{};

    // DWORD ipbufferlength = 46;
    wchar_t ipstringbuffer[46]{};

    // variables needed to print namespace provider GUID
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    // Call getaddrinfoex(). If the call succeeds,
    // the result variable will hold a linked list of addrinfo structures containing response information
    dwRetval =
        GetAddrInfoEx(argv[1], argv[2], dwNamespace, lpNspid, &hints, &result, nullptr, nullptr, nullptr, nullptr);
    if (dwRetval != 0) {
        wprintf(L"GetAddrInfoEx failed with error: %u\n", dwRetval);
        WSACleanup();
        return 1;
    }

    wprintf(L"GetAddrInfoEx returned success\n");

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        wprintf(L"GetAddrInfoEx response %d\n", i++);
        wprintf(L"\tFlags: 0x%x\n", ptr->ai_flags);
        wprintf(L"\tFamily: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            wprintf(L"Unspecified\n");
            break;
        case AF_INET:
            wprintf(L"AF_INET (IPv4)\n");
            // the InetNtop function is available on Windows Vista and later
            sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(ptr->ai_addr);
            wprintf(L"\tIPv4 address %ws\n", InetNtop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, 46));

            // We could also use the WSAAddressToString function
            // sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            // ipbufferlength = 46;
            // iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, nullptr, ipstringbuffer,
            // &ipbufferlength ); if (iRetval)
            //    wprintf(L"WSAAddressToString failed with %u\n", WSAGetLastError() );
            // else
            //    wprintf(L"\tIPv4 address %ws\n", ipstringbuffer);
            break;
        case AF_INET6:
            wprintf(L"AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(ptr->ai_addr);
            wprintf(L"\tIPv6 address %ws\n", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46));

            // We could also use WSAAddressToString which also returns the scope ID
            // sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            // ipbufferlength = 46;
            // iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, nullptr, ipstringbuffer,
            // &ipbufferlength ); if (iRetval)
            //    wprintf(L"WSAAddressToString failed with %u\n", WSAGetLastError() );
            // else
            //    wprintf(L"\tIPv6 address %ws\n", ipstringbuffer);
            break;
        default:
            wprintf(L"Other %ld\n", ptr->ai_family);
            break;
        }
        wprintf(L"\tSocket type: ");
        PrintSocketType(ptr->ai_socktype);
        wprintf(L"\tProtocol: ");
        PrintProtocol(static_cast<IPPROTO>(ptr->ai_protocol));
        wprintf(L"\tLength of this sockaddr: %zd\n", ptr->ai_addrlen);
        wprintf(L"\tCanonical name: %s\n", ptr->ai_canonname);

        if (ptr->ai_blob == nullptr)
            wprintf(L"\tBlob: (nullptr)\n");
        else
            wprintf(L"\tLength of the blob: %u\n", static_cast<DWORD>(ptr->ai_bloblen));

        if (ptr->ai_provider == nullptr)
            wprintf(L"\tNamespace provider GUID: (null)\n");
        else {
            iRet = StringFromGUID2(*(ptr->ai_provider), reinterpret_cast<LPOLESTR>(&GuidString), 39);
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&ptr.ai_provider, (LPOLESTR) &GuidString, 39);
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"\tNamespace provider: %ws\n", GuidString);
            }
        }
    }

    FreeAddrInfoEx(result);
    WSACleanup();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


//    This sample demonstrates how to use asynchronous GetAddrInfoEx to resolve a name to an IP address.
//
//    ResolveName <QueryName>


VOID WINAPI QueryCompleteCallback(_In_ DWORD Error, _In_ DWORD Bytes, _In_ LPOVERLAPPED Overlapped);


int __cdecl GetAddrInfoExA2(_In_ int Argc, PWCHAR Argv[])
/*
https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfoexa

The following example demonstrates how to use the GetAddrInfoEx function asynchronous to resolve a name to an IP
address..
*/
{
    INT Error = ERROR_SUCCESS;
    WSADATA wsaData{};
    BOOL IsWSAStartupCalled = FALSE;
    ADDRINFOEX Hints{};
    QUERY_CONTEXT QueryContext{};
    HANDLE CancelHandle = nullptr;
    DWORD QueryTimeout = 5 * 1000; // 5 seconds

    if (Argc != 2) { //  Validate the parameters
        wprintf(L"Usage: ResolveName <QueryName>\n");
        goto exit;
    }

    Error = WSAStartup(MAKEWORD(2, 2), &wsaData); //  All Winsock functions require WSAStartup() to be called first
    if (Error != 0) {
        wprintf(L"WSAStartup failed with %d\n", Error);
        goto exit;
    }

    IsWSAStartupCalled = TRUE;

    ZeroMemory(&Hints, sizeof(Hints));
    Hints.ai_family = AF_UNSPEC;

    //  Note that this is a simple sample that waits/cancels a single
    //  asynchronous query. The reader may extend this to support multiple asynchronous queries.
    QueryContext.CompleteEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (QueryContext.CompleteEvent == nullptr) {
        Error = GetLastError();
        wprintf(L"Failed to create completion event: Error %d\n", Error);
        goto exit;
    }

    //  Initiate asynchronous GetAddrInfoExW.
    //
    //  Note GetAddrInfoEx can also be invoked asynchronously using an event
    //  in the overlapped object (Just set hEvent in the Overlapped object and set NULL as completion callback.)
    //
    //  This sample uses the completion callback method.

    Error = GetAddrInfoExW(Argv[1],
                           nullptr,
                           NS_DNS,
                           nullptr,
                           &Hints,
                           &QueryContext.QueryResults,
                           nullptr,
                           &QueryContext.QueryOverlapped,
                           QueryCompleteCallback,
                           &CancelHandle);

    //  If GetAddrInfoExW() returns  WSA_IO_PENDING, GetAddrInfoExW will invoke the completion routine.
    //  If GetAddrInfoExW returned anything else we must invoke the completion directly.
    if (Error != WSA_IO_PENDING) {
        QueryCompleteCallback(Error, 0, &QueryContext.QueryOverlapped);
        goto exit;
    }

    //  Wait for query completion for 5 seconds and cancel the query if it has not yet completed.
    if (WaitForSingleObject(QueryContext.CompleteEvent, QueryTimeout) == WAIT_TIMEOUT) {
        //  Cancel the query: Note that the GetAddrInfoExCancelcancel call does
        //  not block, so we must wait for the completion routine to be invoked.
        //  If we fail to wait, WSACleanup() could be called while an
        //  asynchronous query is still in progress, possibly causing a crash.

        wprintf(L"The query took longer than %u seconds to complete; cancelling the query...\n",
                QueryTimeout / 1000);

        GetAddrInfoExCancel(&CancelHandle);

        WaitForSingleObject(QueryContext.CompleteEvent, INFINITE);
    }

exit:

    if (IsWSAStartupCalled) {
        WSACleanup();
    }

    if (QueryContext.CompleteEvent) {
        CloseHandle(QueryContext.CompleteEvent);
    }

    return Error;
}


VOID WINAPI QueryCompleteCallback(_In_ DWORD Error, _In_ DWORD Bytes, _In_ LPOVERLAPPED Overlapped)
// Callback function called by Winsock as part of asynchronous query complete
{
    PQUERY_CONTEXT QueryContext = nullptr;
    PADDRINFOEX QueryResults = nullptr;
    WCHAR AddrString[MAX_ADDRESS_STRING_LENGTH]{};
    DWORD AddressStringLength{};

    UNREFERENCED_PARAMETER(Bytes);

    QueryContext = CONTAINING_RECORD(Overlapped, QUERY_CONTEXT, QueryOverlapped);
    if (Error != ERROR_SUCCESS) {
        wprintf(L"ResolveName failed with %u\n", Error);
        goto exit;
    }

    wprintf(L"ResolveName succeeded. Query Results:\n");

    QueryResults = QueryContext->QueryResults;

    while (QueryResults) {
        AddressStringLength = MAX_ADDRESS_STRING_LENGTH;

        WSAAddressToString(QueryResults->ai_addr,
                           static_cast<DWORD>(QueryResults->ai_addrlen),
                           nullptr,
                           AddrString,
                           &AddressStringLength);

        wprintf(L"Ip Address: %s\n", AddrString);
        QueryResults = QueryResults->ai_next;
    }

exit:
    if (QueryContext->QueryResults) {
        FreeAddrInfoEx(QueryContext->QueryResults);
    }

    SetEvent(QueryContext->CompleteEvent); //  Notify caller that the query completed
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int __cdecl GetAddrInfoExW(int argc, wchar_t ** argv)
/*
The following example demonstrates the use of the GetAddrInfoEx function.

https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfoexw
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    int iResult{};
    DWORD dwRetval{};
    int i = 1;
    DWORD dwNamespace = NS_ALL;
    LPGUID lpNspid = nullptr;
    ADDRINFOEX * result = nullptr;
    ADDRINFOEX * ptr = nullptr;
    ADDRINFOEX hints{};

    // LPSOCKADDR sockaddr_ip;
    struct sockaddr_in * sockaddr_ipv4{};
    struct sockaddr_in6 * sockaddr_ipv6{};

    // DWORD ipbufferlength = 46;
    wchar_t ipstringbuffer[46]{};

    // variables needed to print namespace provider GUID
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    if (argc != 4) { // Validate the parameters
        wprintf(L"usage: %ws <hostname> <servicename> <namespace>\n", argv[0]);
        wprintf(L"getaddrinfoex provides protocol-independent translation\n");
        wprintf(L"   from a host name to an IP address\n");
        wprintf(L"%ws example usage\n", argv[0]);
        wprintf(L"   %ws www.contoso.com 0 12\n", argv[0]);
        wprintf(L"   %ws looks up the www.contoso.com in the NS_DNS namespace\n", argv[0]); //原始代码错误。
        return 1;
    }

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    dwNamespace = static_cast<DWORD>(_wtoi(argv[3]));

    wprintf(L"Calling GetAddrInfoEx with following parameters:\n");
    wprintf(L"\tName = %ws\n", argv[1]);
    wprintf(L"\tServiceName (or port) = %ws\n", argv[2]);
    wprintf(L"\tNamespace = %s (", argv[3]);
    PrintProviderNamespace(dwNamespace);
    wprintf(L")\n\n");

    // Call getaddrinfoex(). If the call succeeds,
    // the result variable will hold a linked list of addrinfo structures containing response information
    dwRetval =
        GetAddrInfoEx(argv[1], argv[2], dwNamespace, lpNspid, &hints, &result, nullptr, nullptr, nullptr, nullptr);
    if (dwRetval != 0) {
        wprintf(L"GetAddrInfoEx failed with error: %u\n", dwRetval);
        WSACleanup();
        return 1;
    }

    wprintf(L"GetAddrInfoEx returned success\n");

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        wprintf(L"GetAddrInfoEx response %d\n", i++);
        wprintf(L"\tFlags: 0x%x\n", ptr->ai_flags);
        wprintf(L"\tFamily: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            wprintf(L"Unspecified\n");
            break;
        case AF_INET:
            wprintf(L"AF_INET (IPv4)\n");
            // the InetNtop function is available on Windows Vista and later
            sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(ptr->ai_addr);
            wprintf(L"\tIPv4 address %ws\n", InetNtop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, 46));

            // We could also use the WSAAddressToString function
            // sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            // ipbufferlength = 46;
            // iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, nullptr, ipstringbuffer,
            // &ipbufferlength ); if (iRetval)
            //    wprintf(L"WSAAddressToString failed with %u\n", WSAGetLastError() );
            // else
            //    wprintf(L"\tIPv4 address %ws\n", ipstringbuffer);
            break;
        case AF_INET6:
            wprintf(L"AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(ptr->ai_addr);
            wprintf(L"\tIPv6 address %ws\n", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46));

            // We could also use WSAAddressToString which also returns the scope ID
            // sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            // ipbufferlength = 46;
            // iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, nullptr, ipstringbuffer,
            // &ipbufferlength ); if (iRetval)
            //    wprintf(L"WSAAddressToString failed with %u\n", WSAGetLastError() );
            // else
            //    wprintf(L"\tIPv6 address %ws\n", ipstringbuffer);
            break;
        default:
            wprintf(L"Other %ld\n", ptr->ai_family);
            break;
        }
        wprintf(L"\tSocket type: ");
        PrintSocketType(ptr->ai_socktype);
        wprintf(L"\tProtocol: ");
        PrintProtocol(static_cast<IPPROTO>(ptr->ai_protocol));
        wprintf(L"\tLength of this sockaddr: %zd\n", ptr->ai_addrlen);
        wprintf(L"\tCanonical name: %s\n", ptr->ai_canonname);

        if (ptr->ai_blob == nullptr)
            wprintf(L"\tBlob: (null)\n");
        else
            wprintf(L"\tLength of the blob: %u\n", static_cast<DWORD>(ptr->ai_bloblen));

        if (ptr->ai_provider == nullptr)
            wprintf(L"\tNamespace provider GUID: (null)\n");
        else {
            iRet = StringFromGUID2(*(ptr->ai_provider), reinterpret_cast<LPOLESTR>(&GuidString), 39);
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&ptr.ai_provider, (LPOLESTR) &GuidString, 39);
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"\tNamespace provider: %ws\n", GuidString);
            }
        }
    }

    FreeAddrInfoEx(result);
    WSACleanup();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


//    This sample demonstrates how to use asynchronous GetAddrInfoEx to resolve a name to an IP address.
//
//    ResolveName <QueryName>


VOID WINAPI QueryCompleteCallback2(_In_ DWORD Error, _In_ DWORD Bytes, _In_ LPOVERLAPPED Overlapped);


int __cdecl GetAddrInfoExW2(_In_ int Argc, PWCHAR Argv[])
/*
The following example demonstrates how to use the GetAddrInfoEx function asynchronous to resolve a name to an IP
address..

https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfoexw
*/
{
    INT Error = ERROR_SUCCESS;
    WSADATA wsaData{};
    BOOL IsWSAStartupCalled = FALSE;
    ADDRINFOEX Hints{};
    QUERY_CONTEXT QueryContext{};
    HANDLE CancelHandle = nullptr;
    DWORD QueryTimeout = 5 * 1000; // 5 seconds

    ZeroMemory(&QueryContext, sizeof(QueryContext));

    if (Argc != 2) { //  Validate the parameters
        wprintf(L"Usage: ResolveName <QueryName>\n");
        goto exit;
    }

    Error = WSAStartup(MAKEWORD(2, 2), &wsaData); //  All Winsock functions require WSAStartup() to be called first
    if (Error != 0) {
        wprintf(L"WSAStartup failed with %d\n", Error);
        goto exit;
    }

    IsWSAStartupCalled = TRUE;

    ZeroMemory(&Hints, sizeof(Hints));
    Hints.ai_family = AF_UNSPEC;

    //  Note that this is a simple sample that waits/cancels a single asynchronous query.
    //  The reader may extend this to support multiple asynchronous queries.
    QueryContext.CompleteEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (QueryContext.CompleteEvent == nullptr) {
        Error = GetLastError();
        wprintf(L"Failed to create completion event: Error %d\n", Error);
        goto exit;
    }

    //  Initiate asynchronous GetAddrInfoExW.
    //
    //  Note GetAddrInfoEx can also be invoked asynchronously using an event
    //  in the overlapped object (Just set hEvent in the Overlapped object and set NULL as completion callback.)
    //
    //  This sample uses the completion callback method.
    Error = GetAddrInfoExW(Argv[1],
                           nullptr,
                           NS_DNS,
                           nullptr,
                           &Hints,
                           &QueryContext.QueryResults,
                           nullptr,
                           &QueryContext.QueryOverlapped,
                           QueryCompleteCallback2,
                           &CancelHandle);

    //  If GetAddrInfoExW() returns  WSA_IO_PENDING, GetAddrInfoExW will invoke the completion routine.
    //  If GetAddrInfoExW returned anything else we must invoke the completion directly.
    if (Error != WSA_IO_PENDING) {
        QueryCompleteCallback2(Error, 0, &QueryContext.QueryOverlapped);
        goto exit;
    }

    //  Wait for query completion for 5 seconds and cancel the query if it has not yet completed.
    if (WaitForSingleObject(QueryContext.CompleteEvent, QueryTimeout) == WAIT_TIMEOUT) {
        //  Cancel the query: Note that the GetAddrInfoExCancelcancel call does
        //  not block, so we must wait for the completion routine to be invoked.
        //  If we fail to wait, WSACleanup() could be called while an
        //  asynchronous query is still in progress, possibly causing a crash.

        wprintf(L"The query took longer than %u seconds to complete; cancelling the query...\n",
                QueryTimeout / 1000);

        GetAddrInfoExCancel(&CancelHandle);

        WaitForSingleObject(QueryContext.CompleteEvent, INFINITE);
    }

exit:

    if (IsWSAStartupCalled) {
        WSACleanup();
    }

    if (QueryContext.CompleteEvent) {
        CloseHandle(QueryContext.CompleteEvent);
    }

    return Error;
}


VOID WINAPI QueryCompleteCallback2(_In_ DWORD Error, _In_ DWORD Bytes, _In_ LPOVERLAPPED Overlapped)
// Callback function called by Winsock as part of asynchronous query complete
{
    PQUERY_CONTEXT QueryContext = nullptr;
    PADDRINFOEX QueryResults = nullptr;
    WCHAR AddrString[MAX_ADDRESS_STRING_LENGTH]{};
    DWORD AddressStringLength{};

    UNREFERENCED_PARAMETER(Bytes);

    QueryContext = CONTAINING_RECORD(Overlapped, QUERY_CONTEXT, QueryOverlapped);
    if (Error != ERROR_SUCCESS) {
        wprintf(L"ResolveName failed with %u\n", Error);
        goto exit;
    }

    wprintf(L"ResolveName succeeded. Query Results:\n");

    QueryResults = QueryContext->QueryResults;

    while (QueryResults) {
        AddressStringLength = MAX_ADDRESS_STRING_LENGTH;

        WSAAddressToString(QueryResults->ai_addr,
                           static_cast<DWORD>(QueryResults->ai_addrlen),
                           nullptr,
                           AddrString,
                           &AddressStringLength);

        wprintf(L"Ip Address: %s\n", AddrString);
        QueryResults = QueryResults->ai_next;
    }

exit:
    if (QueryContext->QueryResults) {
        FreeAddrInfoEx(QueryContext->QueryResults);
    }

    SetEvent(QueryContext->CompleteEvent); //  Notify caller that the query completed
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int __cdecl getnameinfo(int argc, char ** argv)
/*
The following code example shows how to use the getnameinfo function.

https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getnameinfo

1.Note  The gethostbyname function has been deprecated by the introduction of the getaddrinfo function.
Developers creating Windows Sockets 2 applications are urged to use the getaddrinfo function instead of
gethostbyname. 2.unix/linux上的书上也是这么说的。 所以... 3.注意：不是每个IP都有域名的。

made by correy
made at 2016.12.16
http://correy.webs.com
*/
{
    // Declare and initialize variables
    char hostname[NI_MAXHOST]{};
    char servInfo[NI_MAXSERV]{};
    u_short port = 80;

    if (argc != 2) { // Validate the parameters
        printf("usage: %s IPv4 address\n", argv[0]);
        printf("  to return hostname\n");
        printf("       %s 127.0.0.1\n", argv[0]);
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Set up sockaddr_in structure which is passed to the getnameinfo function
    sockaddr_in saGNI{};
    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(argv[1]);
    saGNI.sin_port = htons(port);
    DWORD dwRetval = getnameinfo( //这个函数运行很慢，以分钟来计时单位。
        reinterpret_cast<struct sockaddr *>(&saGNI),
        sizeof(struct sockaddr),
        hostname, //这个不会变。
        NI_MAXHOST,
        servInfo, //这个可以变。参见：%WINDIR%\system32\drivers\etc\services，感觉没啥大的意义。
        NI_MAXSERV,
        NI_NOFQDN //取这个值，方显函数的意义。
    );
    if (dwRetval != 0) {
        printf("failed with error # %d\n", WSAGetLastError());
        return 1;
    } else {
        printf("returned hostname = %s\n", hostname);
        return 0;
    }
}


int __cdecl GetNameInfoW(int argc, char ** argv)
/*
The following example demonstrates the use of the GetNameInfoW function.

https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getnameinfow
*/
{
    // Declare and initialize variables
    WCHAR hostname[NI_MAXHOST]{};
    WCHAR servInfo[NI_MAXSERV]{};
    u_short port = 27015;

    if (argc != 2) { // Validate the parameters
        wprintf(L"usage: %hs IPv4 address\n", argv[0]);
        wprintf(L"  to return hostname\n");
        wprintf(L"       %hs 127.0.0.1\n", argv[0]);
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Set up sockaddr_in structure which is passed to the getnameinfo function
    sockaddr_in saGNI{};
    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(argv[1]);
    saGNI.sin_port = htons(port);
    DWORD dwRetval = GetNameInfoW(reinterpret_cast<struct sockaddr *>(&saGNI),
                                  sizeof(struct sockaddr),
                                  hostname,
                                  NI_MAXHOST,
                                  servInfo,
                                  NI_MAXSERV,
                                  NI_NUMERICSERV);
    if (dwRetval != 0) {
        wprintf(L"failed with error # %ld\n", WSAGetLastError());
        return 1;
    } else {
        wprintf(L"returned hostname = %ws\n", hostname);
        return 0;
    }
}


int gethostbyaddr(int argc, char ** argv)
/*
说明：根据IP获取名字。

尽管：[gethostbyaddr is no longer recommended for use as of Windows Sockets 2. Instead, use getnameinfo.]

说明：不是每个IP都有主机名的。

made at 2017.02.23

The following example demonstrates the use of the gethostbyaddr function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-gethostbyaddr
*/
{
    // Declare and initialize variables
    DWORD dwError{};
    int i = 0;
    int bIpv6 = 0;
    struct hostent * remoteHost{};
    struct in_addr addr{};
    IN6_ADDR addr6{};
    char ** pAlias{};

    if (argc < 2) { // Validate the parameters
        printf("usage: %s 4 ipv4address\n", argv[0]);
        printf(" or\n");
        printf("usage: %s 6 ipv6address\n", argv[0]);
        printf("  to return the hostname\n");
        printf("       %s 4 127.0.0.1\n", argv[0]);
        printf("       %s 6 0::1\n", argv[0]);
        return 1;
    }

    // Validate parameters
    if (atoi(argv[1]) == 4)
        bIpv6 = 0;
    else if (atoi(argv[1]) == 6)
        bIpv6 = 1;
    else {
        printf("usage: %s 4 ipv4address\n", argv[0]);
        printf(" or\n");
        printf("usage: %s 6 ipv6address\n", argv[0]);
        printf("  to return the hostname\n");
        printf("       %s 4 127.0.0.1\n", argv[0]);
        printf("       %s 6 0::1\n", argv[0]);
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    char * host_addr = argv[2];

    printf("Calling gethostbyaddr with %s\n", host_addr);
    if (bIpv6 == 1) {
        iResult = inet_pton(AF_INET6, host_addr, &addr6);
        if (iResult == 0) {
            printf("The IPv6 address entered must be a legal address\n");
            return 1;
        } else
            remoteHost = gethostbyaddr(reinterpret_cast<char *>(&addr6), 16, AF_INET6);
    } else {
        addr.s_addr = inet_addr(host_addr);
        if (addr.s_addr == INADDR_NONE) {
            printf("The IPv4 address entered must be a legal address\n");
            return 1;
        } else
            remoteHost = gethostbyaddr(reinterpret_cast<char *>(&addr), 4, AF_INET);
    }

    if (remoteHost == nullptr) {
        dwError = WSAGetLastError();
        if (dwError != 0) {
            if (dwError == WSAHOST_NOT_FOUND) {
                printf("Host not found\n");
                return 1;
            } else if (dwError == WSANO_DATA) {
                printf("No data record found\n");
                return 1;
            } else {
                printf("Function failed with error: %lu\n", dwError);
                return 1;
            }
        }
    } else {
        printf("Function returned:\n");
        printf("\tOfficial name: %s\n", remoteHost->h_name);
        for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
            printf("\tAlternate name #%d: %s\n", ++i, *pAlias);
        }
        printf("\tAddress type: ");
        PrintAddressFamily(remoteHost->h_addrtype);
        printf("\tAddress length: %d\n", remoteHost->h_length);

        if (remoteHost->h_addrtype == AF_INET) {
            while (remoteHost->h_addr_list[i] != 0) {
                addr.s_addr = *reinterpret_cast<u_long *>(remoteHost->h_addr_list[i++]);
                printf("\tIPv4 Address #%d: %s\n", i, inet_ntoa(addr));
            }
        } else if (remoteHost->h_addrtype == AF_INET6)
            printf("\tRemotehost is an IPv6 address\n");
    }

    return 0;
}


int gethostbyname(int argc, char ** argv)
/*
The following examples demonstrates the use of the gethostbyname function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-gethostbyname
*/
{
    if (argc != 2) { // Validate the parameters
        printf("usage: %s hostname\n", argv[0]);
        printf("  to return the IP addresses for the host\n");
        printf("       %s www.contoso.com\n", argv[0]);
        printf(" or\n");
        printf("       %s IPv4string\n", argv[0]);
        printf("  to return an IPv4 binary address for an IPv4string\n");
        printf("       %s 127.0.0.1\n", argv[0]);
        return 1;
    }

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    auto host_name = argv[1];
    printf("Calling gethostbyname with %s\n", host_name);
    auto remoteHost = gethostbyname(host_name);
    if (remoteHost == nullptr) {
        DWORD dwError = WSAGetLastError();
        if (dwError != 0) {
            if (dwError == WSAHOST_NOT_FOUND) {
                printf("Host not found\n");
                return 1;
            } else if (dwError == WSANO_DATA) {
                printf("No data record found\n");
                return 1;
            } else {
                printf("Function failed with error: %lu\n", dwError);
                return 1;
            }
        }
    } else {
        int i = 0;
        printf("Function returned:\n");
        printf("\tOfficial name: %s\n", remoteHost->h_name);
        for (auto pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
            printf("\tAlternate name #%d: %s\n", ++i, *pAlias);
        }
        printf("\tAddress type: ");
        PrintAddressFamily(remoteHost->h_addrtype);
        printf("\tAddress length: %d\n", remoteHost->h_length);

        i = 0;
        if (remoteHost->h_addrtype == AF_INET) {
            in_addr addr{};
            while (remoteHost->h_addr_list[i] != 0) {
                addr.s_addr = *reinterpret_cast<u_long *>(remoteHost->h_addr_list[i++]);
                printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
            }
        } else if (remoteHost->h_addrtype == AF_NETBIOS) {
            printf("NETBIOS address was returned\n");
        }
    }

    return 0;
}
