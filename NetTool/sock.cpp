#include "sock.h"


static int Usage(__in const wchar_t * name)
/*++
Routine Description:
    This routine prints the intended usage for this program.
Arguments:
    progName - NULL terminated string representing the name of the executable
--*/
{
    wprintf(L"%s IPv4.\n", name);
    wprintf(L"%s IPv6.\n", name);
    wprintf(L"%s rm.\n", name);
    wprintf(L"%s tcp.\n", name);
    wprintf(L"%s udp.\n", name);
    wprintf(L"%s ipx.\n", name);
    wprintf(L"%s AppleTalk.\n", name);
    wprintf(L"%s irlmp.\n", name);
    wprintf(L"%s sol.\n", name);
    wprintf(L"%s all.\n", name);

    return ERROR_SUCCESS;
}


int get_one_sock_opt(_In_ SOCKET s, _In_ int optname)
/*

经IDA发现level只能取SOL_SOCKET。
*/
{
    int ret = ERROR_SUCCESS;

    char * optVal = (char *)MALLOC(1);
    _ASSERTE(optVal);

    int optLen = sizeof(char);
    int iResult = getsockopt(s, SOL_SOCKET, optname, optVal, &optLen);
    switch (iResult) {
    case ERROR_SUCCESS:
    {

        break;
    }
    case SOCKET_ERROR:
    {
        int LastError = WSAGetLastError();
        switch (LastError) {
        case WSAENOPROTOOPT:
        {

            break;
        }
        case WSAEFAULT:
        {
            FREE(optVal);

            optVal = (char *)MALLOC(optLen);
            _ASSERTE(optVal);

            int iResult = getsockopt(s, SOL_SOCKET, optname, optVal, &optLen);
            switch (iResult) {
            case ERROR_SUCCESS:

                break;
            case SOCKET_ERROR:

                break;
            default:
                break;
            }

            break;
        }
        default:
            wprintf(L"optname:%d.\n", optname);
            DisplayError(LastError);
            wprintf(L"\n");
            break;
        }

        break;
    }
    default:
        _ASSERTE(FALSE);
        break;
    }

    if (optVal) {
        FREE(optVal);
    }

    return ret;
}


int get_all_sock_opt(_In_ SOCKET s)
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-ip-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    for (int optname = 0; optname < SOL_SOCKET; optname++) {
        get_one_sock_opt(s, optname);
    }

    return ret;
}


int get_all_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-ip-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    _ASSERTE(iResult == NO_ERROR);

    SOCKET s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //还可以搞侦听绑定的。

    get_all_sock_opt(s4);
    get_all_sock_opt(s6);

    closesocket(s4);

    WSACleanup();

    return ret;
}


int get_ipv4_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-ip-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_ipv6_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-ipv6-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_rm_sock_opt()
/*

https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-rm-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_tcp_sock_opt(_In_ SOCKET s)
/*


*/
{
    int ret = ERROR_SUCCESS;

    /*
    如果 为 TRUE，则服务提供商实现 Berkeley Software Distribution (BSD) 样式， (用于处理加速数据的默认) 。
    此选项与 TCP_EXPEDITED_1122 选项相反。 此选项只能在连接上设置一次。 设置此选项后，无法关闭此选项。 
    服务提供商不需要实现此选项。 默认情况下， (设置为 TRUE) 启用此选项。
    */
    int optVal{};
    int optLen = sizeof(int);
    int iResult = getsockopt(s, SOL_SOCKET, TCP_BSDURGENT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        printf("LastError:%d\n", WSAGetLastError());
    }

    /*
    如果 为 TRUE，则服务提供商实现 RFC-1222 中指定的加速数据。 
    否则，将使用 Berkeley Software Distribution (BSD) 样式 (默认) 。 
    此选项只能在连接上设置一次。
    设置此选项后，无法关闭此选项。 服务提供商不需要实现此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_EXPEDITED_1122, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    如果 为 TRUE，则收到值为 WSAEHOSTUNREACH 的 ICMP 错误时，将返回连接 API 调用。
    然后，将通过“TCP_ICMP_ERROR_INFO套接字”选项获取错误的源地址。 
    如果 为 FALSE，则套接字的行为正常。 默认值为禁用， (设置为 FALSE) 。 
    对于类型安全性，应使用 WSAGetFailConnectOnIcmpError 和 WSASetFailConnectOnIcmpError 函数，而不是直接使用套接字选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_FAIL_CONNECT_ON_ICMP_ERROR, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {//在 getsockopt 或 setsockopt 调用中指定的一个未知的、无效的或不受支持的选项或层次。
        DisplayError(WSAGetLastError());
    }

    /*
    检索 TCP 套接字在连接调用失败期间收到的 ICMP 错误的信息。 
    仅在以前启用了 TCP_FAIL_CONNECT_ON_ICMP_ERROR 且 连接 已返回 WSAEHOSTUNREACH 的 TCP 套接字上有效。
    查询是非阻塞的。 如果查询成功且返回的 optlen 值为 0，则自上次连接调用以来未收到任何 ICMP 错误。 
    如果收到 ICMP 错误，则其信息将可用，直到再次调用 连接 。 信息作为 ICMP_ERROR_INFO 结构返回。 
    对于类型安全，应使用 WSAGetIcmpErrorInfo 函数，而不是直接使用套接字选项。
    */
    ICMP_ERROR_INFO IcmpErrorInfo{};
    optLen = sizeof(ICMP_ERROR_INFO);
    iResult = getsockopt(s, SOL_SOCKET, TCP_ICMP_ERROR_INFO, reinterpret_cast<char *>(&IcmpErrorInfo), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());//WSAENOPROTOOPT
    }

    /*
    获取或设置在连接终止前将发送的 TCP 保持活动探测数。 将TCP_KEEPCNT设置为大于 255 的值是非法的。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_KEEPCNT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    如果此值为非负值，则表示所需的连接超时（以秒为单位）。
    如果为 -1，则表示请求禁用连接超时 (即连接将永久重新传输) 。 
    如果禁用连接超时，则每次重新传输的重新传输超时都会呈指数级增长，最大值为 60 秒，然后保留该超时。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_MAXRT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    启用或禁用 TCP 套接字的 Nagle 算法。 默认情况下，此选项 (设置为 FALSE) 禁用。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    启用或禁用 RFC 1323 时间戳。 
    请注意，时间戳的全局配置 (默认值为关闭) ， (set/get) -nettcpsetting 中的“时间戳”。 
    设置此套接字选项将覆盖该全局配置设置。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_TIMESTAMPS, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    启用或禁用 RFC 7413 TCP 快速打开，这使你可以在打开连接的三向握手阶段开始发送数据。
    请注意，若要利用快速打开，应使用 ConnectEx 进行初始连接，并在该函数的 lpSendBuffer 参数中指定要在握手过程中传输的数据。
    lpSendBuffer 中的某些数据将根据快速打开协议传输。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_FASTOPEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    获取或设置 TCP 连接在将保留探测发送到远程之前保持空闲状态的秒数。
    注意：此选项从 Windows 10 版本 1709 开始可用。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_KEEPIDLE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    获取或设置 TCP 连接在发送另一个 keepalive 探测之前等待保持响应的秒数。
    注意：此选项从 Windows 10 版本 1709 开始可用。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_KEEPINTVL, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    return ret;
}


int get_tcp_sock_opt()
/*

https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-tcp-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    _ASSERTE(iResult == NO_ERROR);

    SOCKET s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //还可以搞侦听绑定的。

    get_tcp_sock_opt(s4);
    get_tcp_sock_opt(s6);

    closesocket(s4);

    WSACleanup();

    return ret;
}


int get_udp_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-udp-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_ipx_sock_opt()
/*

当 level 参数设置为 NSPROTO_IPX 时，optname 参数的取值可以是IPX_PTYPE等。

https://learn.microsoft.com/zh-cn/windows/win32/winsock/nsproto-ipx-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_AppleTalk_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/sol-appletalk-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_irlmp_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/sol-irlmp-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_sol_sock_opt(_In_ SOCKET s)
{
    int ret = ERROR_SUCCESS;

    /*
    套接字正在侦听。
    */
    int optVal{};
    int optLen = sizeof(int);
    int iResult = getsockopt(s, SOL_SOCKET, SO_ACCEPTCONN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("SO_ACCEPTCONN: %ld\n", optVal);
    } else {
        printf("LastError:%d\n", WSAGetLastError());
    }

    /*
    套接字配置为用于传输和接收广播消息。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("SO_BROADCAST: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    返回套接字使用的本地地址、本地端口、远程地址、远程端口、套接字类型和协议。
    */
    CSADDR_INFO AddrInfo{};
    optLen = sizeof(CSADDR_INFO);
    iResult = getsockopt(s, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char *>(&AddrInfo), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    从上一次调用 setsockopt 或系统默认值返回当前套接字状态。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    返回已连接套接字的秒数。 此套接字选项仅适用于面向连接的协议。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONNECT_TIME, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    //已启用调试。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DEBUG, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //如果 为 TRUE，则禁用SO_LINGER选项。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DONTLINGER, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    路由功能被禁用。 
    此设置成功，但在AF_INET套接字上将被忽略;使用 WSAENOPROTOOPT AF_INET6套接字失败。 
    ATM 套接字不支持此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DONTROUTE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //检索错误状态并清除。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    阻止任何其他套接字绑定到同一地址和端口。 在调用 绑定 函数之前，必须设置此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //保留。
    GROUP Group{};
    optLen = sizeof(GROUP);
    iResult = getsockopt(s, SOL_SOCKET, SO_GROUP_ID, reinterpret_cast<char *>(&Group), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //保留。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_GROUP_PRIORITY, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //正在发送“保持连接”。 ATM 套接字不支持。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //返回当前的逗留选项。
    LINGER Linger{};
    optLen = sizeof(LINGER);
    iResult = getsockopt(s, SOL_SOCKET, SO_LINGER, reinterpret_cast<char *>(&Linger), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    面向消息的套接字类型的消息的最大大小 (例如，SOCK_DGRAM) 。 对于面向流的套接字没有意义。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    正在正常数据流中接收 OOB 数据。 (有关本主题的讨论，请参阅 Windows 套接字 1.1 阻止例程和 EINPROGRESS 部分。)
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_OOBINLINE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    通过为本地计算机上的不同本地地址端口对多次分配通配符端口，允许最大化端口分配，从而为套接字启用本地端口可伸缩性。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_PORT_SCALABILITY, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //绑定到此套接字的协议的协议信息的说明。
    WSAPROTOCOL_INFO ProtocalInfo{};
    optLen = sizeof(WSAPROTOCOL_INFO);
    iResult = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, reinterpret_cast<char *>(&ProtocalInfo), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    为接收保留的每个套接字的总缓冲区空间。 这与SO_MAX_MSG_SIZE无关，也不一定对应于 TCP 接收窗口的大小。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //可以将套接字绑定到已在使用中的地址。 不适用于 ATM 插座。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    为发送保留的每个套接字缓冲区总空间。 这与SO_MAX_MSG_SIZE无关，也不一定对应于 TCP 发送窗口的大小。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //套接字的类型 (例如，SOCK_STREAM) 。
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_TYPE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    与 套接字关联的服务提供程序中的不透明数据结构对象。 此对象存储服务提供程序的当前配置信息。 
    此数据结构的确切格式特定于服务提供程序。
    */
    //optLen = sizeof(int);
    //iResult = getsockopt(s, SOL_SOCKET, PVD_CONFIG, reinterpret_cast<char *>(&optVal), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

     /*
    从此套接字发送数据时，请使用本地环回地址。 仅当发送的所有数据也将在本地接收时，才应使用此选项。
    Windows TCP/IP 提供程序不支持此选项。
    如果在 Windows Vista 及更高版本上使用此选项， 则 getsockopt 和 setsockopt 函数将失败并显示 WSAEINVAL。
    在早期版本的 Windows 上，这些函数失败并显示 WSAENOPROTOOPT。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_USELOOPBACK, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    包含 BSD UNIX 中的套接字选项，用于向后兼容。
    此选项设置要处理的套接字输出操作的最小字节数。 Windows TCP/IP 提供程序不支持此选项。
    如果在 Windows Vista 及更高版本上使用此选项， 则 getsockopt 和 setsockopt 函数将失败并出现 WSAEINVAL。
    在早期版本的 Windows 上，这些函数失败并出现 WSAENOPROTOOPT。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_SNDLOWAT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    包含 BSD UNIX 中的套接字选项，用于向后兼容。 此选项设置套接字输入操作要处理的最小字节数。
    Windows TCP/IP 提供程序不支持此选项。
    如果在 Windows Vista 及更高版本上使用此选项， 则 getsockopt 和 setsockopt 函数将失败并出现 WSAEINVAL。
    在早期版本的 Windows 上，这些函数失败并出现 WSAENOPROTOOPT。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RCVLOWAT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    阻止发送调用的超时（以毫秒为单位）。
    此选项的默认值为零，表示发送操作不会超时。如果阻止发送调用超时，则连接处于不确定状态，应关闭。
    如果使用 WSASocket 函数创建套接字，则 dwFlags 参数必须设置 WSA_FLAG_OVERLAPPED 属性才能使超时正常工作。
    否则，超时永远不会生效。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    阻止接收调用的超时（以毫秒为单位）。
    此选项的默认值为零，表示接收操作不会超时。如果阻止接收调用超时，则连接处于不确定状态，应关闭。
    如果使用 WSASocket 函数创建套接字，则 dwFlags 参数必须设置 WSA_FLAG_OVERLAPPED 属性才能使超时正常工作。
    否则，超时永远不会生效。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    //返回给定套接字 的WSAPROTOCOL_INFOA 结构
    WSAPROTOCOL_INFOA ProtocalInfoA{};
    optLen = sizeof(WSAPROTOCOL_INFOA);
    iResult = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFOA, reinterpret_cast<char *>(&ProtocalInfoA), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    //返回给定套接字 的WSAPROTOCOL_INFOW 结构
    WSAPROTOCOL_INFOW ProtocalInfoW{};
    optLen = sizeof(WSAPROTOCOL_INFOW);
    iResult = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFOW, reinterpret_cast<char *>(&ProtocalInfoW), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    return ret;
}


int get_sol_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/sol-socket-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    _ASSERTE(iResult == NO_ERROR);

    SOCKET s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //还可以搞侦听绑定的。

    get_sol_sock_opt(s4);
    get_sol_sock_opt(s6);

    closesocket(s4);

    WSACleanup();

    return ret;
}


int sock(int argc, wchar_t * argv[])
/*
目的：获取套接字的属性，详尽使用getsockopt。

https://learn.microsoft.com/zh-cn/windows/win32/api/winsock/nf-winsock-getsockopt
https://learn.microsoft.com/zh-cn/windows/win32/winsock/socket-options
*/
{
    int ret = ERROR_SUCCESS;

    if (argc == 1) {
        return Usage(argv[0]);
    }

    if (argc == 2 && lstrcmpi(argv[1], TEXT("IPv4")) == 0) {  
        ret = get_ipv4_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("IPv6")) == 0) {
        ret = get_ipv6_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("rm")) == 0) {
        ret = get_rm_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("tcp")) == 0) {
        ret = get_tcp_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("udp")) == 0) {
        ret = get_udp_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("ipx")) == 0) {
        ret = get_ipx_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("AppleTalk")) == 0) {
        ret = get_AppleTalk_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("irlmp")) == 0) {
        ret = get_irlmp_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("sol")) == 0) {
        ret = get_sol_sock_opt();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("all")) == 0) {
        ret = get_all_sock_opt();
    } else {
        ret = Usage(argv[0]);
    }

    return ret;
}