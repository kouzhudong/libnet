#include "PortReuse.h"

#pragma warning(disable : 6386)

/*
import socket
import threading

def handle_client_connection(client_socket):
    while True:
        try:
            request = client_socket.recv(1024)
            if not request:
                break
            print(f"Received: {request.decode('utf-8')}")
            client_socket.send("ACK".encode('utf-8'))
        except ConnectionResetError:
            break
    client_socket.close()

def create_server_socket(port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)  #这里发生异常。WinSock2.h里无此定义。
    server_socket.bind(('0.0.0.0', port))
    server_socket.listen(5)
    print(f"Server is listening on port {port}")
    return server_socket

def main():
    port = 12345
    server_socket = create_server_socket(port)

    while True:
        client_socket, addr = server_socket.accept()
        print(f"Accepted connection from {addr}")
        client_handler = threading.Thread(target=handle_client_connection, args=(client_socket,))
        client_handler.start()

if __name__ == "__main__":
    main()
*/

void handle_client(SOCKET client_socket)
/*
如何走这里呢？侦听这个端口的程序有两个，难道是IP，都是一样的IP呢？如：INADDR_ANY。
*/
{
    char buffer[1024]{};
    int bytes_received{};

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';//
        printf("Received: %s\n", buffer);
        send(client_socket, "ACK", 3, 0);
    }

    closesocket(client_socket);
}

int PortReuse()
/*
在 Windows 上进行端口复用（Port Reuse）可以通过设置 SO_REUSEADDR 和 SO_EXCLUSIVEADDRUSE 选项来实现。
以下是一个使用 C 语言和 Winsock 库的示例，展示了如何在 Windows 上实现端口复用。

代码解释
1.初始化 Winsock：使用 WSAStartup 初始化 Winsock 库。
2.创建服务器 socket：通过 socket(AF_INET, SOCK_STREAM, 0) 创建一个 TCP socket。
3.设置端口复用选项：使用 setsockopt 设置 SO_REUSEADDR 和 SO_EXCLUSIVEADDRUSE 选项，以允许端口复用。
4.绑定和监听：将 socket 绑定到指定的端口，并开始监听连接。
5.处理连接：接受客户端连接，并处理每个客户端的请求。

注意事项
1.SO_EXCLUSIVEADDRUSE 选项在某些平台上可能不受支持，因此需要测试环境的支持情况。
2.端口复用主要用于高可用性和负载均衡场景，在实际使用中需要结合具体需求和环境进行配置。
3.示例中的 handle_client 函数用于处理客户端连接，接收数据并发送响应。

这个示例展示了如何在 Windows 上实现端口复用功能，可以根据需要进行扩展和修改。
如果您有任何问题或需要进一步的帮助，请告诉我。
*/
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c, opt = 1;

    // 初始化 Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Initialized.\n");

    // 创建 socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("Socket created.\n");

    // 设置 socket 选项以允许端口复用
    // Windows没有SO_REUSEPORT。
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    //这个应该注释掉。
    // #define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR))
    // 看看这个定义和说明，应该这两个不能同时调用。
    //if (setsockopt(server_socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&opt, sizeof(opt)) < 0) {
    //    printf("setsockopt(SO_EXCLUSIVEADDRUSE) failed: %d\n", WSAGetLastError());
    //    closesocket(server_socket);
    //    WSACleanup();
    //    return 1;
    //}

    // 准备 sockaddr_in 结构体
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1080);//8888 确保这个端口已经处于侦听状态。

    // 绑定 socket
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Bind done.\n");

    // 监听连接
    listen(server_socket, 3);

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    //在这里用tcpview.exe等类似的工具，可以看到有两个进程侦听相同的端口。且本地IPv4也一样。

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Connection accepted.\n");
        handle_client(client_socket);
    }

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}


int __cdecl EXCLUSIVEADDRUSE(int argc, wchar_t ** argv)
/*
SO_EXCLUSIVEADDRUSE套接字选项可防止其他套接字被强行绑定到同一地址和端口。

语法
SO_EXCLUSIVEADDRUSE选项可防止其他套接字被强行绑定到同一地址和端口，这是SO_REUSEADDR套接字选项启用的做法。
恶意应用程序可能会执行此类重用，以破坏应用程序。 SO_EXCLUSIVEADDRUSE选项对于需要高可用性的系统服务非常有用。

下面的代码示例演示了如何设置此选项。

若要产生任何效果，必须在调用 绑定 函数之前设置SO_EXCLUSIVADDRUSE选项， (这也适用于) SO_REUSEADDR选项。 表 1
列出了设置 SO_EXCLUSIVEADDRUSE 选项的效果。 通配符指示绑定到通配符地址，例如 0.0.0.0（对于 IPv4）和 ：：（对于
IPv6）。 Specific 指示绑定到特定接口，例如绑定分配给适配器的 IP 地址。 Specific2
指示绑定到特定地址，而不是特定情况下绑定到的地址。

 备注

仅当使用特定地址执行第一个绑定时，Specific2 才适用;对于第一个套接字绑定到通配符的情况，则 Specific
的条目涵盖所有特定的地址事例。

例如，假设有两个 IP 接口的计算机：10.0.0.1 和 10.99.99.99。 如果第一个绑定是 10.0.0.1 和端口 5150，并且设置了
SO_EXCLUSIVEADDRUSE 选项，则第二个绑定到 10.99.99.99 和端口 5150 并成功设置了任何选项或未设置任何选项。
但是，如果第一个套接字绑定到通配符地址 (0.0.0.0) ，并且端口 5150 设置了SO_EXCLUSIVEADDRUSE， 与 WSAEADDRINUSE
(10048) 或 WSAEACCESS (10013) （无论 IP 地址如何）的任何后续绑定都将失败，具体取决于第二个绑定套接字上设置的选项。

如果第一个绑定未设置任何选项或SO_REUSEADDR，并且第二个绑定执行SO_REUSEADDR，则第二个套接字已超过端口，并且有关哪个套接字将接收数据包的行为不确定。
引入了SO_EXCLUSIVEADDRUSE来解决此问题。

设置SO_EXCLUSIVEADDRUSE的套接字在套接字关闭后不能始终立即重复使用。
例如，如果设置了独占标志的侦听套接字接受连接，在该连接之后将关闭侦听套接字，则另一个套接字无法绑定到与具有独占标志的第一个侦听套接字相同的端口，直到接受的连接不再处于活动状态。

这种情况可能相当复杂：即使套接字已关闭，基础传输也可能不会终止其连接。
即使在套接字关闭后，系统也必须发送所有缓冲数据，将正常断开连接传输到对等方，并等待与对等方正常断开连接。
因此，基础传输可能永远不会释放连接，例如当对等播发零大小的窗口或其他此类攻击时。
在前面的示例中，侦听套接字在接受客户端连接后关闭。
现在，即使客户端连接已关闭，如果客户端连接由于未确认的数据等而保持活动状态，则仍可能不重复使用该端口。

若要避免这种情况，应用程序应确保正常关闭：使用 SD_SEND 标志调用 shutdown ，然后在 recv
循环中等待，直到返回零个字节。
这样做可避免与端口重用相关的问题，保证对等方接收所有数据，并保证对等方成功接收其所有数据。

可以在套接字上设置SO_LINGER选项，以防止端口进入活动等待状态之一;但是，不建议这样做，因为这可能会导致意外的影响，因为它可能会导致连接重置。
例如，如果已收到数据但尚未被对等方确认，并且本地计算机关闭了SO_LINGER设置的套接字，则会重置连接，并且对等方会丢弃未确认的数据。
此外，挑选合适的时间挥之不去是困难的：值过小会导致许多连接中止，而长时间超时可能会使系统容易受到拒绝服务攻击，方法是建立许多连接，从而停止大量应用程序线程。

 备注

使用 SO_EXCLUSIVEADDRUSE 选项的套接字在关闭之前必须正确关闭。 如果关联服务需要重启，否则可能会导致拒绝服务攻击。

https://learn.microsoft.com/zh-cn/windows/win32/winsock/so-exclusiveaddruse
https://learn.microsoft.com/zh-tw/windows/win32/winsock/using-so-reuseaddr-and-so-exclusiveaddruse
*/
{
    WSADATA wsaData;
    int iResult = 0;
    int iError = 0;

    SOCKET s = INVALID_SOCKET;
    SOCKADDR_IN saLocal;
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
        wprintf(L"   where AF_UNSPEC=0 SOCK_DGRAM=2 IPPROTO_UDP=17  PORT=5150\n");//, argv[0]
        wprintf(L"   %ws 2 1 17 5150\n", argv[0]);
        wprintf(L"   where AF_INET=2 SOCK_STREAM=1 IPPROTO_TCP=6  PORT=5150\n");//, argv[0]
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

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Create the socket
    s = socket(iFamily, iType, iProtocol);
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

    // Bind the socket
    iResult = bind(s, (SOCKADDR *)&saLocal, sizeof(saLocal));
    if (iResult == SOCKET_ERROR) {

        // Most errors related to setting SO_EXCLUSIVEADDRUSE
        //    will occur at bind.
        iError = WSAGetLastError();
        if (iError == WSAEACCES)
            wprintf(L"bind failed with WSAEACCES (access denied)\n");
        else
            wprintf(L"bind failed with error: %ld\n", iError);//已经存在的端口，走这里，返回值10048。

    } else {
        wprintf(L"bind on socket with SO_EXCLUSIVEADDRUSE succeeded to port: %ld\n", iPort);
    }

    // cleanup
    closesocket(s);
    WSACleanup();

    return 0;
}