#include "pch.h"
#include "IOCTL.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void getsockopt1()
/*
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
    // that tells the function to check whether the socket has been put in listening mode or not. 
    // The various socket options return different information about the socket.
    // This call should return 0 to the optVal parameter, since the socket is not in listening mode.
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
    // that makes the socket send keepalive messages on the session.
    // The SO_KEEPALIVE socket option requires a boolean value to be passed to the setsockopt function.
    // If TRUE, the socket is configured to send keepalive messages, if FALSE
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
    // enables or disables the blocking mode for the socket based on the numerical value of iMode.
    // If iMode = 0, blocking is enabled; 
    // If iMode != 0, non-blocking mode is enabled.

    iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
        printf("ioctlsocket failed with error: %ld\n", iResult);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


//#define  UNICODE
//#define  _WIN32_WINNT  0x0500


DWORD GetTCPHandle(PHANDLE pTCPDriverHandle)
/*  Function:              GetTCPHandle
    Description:
      Opens a handle to the TCP driver
    Parameters:
      pTCPDriverHandle --  Pointer to a handle variable.
    Return Value (DWORD):  Returns TRUE if successful, and places
                           a valid handle to the TCP driver in the
                           handle pointed to by pTCPDriverHandle, or
                           returns FALSE otherwise, and sets the handle to INVALID_HANDLE_VALUE.
*/
{
#define FILE_OPEN_IF                    0x00000003
#define FILE_SYNCHRONOUS_IO_NONALERT    0x00000020
#define OBJ_CASE_INSENSITIVE            0x00000040L

    typedef NTSTATUS(NTAPI * P_NT_CREATE_FILE)(
        OUT PHANDLE              FileHandle,
        IN  ACCESS_MASK          DesiredAccess,
        IN  POBJECT_ATTRIBUTES   ObjectAttributes,
        OUT PIO_STATUS_BLOCK     IoStatusBlock,
        IN  PLARGE_INTEGER       AllocationSize OPTIONAL,
        IN  ULONG                FileAttributes,
        IN  ULONG                ShareAccess,
        IN  ULONG                CreateDisposition,
        IN  ULONG                CreateOptions,
        IN  PVOID                EaBuffer OPTIONAL,
        IN  ULONG                EaLength);

    HINSTANCE hNtDLL;
    P_NT_CREATE_FILE pNtCreateFile;
    NTSTATUS rVal;
    WCHAR TCPDriverName[] = DD_TCP_DEVICE_NAME;

    OBJECT_ATTRIBUTES  objectAttributes;
    IO_STATUS_BLOCK    ioStatusBlock;
    UNICODE_STRING     UnicodeStr;

    *pTCPDriverHandle = INVALID_HANDLE_VALUE;

    if ((hNtDLL = LoadLibrary(L"ntdll")) == NULL)
        return(FALSE);

    pNtCreateFile = (P_NT_CREATE_FILE)GetProcAddress(hNtDLL, "NtCreateFile");
    if (pNtCreateFile == NULL)
        return(FALSE);

    UnicodeStr.Buffer = TCPDriverName;
    UnicodeStr.Length = (USHORT)(wcslen(TCPDriverName) * sizeof(WCHAR));
    UnicodeStr.MaximumLength = UnicodeStr.Length + sizeof(UNICODE_NULL);

    objectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
    objectAttributes.ObjectName = &UnicodeStr;
    objectAttributes.Attributes = OBJ_CASE_INSENSITIVE;
    objectAttributes.RootDirectory = NULL;
    objectAttributes.SecurityDescriptor = NULL;
    objectAttributes.SecurityQualityOfService = NULL;

    rVal = pNtCreateFile(pTCPDriverHandle,
                         SYNCHRONIZE | GENERIC_EXECUTE,
                         &objectAttributes,
                         &ioStatusBlock,
                         NULL,
                         FILE_ATTRIBUTE_NORMAL,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         FILE_OPEN_IF,
                         FILE_SYNCHRONOUS_IO_NONALERT,
                         NULL,
                         0);
    if (rVal < 0) {
        printf("\nFailed to create TCP Driver handle; NT status code = %d.", rVal);
        *pTCPDriverHandle = INVALID_HANDLE_VALUE;
        return(FALSE);
    }

    return(TRUE);
}


DWORD GetEntityArray(IN HANDLE TCPDriverHandle, OUT TDIEntityID ** lplpEntities)
/*  Function:              GetEntityList
    Description:
      Allocates a buffer for and retrieves an array of TDIEntityID
    structures that identifies the entities supported by
    the TCP/IP device driver.
    Parameters:
      TCPDriverHandle  --  An open handle to the TCP Driver; if
            no such handle is available,
            may be INVALID_HANDLE_VALUE.
      lplpEntities     --  Pointer to a buffer that contains
            the array of TDIEntityID structures.
            Must be freed by the calling process
            using LocalFree( ).
    Return Value:
      DWORD  --  the number of entity structures in the returned array
*/
{
    TCP_REQUEST_QUERY_INFORMATION_EX  req;
    DWORD arrayLen = sizeof(TDIEntityID) * MAX_TDI_ENTITIES;
    DWORD bufferLen = arrayLen;
    TDIEntityID * pEntity = NULL;
    NTSTATUS status = TDI_SUCCESS;
    DWORD temporaryHandle = 0;
    int i;

    // First, if the handle passed in is not valid, try to obtain one.
    if (TCPDriverHandle == INVALID_HANDLE_VALUE) {
        if (GetTCPHandle(&TCPDriverHandle) == FALSE) {
            *lplpEntities = NULL;
            return(0);
        }
        temporaryHandle = TRUE;
    }

    // Next, set up the input structure for the IOCTL operation.
    req.ID.toi_entity.tei_entity = GENERIC_ENTITY;
    req.ID.toi_entity.tei_instance = 0;
    req.ID.toi_class = INFO_CLASS_GENERIC;
    req.ID.toi_type = INFO_TYPE_PROVIDER;
    req.ID.toi_id = ENTITY_LIST_ID;

    // The loop below is defensively engineered:
    // (1)  In the first place, it is unlikely that more 
    //     than MAX_TDI_ENTITIES of TCP/IP entities exist, so the loop should execute only once.
    // (2)  Execution is limited to 4 iterations to rule out 
    //     infinite looping in case of parameter corruption. Only 2
    //     iterations should ever be necessary unless entities are 
    //     being added while the loop is running.
    for (i = 0; i < 4; ++i) {
        if (pEntity != NULL) {
            LocalFree(pEntity);
            pEntity = NULL;
            bufferLen = arrayLen;
        }

        if (arrayLen == 0)
            break;

        pEntity = (TDIEntityID *)LocalAlloc(LMEM_FIXED, bufferLen);
        if (pEntity == NULL) {
            arrayLen = 0;
            break;
        }

        if (!DeviceIoControl(TCPDriverHandle, // Handle to TCP driver
                             IOCTL_TCP_QUERY_INFORMATION_EX, // Cmd code
                             &req,            // Pointer to input buffer
                             sizeof(req),     // Size of ipt buffer
                             pEntity,         // Ptr to output buffer
                             bufferLen,       // Size of output buffer
                             &arrayLen,       // Actual size of array
                             NULL))
            status = GetLastError();

        // Even if the output buffer is too small, the TCP driver 
        // returns a status of TDI_SUCCESS; it is the value returned in
        // arrayLen that indicates whether the entire array was 
        // successfully copied to the output buffer.
        if (status == TDI_SUCCESS) {
            if (arrayLen && (arrayLen <= bufferLen))
                break;
        } else
            arrayLen = 0;
    }

    if (temporaryHandle)
        CloseHandle(TCPDriverHandle);

    *lplpEntities = pEntity;
    return((DWORD)(arrayLen / sizeof(TDIEntityID)));
}


EXTERN_C
__declspec(dllexport)
int WINAPI EnumEntityArray()
/*
The following example shows how to obtain a list of the entities present on the TCP adapter on the current machine.

https://docs.microsoft.com/en-us/windows/win32/api/tcpioctl/ni-tcpioctl-ioctl_tcp_query_information_ex
*/
{
    DWORD i;
    DWORD entityCount;
    TDIEntityID * entityArray, * entityPtr;

    if (!(entityCount = GetEntityArray(INVALID_HANDLE_VALUE, &entityArray)))
        return(1);

    entityPtr = entityArray;
    printf("\n\nList of %d Transport Driver Interface Entities on this machine:\n", entityCount);

    for (i = 0; i < entityCount; ++i) {
        printf("\n  Entity #%d:\n    Category (tei_entity) is ", i);
        switch (entityPtr->tei_entity) {
        case GENERIC_ENTITY:
            printf("Generic.");
            break;
        case CL_NL_ENTITY:
            printf("Connectionless Network-Layer (CL_NL)");
            break;
        case CO_NL_ENTITY:
            printf("Connected Network-Layer (CO_NL)");
            break;
        case CL_TL_ENTITY:
            printf("Connectionless Transport-Layer (CL_TL)");
            break;
        case CO_TL_ENTITY:
            printf("Connected Transport-Layer (CO_TL)");
            break;
        case AT_ENTITY:
            printf("Address Translation (AT)");
            break;
        case IF_ENTITY:
            printf("Interface (IF)");
            break;
        case ER_ENTITY:
            printf("Echo Request/Response (ER)");
            break;
        default:
            printf("[Unidentified Entity Type] = 0x%x", entityPtr->tei_entity);
        }

        printf("\n Instance (tei_instance) = %d\n", entityPtr->tei_instance);

        ++entityPtr;
    }

    LocalFree(entityArray);//  Free the entity-array buffer before quitting.

    return(0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
