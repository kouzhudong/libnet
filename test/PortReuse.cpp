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
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)  #���﷢���쳣��WinSock2.h���޴˶��塣
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
����������أ���������˿ڵĳ������������ѵ���IP������һ����IP�أ��磺INADDR_ANY��
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
�� Windows �Ͻ��ж˿ڸ��ã�Port Reuse������ͨ������ SO_REUSEADDR �� SO_EXCLUSIVEADDRUSE ѡ����ʵ�֡�
������һ��ʹ�� C ���Ժ� Winsock ���ʾ����չʾ������� Windows ��ʵ�ֶ˿ڸ��á�

�������
1.��ʼ�� Winsock��ʹ�� WSAStartup ��ʼ�� Winsock �⡣
2.���������� socket��ͨ�� socket(AF_INET, SOCK_STREAM, 0) ����һ�� TCP socket��
3.���ö˿ڸ���ѡ�ʹ�� setsockopt ���� SO_REUSEADDR �� SO_EXCLUSIVEADDRUSE ѡ�������˿ڸ��á�
4.�󶨺ͼ������� socket �󶨵�ָ���Ķ˿ڣ�����ʼ�������ӡ�
5.�������ӣ����ܿͻ������ӣ�������ÿ���ͻ��˵�����

ע������
1.SO_EXCLUSIVEADDRUSE ѡ����ĳЩƽ̨�Ͽ��ܲ���֧�֣������Ҫ���Ի�����֧�������
2.�˿ڸ�����Ҫ���ڸ߿����Ժ͸��ؾ��ⳡ������ʵ��ʹ������Ҫ��Ͼ�������ͻ����������á�
3.ʾ���е� handle_client �������ڴ���ͻ������ӣ��������ݲ�������Ӧ��

���ʾ��չʾ������� Windows ��ʵ�ֶ˿ڸ��ù��ܣ����Ը�����Ҫ������չ���޸ġ�
��������κ��������Ҫ��һ���İ�����������ҡ�
*/
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c, opt = 1;

    // ��ʼ�� Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Initialized.\n");

    // ���� socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("Socket created.\n");

    // ���� socket ѡ��������˿ڸ���
    // Windowsû��SO_REUSEPORT��
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    //���Ӧ��ע�͵���
    // #define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR))
    // ������������˵����Ӧ������������ͬʱ���á�
    //if (setsockopt(server_socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&opt, sizeof(opt)) < 0) {
    //    printf("setsockopt(SO_EXCLUSIVEADDRUSE) failed: %d\n", WSAGetLastError());
    //    closesocket(server_socket);
    //    WSACleanup();
    //    return 1;
    //}

    // ׼�� sockaddr_in �ṹ��
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1080);//8888 ȷ������˿��Ѿ���������״̬��

    // �� socket
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Bind done.\n");

    // ��������
    listen(server_socket, 3);

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    //��������tcpview.exe�����ƵĹ��ߣ����Կ�������������������ͬ�Ķ˿ڡ��ұ���IPv4Ҳһ����

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
SO_EXCLUSIVEADDRUSE�׽���ѡ��ɷ�ֹ�����׽��ֱ�ǿ�а󶨵�ͬһ��ַ�Ͷ˿ڡ�

�﷨
SO_EXCLUSIVEADDRUSEѡ��ɷ�ֹ�����׽��ֱ�ǿ�а󶨵�ͬһ��ַ�Ͷ˿ڣ�����SO_REUSEADDR�׽���ѡ�����õ�������
����Ӧ�ó�����ܻ�ִ�д������ã����ƻ�Ӧ�ó��� SO_EXCLUSIVEADDRUSEѡ�������Ҫ�߿����Ե�ϵͳ����ǳ����á�

����Ĵ���ʾ����ʾ��������ô�ѡ�

��Ҫ�����κ�Ч���������ڵ��� �� ����֮ǰ����SO_EXCLUSIVADDRUSEѡ� (��Ҳ������) SO_REUSEADDRѡ� �� 1
�г������� SO_EXCLUSIVEADDRUSE ѡ���Ч���� ͨ���ָʾ�󶨵�ͨ�����ַ������ 0.0.0.0������ IPv4���� ����������
IPv6���� Specific ָʾ�󶨵��ض��ӿڣ�����󶨷������������ IP ��ַ�� Specific2
ָʾ�󶨵��ض���ַ���������ض�����°󶨵��ĵ�ַ��

 ��ע

����ʹ���ض���ִַ�е�һ����ʱ��Specific2 ������;���ڵ�һ���׽��ְ󶨵�ͨ������������ Specific
����Ŀ���������ض��ĵ�ַ������

���磬���������� IP �ӿڵļ������10.0.0.1 �� 10.99.99.99�� �����һ������ 10.0.0.1 �Ͷ˿� 5150������������
SO_EXCLUSIVEADDRUSE ѡ���ڶ����󶨵� 10.99.99.99 �Ͷ˿� 5150 ���ɹ��������κ�ѡ���δ�����κ�ѡ�
���ǣ������һ���׽��ְ󶨵�ͨ�����ַ (0.0.0.0) �����Ҷ˿� 5150 ������SO_EXCLUSIVEADDRUSE�� �� WSAEADDRINUSE
(10048) �� WSAEACCESS (10013) ������ IP ��ַ��Σ����κκ����󶨶���ʧ�ܣ�����ȡ���ڵڶ������׽��������õ�ѡ�

�����һ����δ�����κ�ѡ���SO_REUSEADDR�����ҵڶ�����ִ��SO_REUSEADDR����ڶ����׽����ѳ����˿ڣ������й��ĸ��׽��ֽ��������ݰ�����Ϊ��ȷ����
������SO_EXCLUSIVEADDRUSE����������⡣

����SO_EXCLUSIVEADDRUSE���׽������׽��ֹرպ���ʼ�������ظ�ʹ�á�
���磬��������˶�ռ��־�������׽��ֽ������ӣ��ڸ�����֮�󽫹ر������׽��֣�����һ���׽����޷��󶨵�����ж�ռ��־�ĵ�һ�������׽�����ͬ�Ķ˿ڣ�ֱ�����ܵ����Ӳ��ٴ��ڻ״̬��

������������൱���ӣ���ʹ�׽����ѹرգ���������Ҳ���ܲ�����ֹ�����ӡ�
��ʹ���׽��ֹرպ�ϵͳҲ���뷢�����л������ݣ��������Ͽ����Ӵ��䵽�Եȷ������ȴ���Եȷ������Ͽ����ӡ�
��ˣ��������������Զ�����ͷ����ӣ����統�ԵȲ������С�Ĵ��ڻ��������๥��ʱ��
��ǰ���ʾ���У������׽����ڽ��ܿͻ������Ӻ�رա�
���ڣ���ʹ�ͻ��������ѹرգ�����ͻ�����������δȷ�ϵ����ݵȶ����ֻ״̬�����Կ��ܲ��ظ�ʹ�øö˿ڡ�

��Ҫ�������������Ӧ�ó���Ӧȷ�������رգ�ʹ�� SD_SEND ��־���� shutdown ��Ȼ���� recv
ѭ���еȴ���ֱ����������ֽڡ�
�������ɱ�����˿�������ص����⣬��֤�Եȷ������������ݣ�����֤�Եȷ��ɹ��������������ݡ�

�������׽���������SO_LINGERѡ��Է�ֹ�˿ڽ����ȴ�״̬֮һ;���ǣ�����������������Ϊ����ܻᵼ�������Ӱ�죬��Ϊ�����ܻᵼ���������á�
���磬������յ����ݵ���δ���Եȷ�ȷ�ϣ����ұ��ؼ�����ر���SO_LINGER���õ��׽��֣�����������ӣ����ҶԵȷ��ᶪ��δȷ�ϵ����ݡ�
���⣬��ѡ���ʵ�ʱ���֮��ȥ�����ѵģ�ֵ��С�ᵼ�����������ֹ������ʱ�䳬ʱ���ܻ�ʹϵͳ�����ܵ��ܾ����񹥻��������ǽ���������ӣ��Ӷ�ֹͣ����Ӧ�ó����̡߳�

 ��ע

ʹ�� SO_EXCLUSIVEADDRUSE ѡ����׽����ڹر�֮ǰ������ȷ�رա� �������������Ҫ������������ܻᵼ�¾ܾ����񹥻���

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
            wprintf(L"bind failed with error: %ld\n", iError);//�Ѿ����ڵĶ˿ڣ����������ֵ10048��

    } else {
        wprintf(L"bind on socket with SO_EXCLUSIVEADDRUSE succeeded to port: %ld\n", iPort);
    }

    // cleanup
    closesocket(s);
    WSACleanup();

    return 0;
}