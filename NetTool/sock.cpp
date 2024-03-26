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
下表介绍了IPPROTO_RM套接字选项，这些选项适用于为 IPv4 地址系列 (AF_INET) 创建的套接字，
这些套接字具有指定为可靠多播 (IPPROTO_RM) 的套接字函数的协议参数。

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


int get_udp_sock_opt(_In_ SOCKET s)
{
    int ret = ERROR_SUCCESS;

    /*
    如果 为 TRUE，则 UDP 数据报会随校验和一起发送。
    */
    int optVal{};
    int optLen = sizeof(int);
    int iResult = getsockopt(s, SOL_SOCKET, UDP_CHECKSUM_COVERAGE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    如果 为 TRUE，则发送 UDP 数据报，校验和为零。 对于服务提供商是必需的。 
    如果服务提供商没有禁用 UDP 校验和计算的机制，它可能只存储此选项而不执行任何操作。 IPv6 不支持此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, UDP_NOCHECKSUM, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    设置为非零值时，多个接收的数据报可能会在向应用程序指示之前合并成单个消息缓冲区。 
    选项值表示可以向应用程序指示的合并消息的最大消息大小（以字节为单位）。 
    可能仍会指示大于选项值的未合并消息。 默认值为 0 (无合并) 。 
    仅当数据报源自同一源地址和端口时，才会合并数据报。 
    合并的所有数据报的大小都相同，但最后一个数据报可能更小。 
    如果应用程序想要检索除最后一个数据报以外的 (数据报大小（) 可能有所不同），
    则必须使用支持控制信息 (的接收 API，例如 LPFN_WSARECVMSG (WSARecvMsg) ) 。 
    除最后一条消息之外的所有消息的大小都可以在 UDP_COALESCED_INFO 控件消息中找到，该消息的类型为 DWORD。 
    为保护类型，应用程序应直接使用 WSAGetUdpRecvMaxCoalescedSize 和 WSASetUdpRecvMaxCoalescedSize 函数，
    而不是直接使用套接字选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, UDP_RECV_MAX_COALESCED_SIZE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    当设置为非零值时，应用程序发送的缓冲区将按网络堆栈细分为多个消息。 选项值表示每个分解消息的大小。 
    选项值以字节表示。 最后一个段的大小可能小于选项的值。 默认值为 0 (无分段) 。 
    应用程序应设置一个小于目标路径的 MTU 的值， () ，以避免 IP 碎片。 
    为保护类型，应用程序应使用 WSAGetUdpSendMessageSize 和 WSASetUdpSendMessageSize 函数，而不是直接使用套接字选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, UDP_SEND_MSG_SIZE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    return ret;
}


int get_udp_sock_opt()
/*
下表描述了 IPPROTO_UDP 套接字选项，这些选项适用于为 IPv4 和 IPv6 地址系列创建的套接字，
(AF_INET并将 协议 参数AF_INET6) 指定为 UDP (IPPROTO_UDP) 的 套接字 函数。

https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-udp-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    WSADATA wsaData{};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    _ASSERTE(iResult == NO_ERROR);

    SOCKET s4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //还可以搞侦听绑定的。

    get_udp_sock_opt(s4);
    get_udp_sock_opt(s6);

    closesocket(s4);

    WSACleanup();

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

    /*
    其他数据（不在正常网络数据流中）与网络请求一起发送以建立连接。 旧版协议（如 DECNet、OSI TP4 等）使用此选项。 
    Windows 中的 TCP/IP 协议不支持此选项。
    */
    //SO_CONNDATA


    /*
    附加数据的长度，以字节为单位，而不是在正常网络数据流中，与网络请求一起发送以建立连接。
    旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONNDATALEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    其他连接选项数据（不在正常网络数据流中）与网络请求一起发送以建立连接。 
    旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    //SO_CONNOPT


    /*
    连接选项数据的长度（以字节为单位），而不是在正常网络数据流中，该数据随网络请求一起发送以建立连接。 
    旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONNOPTLEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    其他数据（不在正常网络数据流中）与网络请求一起发送，以断开连接。
    旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    //SO_DISCDATA


    /*
    	附加数据的长度（以字节为单位），该长度不是在正常网络数据流中，随网络请求一起发送，以断开连接。 
        旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DISCDATALEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    其他断开连接选项数据，而不是在正常网络数据流中，随网络请求一起发送，以断开连接。 
    旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    //SO_DISCOPT

    /*
    附加断开连接选项数据（而不是在正常网络数据流中）的长度（以字节为单位），该数据随网络请求一起发送以断开连接。
    旧版协议（如 DECNet、OSI TP4 等）使用此选项。 Windows 中的 TCP/IP 协议不支持此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DISCOPTLEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    返回协议支持的出站数据报的最大大小（以字节为单位）。 此套接字选项对于面向流的套接字没有意义。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_MAXDG, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    返回协议支持到给定目标地址的出站数据报的最大大小（以字节为单位）。 此套接字选项对于面向流的套接字没有意义。
    Microsoft 提供商可能会以无提示方式将此视为SO_MAXDG。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_MAXPATHDG, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    设置后，将影响创建的后续套接字是否不重叠。 此选项的可能值为SO_SYNCHRONOUS_ALERT和SO_SYNCHRONOUS_NONALERT。 
    不应使用此选项。 请改用 WSASocket 函数并关闭 dwFlags 参数中的WSA_FLAG_OVERLAPPED位。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_OPENTYPE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    使用此选项侦听套接字。 设置 选项后，套接字将使用 RST 响应所有传入连接，而不是接受它们。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_PAUSE_ACCEPT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    应在未绑定的套接字上设置此选项。 设置SO_RANDOMIZE_PORT并在套接字上选择临时端口时，将绑定随机端口号。
    使用 SO_REUSE_UNICASTPORT) 选择的端口 (自动重用端口也会随机化返回的端口，因此，如果应用程序SO_REUSE_UNICASTPORT然后尝试设置SO_RANDOMIZE_PORT，则第二 个 setockopt 调用将失败。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RANDOMIZE_PORT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    设置后，允许 Winsock API 连接函数重复使用需要显式绑定的临时端口，例如 ConnectEx。 
    请注意，具有隐式绑定 (的连接函数（例如，在没有显式绑定的情况下进行连接) 默认设置了此选项。 
    使用此选项，而不是在两者都可用的平台上 SO_PORT_SCALABILITY 。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_REUSE_UNICASTPORT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    在套接字上设置此选项时，表示永远不会使用该套接字接收单播数据包，因此，其端口可以与其他仅多播应用程序共享。
    将值设置为 1 可始终在端口上共享多播流量。 将值设置为 0 (默认) 会禁用此行为。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_REUSE_MULTICASTPORT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    此选项与 AcceptEx 函数一起使用。 此选项更新从侦听套接字继承的套接字的属性。 
    如果要在接受的套接字上使用 getpeername、 getockname、 getockopt 或 setsockopt 函数，则应设置此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    此选项与 ConnectEx、 WSAConnectByList 和 WSAConnectByName 函数一起使用。 建立连接后，此选项将更新套接字的属性。
    如果要在连接的套接字上使用 getpeername、 getockname、 getockopt、 setsockopt 或 shutdown 函数，则应设置此选项。
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, reinterpret_cast<char *>(&optVal), &optLen);
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