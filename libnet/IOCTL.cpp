#include "pch.h"
#include "ioctl.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void getsockopt1()
/*
The following code sample demonstrates the use of the getsockopt function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
*/
{
    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != NO_ERROR)
        printf("Error at WSAStartup\n");

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a listening socket
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket()\n");
        WSACleanup();
        return;
    }

    // Bind the socket to the local IP address and port 27015
    hostent * thisHost{};
    char * ip{};
    u_short port = 27015;
    thisHost = gethostbyname("");
    ip = inet_ntoa(*reinterpret_cast<struct in_addr *>(*thisHost->h_addr_list));

    sockaddr_in service{};
    service.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &service.sin_addr);
    service.sin_port = htons(port);
    if (bind(ListenSocket, reinterpret_cast<SOCKADDR *>(&service), sizeof(service)) == SOCKET_ERROR) {
        printf("bind failed\n");
        closesocket(ListenSocket);
        return;
    }

    // Initialize variables and call getsockopt.
    // The SO_ACCEPTCONN parameter is a socket option that tells the function to check whether the socket has been put in listening mode or not.
    // The various socket options return different information about the socket.
    // This call should return 0 to the optVal parameter, since the socket is not in listening mode.
    int optVal{};
    int optLen = sizeof(int);
    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_ACCEPTCONN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR)
        printf("SockOpt Value: %d\n", optVal);

    if (listen(ListenSocket, 100) == SOCKET_ERROR) { // Put the listening socket in listening mode.
        printf("error listening\n");
    }

    // Call getsockopt again to verify that the socket is in listening mode.
    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_ACCEPTCONN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR)
        printf("SockOpt Value: %d\n", optVal);

    WSACleanup();
}


int getsockopt2()
/*
Example Code

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
*/
{
    // Declare and initialize variables
    WSADATA wsaData{};
    DWORD dwError{};
    SOCKET Sock = INVALID_SOCKET;
#define DEVICE_LIST_LEN 10
    SOCKADDR_IRDA DestSockAddr = {AF_IRDA, 0, 0, 0, 0, "SampleIrDAService"};
    unsigned char DevListBuff[sizeof(DEVICELIST) - sizeof(IRDA_DEVICE_INFO) + (sizeof(IRDA_DEVICE_INFO) * DEVICE_LIST_LEN)]{};
    int DevListLen = sizeof(DevListBuff);
    PDEVICELIST pDevList = reinterpret_cast<PDEVICELIST>(&DevListBuff);

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    Sock = socket(AF_IRDA, SOCK_STREAM, 0);
    if (Sock == INVALID_SOCKET) {
        dwError = WSAGetLastError();
        printf("socket failed trying to create an AF_IRDA socket with error %lu\n", dwError);
        if (dwError == WSAEAFNOSUPPORT) {
            printf("Check that the local computer has an infrared device\n");
            printf("and a device driver is installed for the infrared device\n");
        }
        WSACleanup();
        return 1;
    }

    // Sock is not in connected state
    iResult = getsockopt(Sock, SOL_IRLMP, IRLMP_ENUMDEVICES, reinterpret_cast<char *>(pDevList), &DevListLen);
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
        for (ULONG i = 0; i < pDevList->numDevice; i++) {
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

    iResult = connect(Sock, reinterpret_cast<const struct sockaddr *>(&DestSockAddr), sizeof(SOCKADDR_IRDA));
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
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != NO_ERROR)
        printf("Error at WSAStartup\n");

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a socket
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket()\n");
        WSACleanup();
        return;
    }

    int optVal{};
    int optLen = sizeof(int);
    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR)
        printf("SO_RCVBUF Value: %d\n", optVal);

    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR)
        printf("SO_SNDBUF Value: %d\n", optVal);

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
    WSADATA wsaData{};    
    BOOL bOptVal = FALSE;
    int bOptLen = sizeof(BOOL);
    int iOptVal = 0;
    int iOptLen = sizeof(int);

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != NO_ERROR) {
        wprintf(L"Error at WSAStartup()\n");
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a listening socket
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind the socket to the local IP address and port 27015
    hostent * thisHost{};
    char * ip{};
    u_short port = 27015;
    thisHost = gethostbyname("");
    ip = inet_ntoa(*reinterpret_cast<struct in_addr *>(*thisHost->h_addr_list));

    sockaddr_in service{};
    service.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &service.sin_addr);
    service.sin_port = htons(port);
    iResult = bind(ListenSocket, reinterpret_cast<SOCKADDR *>(&service), sizeof(service));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Initialize variables and call setsockopt.
    // The SO_KEEPALIVE parameter is a socket option that makes the socket send keepalive messages on the session.
    // The SO_KEEPALIVE socket option requires a boolean value to be passed to the setsockopt function.
    // If TRUE, the socket is configured to send keepalive messages, if FALSE the socket configured to NOT send keepalive messages.
    // This section of code tests the setsockopt function by checking the status of SO_KEEPALIVE on the socket using the getsockopt function.

    bOptVal = TRUE;
    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&iOptVal), &iOptLen);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"getsockopt for SO_KEEPALIVE failed with error: %d\n", WSAGetLastError());
    } else
        wprintf(L"SO_KEEPALIVE Value: %ld\n", iOptVal);

    iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&bOptVal), bOptLen);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"setsockopt for SO_KEEPALIVE failed with error: %d\n", WSAGetLastError());
    } else
        wprintf(L"Set SO_KEEPALIVE: ON\n");

    iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&iOptVal), &iOptLen);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"getsockopt for SO_KEEPALIVE failed with error: %d\n", WSAGetLastError());
    } else
        wprintf(L"SO_KEEPALIVE Value: %ld\n", iOptVal);

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}


void EnableKeepAlive(SOCKET sock)
/*
功能：设置保活。

AI生成的代码，没有测试。

SO_KEEPALIVE 是干嘛的
一句话：用来检测 TCP 连接是否“假死”。

典型场景：
1.对端已经断电/崩溃/网络断了
2.本地 TCP 连接还显示是 ESTABLISHED
3.你不读不写，永远发现不了连接已经废了

开启 SO_KEEPALIVE 后，内核会定期发探测包，如果多次无响应，就判定连接已断，recv/send 会报错。

只对 TCP 有效，对 UDP 没意义。

KEEPALIVE 只检测空闲连接的存活，不能立即检测有数据传输时的掉线（例如，半开连接）。
对于实时性要求高的应用，可能需要结合心跳机制（应用层自定义包）使用。

注意事项局限性：
1.KEEPALIVE 无法检测防火墙或 NAT 导致的半开连接（一方认为连接正常，另一方已关闭）。应用层心跳（如每几秒发送一个包）更可靠。
2.跨平台：Windows 默认行为如上所述；Linux/macOS 可通过 TCP_KEEPIDLE、TCP_KEEPINTVL、TCP_KEEPCNT 更精细控制。
3.测试：模拟掉线可拔网线或杀掉对端进程，观察检测时间。
4.如果是服务器端，类似逻辑适用于接受的套接字。

检测掉线：KEEPALIVE 本身不直接通知应用程序掉线，而是通过套接字的读/写操作返回错误来体现。
常见方式：
1.使用 select、poll 或 epoll（Windows 用 WSAPoll）监控套接字的可读/可写/异常事件。
2.尝试读/写数据：如果连接已关闭，recv 返回 0（优雅关闭）或 -1（错误，如 WSAECONNRESET）。
3.定期检查套接字状态。

https://learn.microsoft.com/en-us/windows/win32/winsock/sio-keepalive-vals
https://learn.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2003/cc782936(v=ws.10)
https://learn.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2003/cc758083(v=ws.10)
*/
/*
另一个AI的代码：

tcp_keepalive ka;
ka.onoff = 1;
ka.keepalivetime = 30 * 1000;      // 30 秒无数据后开始探测
ka.keepaliveinterval = 5 * 1000;  // 每 5 秒一次

DWORD bytesReturned;
WSAIoctl(sock, SIO_KEEPALIVE_VALS, &ka, sizeof(ka), nullptr, 0, &bytesReturned, nullptr, nullptr);
*/
/*
检测代码：

int main() {
    // ... WSAStartup, socket 创建, connect 连接 ...

    enableKeepAlive(sock);  // 启用 KEEPALIVE

    // 使用 select 监控（示例循环）
    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        struct timeval timeout = {5, 0};  // 每 5 秒检查一次
        int ret = select(0, &readfds, nullptr, nullptr, &timeout);

        if (ret == SOCKET_ERROR) {
            // select 错误
            break;
        } else if (ret == 0) {
            // 超时，继续循环（可发送心跳如果需要）
            continue;
        }

        if (FD_ISSET(sock, &readfds)) {
            char buf[1024];
            int bytes = recv(sock, buf, sizeof(buf), 0);
            if (bytes > 0) {
                // 收到数据，处理
            } else if (bytes == 0) {
                // 连接优雅关闭（对方调用 closesocket）
                printf("对方已关闭连接\n");
                break;
            } else {
                // 错误：如 WSAECONNRESET 表示连接重置（掉线）
                int err = WSAGetLastError();
                if (err == WSAECONNRESET || err == WSAECONNABORTED) {
                    printf("检测到对方掉线\n");
                }
                break;
            }
        }
    }

    // ... 清理 ...
    return 0;
}
*/
{
    // 启用 SO_KEEPALIVE
    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval)) == SOCKET_ERROR) {
        // 处理错误
        return;
    }

    // 自定义参数：空闲 30 秒后探测，每 1 秒一次
    struct tcp_keepalive alive;
    alive.onoff = 1;
    alive.keepalivetime = 30000; // 毫秒
    alive.keepaliveinterval = 1000;

    DWORD bytesReturned;
    if (WSAIoctl(sock, SIO_KEEPALIVE_VALS, &alive, sizeof(alive), nullptr, 0, &bytesReturned, nullptr, nullptr) == SOCKET_ERROR) {
        // 处理错误
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void ioctlsocket1()
/*
The following example demonstrates the use of the ioctlsocket function.

https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-ioctlsocket
*/
{
    // Initialize Winsock
    WSADATA wsaData{};
    u_long iMode = 0;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
        printf("Error at WSAStartup()\n");

    SOCKET m_socket;
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a SOCKET object.
    if (m_socket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Set the socket I/O mode: In this case FIONBIO
    // enables or disables the blocking mode for the socket based on the numerical value of iMode.
    // If iMode = 0, blocking is enabled;
    // If iMode != 0, non-blocking mode is enabled.

    iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
        printf("ioctlsocket failed with error: %d\n", iResult);
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
    Return Value (DWORD):  Returns TRUE if successful, and places a valid handle to the TCP driver in the
                           handle pointed to by pTCPDriverHandle, or returns FALSE otherwise, and sets the handle to INVALID_HANDLE_VALUE.
*/
{
    NTSTATUS rVal{};
    WCHAR TCPDriverName[] = DD_TCP_DEVICE_NAME;
    OBJECT_ATTRIBUTES objectAttributes{};
    IO_STATUS_BLOCK ioStatusBlock{};
    UNICODE_STRING UnicodeStr{};

    *pTCPDriverHandle = INVALID_HANDLE_VALUE;

    UnicodeStr.Buffer = TCPDriverName;
    UnicodeStr.Length = static_cast<USHORT>(wcslen(TCPDriverName) * sizeof(WCHAR));
    UnicodeStr.MaximumLength = UnicodeStr.Length + sizeof(UNICODE_NULL);

    objectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
    objectAttributes.ObjectName = &UnicodeStr;
    objectAttributes.Attributes = OBJ_CASE_INSENSITIVE;
    objectAttributes.RootDirectory = nullptr;
    objectAttributes.SecurityDescriptor = nullptr;
    objectAttributes.SecurityQualityOfService = nullptr;

    rVal = NtCreateFile(pTCPDriverHandle,
                        SYNCHRONIZE | GENERIC_EXECUTE,
                        &objectAttributes,
                        &ioStatusBlock,
                        nullptr,
                        FILE_ATTRIBUTE_NORMAL,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_OPEN_IF,
                        FILE_SYNCHRONOUS_IO_NONALERT,
                        nullptr,
                        0);
    if (rVal < 0) {
        printf("\nFailed to create TCP Driver handle; NT status code = %ld.", rVal);
        *pTCPDriverHandle = INVALID_HANDLE_VALUE;
        return (FALSE);
    }

    return (TRUE);
}


DWORD GetEntityArray(IN HANDLE TCPDriverHandle, OUT TDIEntityID ** lplpEntities)
/*
Function:              GetEntityList
Description:
      Allocates a buffer for and retrieves an array of TDIEntityID
    structures that identifies the entities supported by the TCP/IP device driver.
Parameters:
      TCPDriverHandle  --  An open handle to the TCP Driver;
                           if no such handle is available, may be INVALID_HANDLE_VALUE.
      lplpEntities     --  Pointer to a buffer that contains the array of TDIEntityID structures.
                           Must be freed by the calling process using LocalFree( ).
Return Value:
      DWORD  --  the number of entity structures in the returned array
*/
{
    TCP_REQUEST_QUERY_INFORMATION_EX req{};
    DWORD arrayLen = sizeof(TDIEntityID) * MAX_TDI_ENTITIES;
    DWORD bufferLen = arrayLen;
    TDIEntityID * pEntity = nullptr;
    NTSTATUS status = TDI_SUCCESS;
    DWORD temporaryHandle = 0;
    int i{};

    // First, if the handle passed in is not valid, try to obtain one.
    if (TCPDriverHandle == INVALID_HANDLE_VALUE) {
        if (GetTCPHandle(&TCPDriverHandle) == FALSE) {
            *lplpEntities = nullptr;
            return (0);
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
    // (1)  In the first place, it is unlikely that more than MAX_TDI_ENTITIES of TCP/IP entities exist, so the loop should execute only once.
    // (2)  Execution is limited to 4 iterations to rule out infinite looping in case of parameter corruption.
    //     Only 2 iterations should ever be necessary unless entities are being added while the loop is running.
    for (i = 0; i < 4; ++i) {
        if (pEntity != nullptr) {
            LocalFree(pEntity);
            pEntity = nullptr;
            bufferLen = arrayLen;
        }

        if (arrayLen == 0)
            break;

        pEntity = static_cast<TDIEntityID *>(LocalAlloc(LMEM_FIXED, bufferLen));
        if (pEntity == nullptr) {
            arrayLen = 0;
            break;
        }

        if (!DeviceIoControl(TCPDriverHandle,                // Handle to TCP driver
                             IOCTL_TCP_QUERY_INFORMATION_EX, // Cmd code
                             &req,                           // Pointer to input buffer
                             sizeof(req),                    // Size of ipt buffer
                             pEntity,                        // Ptr to output buffer
                             bufferLen,                      // Size of output buffer
                             &arrayLen,                      // Actual size of array
                             nullptr))
            status = GetLastError();

        // Even if the output buffer is too small, the TCP driver returns a status of TDI_SUCCESS;
        // it is the value returned in arrayLen that indicates whether the entire array was successfully copied to the output buffer.
        if (status == TDI_SUCCESS) {
            if (arrayLen && (arrayLen <= bufferLen))
                break;
        } else
            arrayLen = 0;
    }

    if (temporaryHandle)
        CloseHandle(TCPDriverHandle);

    *lplpEntities = pEntity;
    return static_cast<DWORD>(arrayLen / sizeof(TDIEntityID));
}


EXTERN_C
DLLEXPORT
int WINAPI EnumEntityArray()
/*
The following example shows how to obtain a list of the entities present on the TCP adapter on the current machine.

https://docs.microsoft.com/en-us/windows/win32/api/tcpioctl/ni-tcpioctl-ioctl_tcp_query_information_ex
*/
{
    TDIEntityID *entityArray{}, *entityPtr{};

    DWORD entityCount = GetEntityArray(INVALID_HANDLE_VALUE, &entityArray);
    if (!(entityCount))
        return (1);

    entityPtr = entityArray;
    printf("\n\nList of %lu Transport Driver Interface Entities on this machine:\n", entityCount);

    for (DWORD i = 0; i < entityCount; ++i) {
        printf("\n  Entity #%lu:\n    Category (tei_entity) is ", i);
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
            printf("[Unidentified Entity Type] = 0x%lx", entityPtr->tei_entity);
        }

        printf("\n Instance (tei_instance) = %lu\n", entityPtr->tei_instance);

        ++entityPtr;
    }

    LocalFree(entityArray); //  Free the entity-array buffer before quitting.

    return (0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI TestPersistentTcpPortReservation(int argc, WCHAR ** argv)
/*
示例
以下示例创建一个永久性 TCP 端口预留，然后创建一个套接字并从端口预留中分配一个端口，然后关闭该套接字并删除 TCP
端口预留。

此示例必须由管理员组成员的用户运行。 运行此示例的最简单方法是使用增强的 shell，因为内置管理员 (RunAs 管理员) 。

https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-createpersistenttcpportreservation
*/
{
    // Declare and initialize variables

    int startPort = 0; // host byte order
    int numPorts = 0;
    USHORT startPortns = 0; // Network byte order
    ULONG64 resToken{};

    unsigned long status = 0;

    WSADATA wsaData{};
    int iResult = 0;

    SOCKET sock = INVALID_SOCKET;
    int iFamily = AF_INET;
    int iType = SOCK_STREAM;
    int iProtocol = IPPROTO_TCP;

    DWORD bytesReturned = 0;

    // Note that the sockaddr_in struct works only with AF_INET not AF_INET6
    // An application needs to use the sockaddr_in6 for AF_INET6
    sockaddr_in service{};
    sockaddr_in sockName{};
    int nameLen = sizeof(sockName);

    // Validate the parameters
    if (argc != 3) {
        wprintf(L"usage: %s <Starting Port> <Number of Ports>\n", argv[0]);
        wprintf(L"Creates a persistent TCP port reservation\n");
        wprintf(L"Example usage:\n");
        wprintf(L"   %s 5000 20\n", argv[0]);
        wprintf(L"   where StartPort=5000 NumPorts=20");
        return 1;
    }

    startPort = _wtoi(argv[1]);
    if (startPort < 0 || startPort > 65535) {
        wprintf(L"Starting point must be either 0 or between 1 and 65,535\n");
        return 1;
    }
    startPortns = htons((USHORT)startPort);

    numPorts = _wtoi(argv[2]);
    if (numPorts < 0) {
        wprintf(L"Number of ports must be a positive number\n");
        return 1;
    }

    status = CreatePersistentTcpPortReservation((USHORT)startPortns, (USHORT)numPorts, &resToken);
    if (status != NO_ERROR) {
        wprintf(L"CreatePersistentTcpPortReservation returned error: %lu\n", status);
        return 1;
    }

    wprintf(L"CreatePersistentTcpPortReservation call succeeded\n");
    wprintf(L"  Token = %I64u\n", resToken);

    // Comment out this block if you don't want to create a socket and associate it with the persistent reservation

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error = %d\n", iResult);
        // return 1;
    }

    sock = socket(iFamily, iType, iProtocol);
    if (sock == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
    else {
        wprintf(L"socket function succeeded\n");

        iResult = WSAIoctl(sock, SIO_ASSOCIATE_PORT_RESERVATION, (LPVOID)&resToken, sizeof(ULONG64), nullptr, 0, &bytesReturned, nullptr, nullptr);
        if (iResult != 0) {
            wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) failed with error = %d\n", WSAGetLastError());
        } else {
            wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) succeeded, bytesReturned = %u\n", bytesReturned);

            service.sin_family = AF_INET;
            service.sin_addr.s_addr = INADDR_ANY;
            service.sin_port = 0;
            iResult = bind(sock, (SOCKADDR *)&service, sizeof(service));
            if (iResult == SOCKET_ERROR)
                wprintf(L"bind failed with error = %d\n", WSAGetLastError());
            else {
                wprintf(L"bind succeeded\n");
                iResult = getsockname(sock, (SOCKADDR *)&sockName, &nameLen);
                if (iResult == SOCKET_ERROR)
                    wprintf(L"getsockname failed with error = %d\n", WSAGetLastError());
                else {
                    wprintf(L"getsockname succeeded\n");
                    wprintf(L"Port number allocated = %u\n", ntohs(sockName.sin_port));
                }
            }
        }

        if (sock != INVALID_SOCKET) {
            iResult = closesocket(sock);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"closesocket failed with error = %d\n", WSAGetLastError());
                WSACleanup();
            }
        }
    }

    // comment out this block of code if you don't want to delete the reservation just created
    status = DeletePersistentTcpPortReservation((USHORT)startPortns, (USHORT)numPorts);
    if (status != NO_ERROR) {
        wprintf(L"DeletePersistentTcpPortReservation returned error: %lu\n", status);
        return 1;
    }
    wprintf(L"DeletePersistentTcpPortReservation call succeeded\n");

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI GetPortReservation(int argc, WCHAR ** argv)
/*
以下示例获取运行时端口预留，然后创建一个套接字，并从该套接字的运行时端口预留中分配一个端口，
然后关闭该套接字并释放运行时端口预留。

https://learn.microsoft.com/zh-cn/windows/win32/winsock/sio-acquire-port-reservation
*/
{
    // Declare and initialize variables

    int startPort = 0; // host byte order
    int numPorts = 0;
    USHORT startPortns = 0; // Network byte order

    INET_PORT_RANGE portRange{};
    INET_PORT_RESERVATION_INSTANCE portRes{};

    // unsigned long status = 0;

    WSADATA wsaData{};
    int iResult = 0;

    SOCKET sock = INVALID_SOCKET;
    int iFamily = AF_INET;
    int iType = 0;
    int iProtocol = 0;

    SOCKET sockRes = INVALID_SOCKET;

    DWORD bytesReturned = 0;

    // Note that the sockaddr_in struct works only with AF_INET not AF_INET6
    // An application needs to use the sockaddr_in6 for AF_INET6
    sockaddr_in service{};
    sockaddr_in sockName{};
    int nameLen = sizeof(sockName);

    // Validate the parameters
    if (argc != 6) {
        wprintf(L"usage: %s <addressfamily> <type> <protocol> <StartingPort> <NumberOfPorts>\n", argv[0]);
        wprintf(L"Opens a socket for the specified family, type, & protocol\n");
        wprintf(L"and then acquires a runtime port reservation for the protocol specified\n");
        wprintf(L"%ws example usage\n", argv[0]);
        wprintf(L"   %ws 2 2 17 5000 20\n", argv[0]);
        wprintf(L"   where AF_INET=2 SOCK_DGRAM=2 IPPROTO_UDP=17 StartPort=5000 NumPorts=20"); //, argv[0]

        return 1;
    }

    iFamily = _wtoi(argv[1]);
    iType = _wtoi(argv[2]);
    iProtocol = _wtoi(argv[3]);

    startPort = _wtoi(argv[4]);
    if (startPort < 0 || startPort > 65535) {
        wprintf(L"Starting point must be either 0 or between 1 and 65,535\n");
        return 1;
    }
    startPortns = htons((USHORT)startPort);

    numPorts = _wtoi(argv[5]);
    if (numPorts < 0) {
        wprintf(L"Number of ports must be a positive number\n");
        return 1;
    }

    portRange.StartPort = startPortns;
    portRange.NumberOfPorts = (USHORT)numPorts;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error = %d\n", iResult);
        return 1;
    }

    sock = socket(iFamily, iType, iProtocol);
    if (sock == INVALID_SOCKET) {
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    } else {
        wprintf(L"socket function succeeded\n");

        iResult = WSAIoctl(sock, 
                           SIO_ACQUIRE_PORT_RESERVATION,
                           (LPVOID)&portRange,
                           sizeof(INET_PORT_RANGE),
                           (LPVOID)&portRes,
                           sizeof(INET_PORT_RESERVATION_INSTANCE),
                           &bytesReturned,
                           nullptr,
                           nullptr);
        if (iResult != 0) {
            wprintf(L"WSAIoctl(SIO_ACQUIRE_PORT_RESERVATION) failed with error = %d\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
            return 1;
        } else {
            wprintf(L"WSAIoctl(SIO_ACQUIRE_PORT_RESERVATION) succeeded, bytesReturned = %u\n", bytesReturned);
            wprintf(L"  Starting port=%u,  Number of Ports=%u, Token=%I64u\n", 
                    htons(portRes.Reservation.StartPort),
                    portRes.Reservation.NumberOfPorts,
                    portRes.Token.Token);

            sockRes = socket(iFamily, iType, iProtocol);
            if (sockRes == INVALID_SOCKET) {
                wprintf(L"socket function for second socket failed with error = %d\n", WSAGetLastError());
                closesocket(sock);
                WSACleanup();
                return 1;
            } else {
                wprintf(L"socket function for second socket succeeded\n");

                iResult = WSAIoctl(sock, SIO_ASSOCIATE_PORT_RESERVATION, (LPVOID)&portRes.Token, sizeof(ULONG64), nullptr, 0, &bytesReturned, nullptr, nullptr);
                if (iResult != 0) {
                    wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) failed with error = %d\n", WSAGetLastError());
                } else {
                    wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) succeeded, bytesReturned = %u\n", bytesReturned);

                    service.sin_family = (ADDRESS_FAMILY)iFamily;
                    service.sin_addr.s_addr = INADDR_ANY;
                    service.sin_port = 0;
                    iResult = bind(sock, (SOCKADDR *)&service, sizeof(service));
                    if (iResult == SOCKET_ERROR)
                        wprintf(L"bind failed with error = %d\n", WSAGetLastError());
                    else {
                        wprintf(L"bind succeeded\n");
                        iResult = getsockname(sock, (SOCKADDR *)&sockName, &nameLen);
                        if (iResult == SOCKET_ERROR)
                            wprintf(L"getsockname failed with error = %d\n", WSAGetLastError());
                        else {
                            wprintf(L"getsockname succeeded\n");
                            wprintf(L"Port number allocated = %u\n", ntohs(sockName.sin_port));
                        }
                    }
                }
            }

            // comment out this block of code if you don't want to delete the reservation just created
            iResult = WSAIoctl(sock, SIO_RELEASE_PORT_RESERVATION, (LPVOID)&portRes.Token, sizeof(ULONG64), nullptr, 0, &bytesReturned, nullptr, nullptr);
            if (iResult != 0) {
                wprintf(L"WSAIoctl(SIO_RELEASE_PORT_RESERVATION) failed with error = %d\n", WSAGetLastError());
            } else {
                wprintf(L"WSAIoctl(SIO_RELEASE_PORT_RESERVATION) succeeded, bytesReturned = %u\n", bytesReturned);
            }
        }

        if (sockRes != INVALID_SOCKET) {
            iResult = closesocket(sockRes);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"closesocket for second socket failed with error = %d\n", WSAGetLastError());
            }
        }
        if (sock != INVALID_SOCKET) {
            iResult = closesocket(sock);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"closesocket for first socket failed with error = %d\n", WSAGetLastError());
            }
        }
    }

    WSACleanup();

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI TestLookupPersistentTcpPortReservation(int argc, WCHAR ** argv)
/*
以下示例查找永久性 TCP 端口预留，然后创建套接字并从端口预留分配端口。

https://learn.microsoft.com/zh-cn/windows/win32/api/iphlpapi/nf-iphlpapi-lookuppersistenttcpportreservation?redirectedfrom=MSDN
*/
{
    // Declare and initialize variables

    int startPort = 0; // host byte order
    int numPorts = 0;
    USHORT startPortns = 0; // Network byte order
    ULONG64 resToken{};

    unsigned long status = 0;

    WSADATA wsaData{};
    int iResult = 0;

    SOCKET sock = INVALID_SOCKET;
    int iFamily = AF_INET;
    int iType = SOCK_STREAM;
    int iProtocol = IPPROTO_TCP;

    DWORD bytesReturned = 0;

    // Note that the sockaddr_in struct works only with AF_INET not AF_INET6
    // An application needs to use the sockaddr_in6 for AF_INET6
    sockaddr_in service{};
    sockaddr_in sockName{};
    int nameLen = sizeof(sockName);

    // Validate the parameters
    if (argc != 3) {
        wprintf(L"usage: %s <Starting Port> <Number of Ports>\n", argv[0]);
        wprintf(L"Look up a persistent TCP port reservation\n");
        wprintf(L"Example usage:\n");
        wprintf(L"   %s 5000 20\n", argv[0]);
        wprintf(L"   where StartPort=5000 NumPorts=20");
        return 1;
    }

    startPort = _wtoi(argv[1]);
    if (startPort < 0 || startPort > 65535) {
        wprintf(L"Starting point must be either 0 or between 1 and 65,535\n");
        return 1;
    }
    startPortns = htons((USHORT)startPort);

    numPorts = _wtoi(argv[2]);
    if (numPorts < 0) {
        wprintf(L"Number of ports must be a positive number\n");
        return 1;
    }

    //应该先设置后查询，或者查询之前确保已经有设置了，否则返回1168。
    status = LookupPersistentTcpPortReservation((USHORT)startPortns, (USHORT)numPorts, &resToken);
    if (status != NO_ERROR) {
        wprintf(L"LookupPersistentTcpPortReservation returned error: %lu\n", status);
        return 1;
    }

    wprintf(L"LookupPersistentTcpPortReservation call succeeded\n");
    wprintf(L"  Token = %I64u\n", resToken);

    // Comment out this block if you don't want to create a socket and associate it with the persistent reservation

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error = %d\n", iResult);
        return 1;
    }

    sock = socket(iFamily, iType, iProtocol);
    if (sock == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
    else {
        wprintf(L"socket function succeeded\n");

        iResult = WSAIoctl(sock, SIO_ASSOCIATE_PORT_RESERVATION, (LPVOID)&resToken, sizeof(ULONG64), nullptr, 0, &bytesReturned, nullptr, nullptr);
        if (iResult != 0) {
            wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) failed with error = %d\n", WSAGetLastError());
        } else {
            wprintf(L"WSAIoctl(SIO_ASSOCIATE_PORT_RESERVATION) succeeded, bytesReturned = %u\n", bytesReturned);

            service.sin_family = AF_INET;
            service.sin_addr.s_addr = INADDR_ANY;
            service.sin_port = 0;
            iResult = bind(sock, (SOCKADDR *)&service, sizeof(service));
            if (iResult == SOCKET_ERROR)
                wprintf(L"bind failed with error = %d\n", WSAGetLastError());
            else {
                wprintf(L"bind succeeded\n");
                iResult = getsockname(sock, (SOCKADDR *)&sockName, &nameLen);
                if (iResult == SOCKET_ERROR)
                    wprintf(L"getsockname failed with error = %d\n", WSAGetLastError());
                else {
                    wprintf(L"getsockname succeeded\n");
                    wprintf(L"Port number allocated = %u\n", ntohs(sockName.sin_port));
                }
            }
        }

        if (sock != INVALID_SOCKET) {
            iResult = closesocket(sock);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"closesocket failed with error = %d\n", WSAGetLastError());
            }
        }
    }

    WSACleanup();

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI Exclusiveaddruse(int argc, wchar_t ** argv)
/*
SO_EXCLUSIVEADDRUSE套接字选项可防止其他套接字被强行绑定到同一地址和端口。

语法
SO_EXCLUSIVEADDRUSE选项可防止其他套接字被强行绑定到同一地址和端口，这是SO_REUSEADDR套接字选项启用的做法。
恶意应用程序可能会执行此类重用，以破坏应用程序。
SO_EXCLUSIVEADDRUSE选项对于需要高可用性的系统服务非常有用。

下面的代码示例演示了如何设置此选项。

https://learn.microsoft.com/zh-cn/windows/win32/winsock/so-exclusiveaddruse
*/
{
    WSADATA wsaData{};
    int iResult = 0;
    int iError = 0;

    SOCKET s = INVALID_SOCKET;
    SOCKADDR_IN saLocal{};
    int iOptval = 0;

    int iFamily = AF_UNSPEC;
    int iType = 0;
    int iProtocol = 0;

    int iPort = 0;

    // Validate the parameters
    if (argc != 5) {
        wprintf(L"usage: %ws <addressfamily> <type> <protocol> <port>\n", argv[0]);
        wprintf(L"    opens a socket for the specified family, type, & protocol\n");
        wprintf(L"    sets the SO_EXCLUSIVEADDRUSE socket option on the socket\n");
        wprintf(L"    then tries to bind the port specified on the command-line\n");
        wprintf(L"%ws example usage\n", argv[0]);
        wprintf(L"   %ws 0 2 17 5150\n", argv[0]);
        wprintf(L"   where AF_UNSPEC=0 SOCK_DGRAM=2 IPPROTO_UDP=17  PORT=5150\n"); //, argv[0]
        wprintf(L"   %ws 2 1 6 5150\n", argv[0]);                                  // 17
        wprintf(L"   where AF_INET=2 SOCK_STREAM=1 IPPROTO_TCP=6  PORT=5150\n");   //, argv[0]
        wprintf(L"   See the documentation for the socket function for other values\n");
        return 1;
    }

    iFamily = _wtoi(argv[1]);
    iType = _wtoi(argv[2]);
    iProtocol = _wtoi(argv[3]);

    iPort = _wtoi(argv[4]);

    if (iFamily != AF_INET && iFamily != AF_INET6) {
        wprintf(L"Address family must be either AF_INET (2) or AF_INET6 (23)\n");
        return 1;
    }

    if (iPort <= 0 || iPort >= 65535) {
        wprintf(L"Port specified must be greater than 0 and less than 65535\n");
        return 1;
    }

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    s = socket(iFamily, iType, iProtocol); // Create the socket
    if (s == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set the exclusive address option
    iOptval = 1;
    iResult = setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&iOptval, sizeof(iOptval));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"setsockopt for SO_EXCLUSIVEADDRUSE failed with error: %ld\n", WSAGetLastError());
    }

    saLocal.sin_family = (ADDRESS_FAMILY)iFamily;
    saLocal.sin_port = htons((u_short)iPort);
    saLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    iResult = bind(s, (SOCKADDR *)&saLocal, sizeof(saLocal)); // Bind the socket
    if (iResult == SOCKET_ERROR) {
        // Most errors related to setting SO_EXCLUSIVEADDRUSE
        //    will occur at bind.
        iError = WSAGetLastError();
        if (iError == WSAEACCES)
            wprintf(L"bind failed with WSAEACCES (access denied)\n");
        else
            wprintf(L"bind failed with error: %ld\n", iError);
    } else {
        wprintf(L"bind on socket with SO_EXCLUSIVEADDRUSE succeeded to port: %ld\n", iPort);
    }

    // cleanup
    closesocket(s);
    WSACleanup();

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
