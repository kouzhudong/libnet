#include "pch.h"
#include "IOCTL.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void getsockopt1()
/*
Example Code
The following code sample demonstrates the use of the getsockopt function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
*/
{
    // Declare variables
    WSADATA wsaData;
    SOCKET ListenSocket;
    sockaddr_in service;

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
        printf("Error at WSAStartup\n");

    // Create a listening socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket()\n");
        WSACleanup();
        return;
    }

    // Bind the socket to the local IP address and port 27015
    hostent * thisHost;
    char * ip;
    u_short port;
    port = 27015;
    thisHost = gethostbyname("");
    ip = inet_ntoa(*(struct in_addr *)*thisHost->h_addr_list);

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(port);
    if (bind(ListenSocket, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR) {
        printf("bind failed\n");
        closesocket(ListenSocket);
        return;
    }

    // Initialize variables and call getsockopt. 
    // The SO_ACCEPTCONN parameter is a socket option 
    // that tells the function to check whether the 
    // socket has been put in listening mode or not. 
    // The various socket options return different
    // information about the socket. This call should
    // return 0 to the optVal parameter, since the socket is not in listening mode.
    int optVal;
    int optLen = sizeof(int);
    if (getsockopt(ListenSocket, SOL_SOCKET, SO_ACCEPTCONN, (char *)&optVal, &optLen) != SOCKET_ERROR)
        printf("SockOpt Value: %ld\n", optVal);

    // Put the listening socket in listening mode.
    if (listen(ListenSocket, 100) == SOCKET_ERROR) {
        printf("error listening\n");
    }

    // Call getsockopt again to verify that the socket is in listening mode.
    if (getsockopt(ListenSocket, SOL_SOCKET, SO_ACCEPTCONN, (char *)&optVal, &optLen) != SOCKET_ERROR)
        printf("SockOpt Value: %ld\n", optVal);

    WSACleanup();
    return;
}


int getsockopt2()
/*
Example Code

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
*/
{
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;
    int i;
    DWORD dwError;
    SOCKET Sock = INVALID_SOCKET;

    #define DEVICE_LIST_LEN    10

    SOCKADDR_IRDA DestSockAddr = {AF_IRDA, 0, 0, 0, 0, "SampleIrDAService"};

    unsigned char DevListBuff[sizeof(DEVICELIST) -
        sizeof(IRDA_DEVICE_INFO) +
        (sizeof(IRDA_DEVICE_INFO) * DEVICE_LIST_LEN)];

    int DevListLen = sizeof(DevListBuff);
    PDEVICELIST pDevList;
    pDevList = (PDEVICELIST)&DevListBuff;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    Sock = socket(AF_IRDA, SOCK_STREAM, 0);
    if (Sock == INVALID_SOCKET) {
        dwError = WSAGetLastError();
        printf("socket failed trying to create an AF_IRDA socket with error %d\n", dwError);
        if (dwError == WSAEAFNOSUPPORT) {
            printf("Check that the local computer has an infrared device\n");
            printf("and a device driver is installed for the infrared device\n");
        }
        WSACleanup();
        return 1;
    }

    // Sock is not in connected state
    iResult = getsockopt(Sock, SOL_IRLMP, IRLMP_ENUMDEVICES, (char *)pDevList, &DevListLen);
    if (iResult == SOCKET_ERROR) {
        printf("getsockopt failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (pDevList->numDevice == 0) {
        // no devices discovered or cached
        // not a bad idea to run a couple of times
        printf("No IRDA devices were discovered or cached\n");
    } else {
        // one per discovered device
        for (i = 0; i < (int)pDevList->numDevice; i++) {
            // typedef struct _IRDA_DEVICE_INFO
            // {
            //     u_char    irdaDeviceID[4];
            //     char      irdaDeviceName[22];
            //     u_char    irdaDeviceHints1;
            //     u_char    irdaDeviceHints2;
            //     u_char    irdaCharSet;
            // } _IRDA_DEVICE_INFO;

            // pDevList->Device[i]. see _IRDA_DEVICE_INFO for fields
            // display the device names and let the user select one
        }
    }

    // assume the user selected the first device [0]
    memcpy(&DestSockAddr.irdaDeviceID[0], &pDevList->Device[0].irdaDeviceID[0], 4);

    iResult = connect(Sock, (const struct sockaddr *)&DestSockAddr, sizeof(SOCKADDR_IRDA));
    if (iResult == SOCKET_ERROR) {
        printf("connect failed with error %d\n", WSAGetLastError());
    } else
        printf("connect to first IRDA device was successful\n");

    WSACleanup();
    return 0;
}


void getsockopt3()
/*
功能：获取套接字的一些信息。
参考：https://msdn.microsoft.com/en-us/library/windows/desktop/ms738544(v=vs.85).aspx
*/
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);// Initialize Winsock
    if (iResult != NO_ERROR)
        printf("Error at WSAStartup\n");

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// Create a socket
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket()\n");
        WSACleanup();
        return;
    }

    int optVal;
    int optLen = sizeof(int);
    if (getsockopt(ListenSocket, SOL_SOCKET, SO_RCVBUF, (char *)&optVal, &optLen) != SOCKET_ERROR)
        printf("SO_RCVBUF Value: %ld\n", optVal);

    if (getsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, &optLen) != SOCKET_ERROR)
        printf("SO_SNDBUF Value: %ld\n", optVal);

    WSACleanup();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int setsockopt1()
/*
Example Code
The following example demonstrates the setsockopt function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
*/
{
    // Declare variables
    WSADATA wsaData;
    SOCKET ListenSocket;
    sockaddr_in service;
    int iResult = 0;
    BOOL bOptVal = FALSE;
    int bOptLen = sizeof(BOOL);
    int iOptVal = 0;
    int iOptLen = sizeof(int);

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"Error at WSAStartup()\n");
        return 1;
    }

    // Create a listening socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind the socket to the local IP address and port 27015
    hostent * thisHost;
    char * ip;
    u_short port;
    port = 27015;
    thisHost = gethostbyname("");
    ip = inet_ntoa(*(struct in_addr *)*thisHost->h_addr_list);

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(port);
    iResult = bind(ListenSocket, (SOCKADDR *)&service, sizeof(service));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Initialize variables and call setsockopt. 
    // The SO_KEEPALIVE parameter is a socket option 
    // that makes the socket send keepalive messages
    // on the session. The SO_KEEPALIVE socket option
    // requires a boolean value to be passed to the
    // setsockopt function. If TRUE, the socket is
    // configured to send keepalive messages, if FALSE
    // the socket configured to NOT send keepalive messages.
    // This section of code tests the setsockopt function
    // by checking the status of SO_KEEPALIVE on the socket using the getsockopt function.

    bOptVal = TRUE;
    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&iOptVal, &iOptLen);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"getsockopt for SO_KEEPALIVE failed with error: %u\n", WSAGetLastError());
    } else
        wprintf(L"SO_KEEPALIVE Value: %ld\n", iOptVal);

    iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&bOptVal, bOptLen);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"setsockopt for SO_KEEPALIVE failed with error: %u\n", WSAGetLastError());
    } else
        wprintf(L"Set SO_KEEPALIVE: ON\n");

    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&iOptVal, &iOptLen);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"getsockopt for SO_KEEPALIVE failed with error: %u\n", WSAGetLastError());
    } else
        wprintf(L"SO_KEEPALIVE Value: %ld\n", iOptVal);

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void ioctlsocket1()
/*
Example Code
The following example demonstrates the use of the ioctlsocket function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-ioctlsocket
*/
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult;
    u_long iMode = 0;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
        printf("Error at WSAStartup()\n");

    // Create a SOCKET object.
    SOCKET m_socket;
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Set the socket I/O mode: In this case FIONBIO
    // enables or disables the blocking mode for the 
    // socket based on the numerical value of iMode.
    // If iMode = 0, blocking is enabled; 
    // If iMode != 0, non-blocking mode is enabled.

    iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
        printf("ioctlsocket failed with error: %ld\n", iResult);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
