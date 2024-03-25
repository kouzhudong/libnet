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

��IDA����levelֻ��ȡSOL_SOCKET��
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

    //�����Ը������󶨵ġ�

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
    ��� Ϊ TRUE��������ṩ��ʵ�� Berkeley Software Distribution (BSD) ��ʽ�� (���ڴ���������ݵ�Ĭ��) ��
    ��ѡ���� TCP_EXPEDITED_1122 ѡ���෴�� ��ѡ��ֻ��������������һ�Ρ� ���ô�ѡ����޷��رմ�ѡ� 
    �����ṩ�̲���Ҫʵ�ִ�ѡ� Ĭ������£� (����Ϊ TRUE) ���ô�ѡ�
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
    ��� Ϊ TRUE��������ṩ��ʵ�� RFC-1222 ��ָ���ļ������ݡ� 
    ���򣬽�ʹ�� Berkeley Software Distribution (BSD) ��ʽ (Ĭ��) �� 
    ��ѡ��ֻ��������������һ�Ρ�
    ���ô�ѡ����޷��رմ�ѡ� �����ṩ�̲���Ҫʵ�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_EXPEDITED_1122, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��� Ϊ TRUE�����յ�ֵΪ WSAEHOSTUNREACH �� ICMP ����ʱ������������ API ���á�
    Ȼ�󣬽�ͨ����TCP_ICMP_ERROR_INFO�׽��֡�ѡ���ȡ�����Դ��ַ�� 
    ��� Ϊ FALSE�����׽��ֵ���Ϊ������ Ĭ��ֵΪ���ã� (����Ϊ FALSE) �� 
    �������Ͱ�ȫ�ԣ�Ӧʹ�� WSAGetFailConnectOnIcmpError �� WSASetFailConnectOnIcmpError ������������ֱ��ʹ���׽���ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_FAIL_CONNECT_ON_ICMP_ERROR, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {//�� getsockopt �� setsockopt ������ָ����һ��δ֪�ġ���Ч�Ļ���֧�ֵ�ѡ����Ρ�
        DisplayError(WSAGetLastError());
    }

    /*
    ���� TCP �׽��������ӵ���ʧ���ڼ��յ��� ICMP �������Ϣ�� 
    ������ǰ������ TCP_FAIL_CONNECT_ON_ICMP_ERROR �� ���� �ѷ��� WSAEHOSTUNREACH �� TCP �׽�������Ч��
    ��ѯ�Ƿ������ġ� �����ѯ�ɹ��ҷ��ص� optlen ֵΪ 0�������ϴ����ӵ�������δ�յ��κ� ICMP ���� 
    ����յ� ICMP ����������Ϣ�����ã�ֱ���ٴε��� ���� �� ��Ϣ��Ϊ ICMP_ERROR_INFO �ṹ���ء� 
    �������Ͱ�ȫ��Ӧʹ�� WSAGetIcmpErrorInfo ������������ֱ��ʹ���׽���ѡ�
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
    ��ȡ��������������ֹǰ�����͵� TCP ���ֻ̽������ ��TCP_KEEPCNT����Ϊ���� 255 ��ֵ�ǷǷ��ġ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_KEEPCNT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �����ֵΪ�Ǹ�ֵ�����ʾ��������ӳ�ʱ������Ϊ��λ����
    ���Ϊ -1�����ʾ����������ӳ�ʱ (�����ӽ��������´���) �� 
    ����������ӳ�ʱ����ÿ�����´�������´��䳬ʱ�����ָ�������������ֵΪ 60 �룬Ȼ�����ó�ʱ��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_MAXRT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���û���� TCP �׽��ֵ� Nagle �㷨�� Ĭ������£���ѡ�� (����Ϊ FALSE) ���á�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���û���� RFC 1323 ʱ����� 
    ��ע�⣬ʱ�����ȫ������ (Ĭ��ֵΪ�ر�) �� (set/get) -nettcpsetting �еġ�ʱ������� 
    ���ô��׽���ѡ����Ǹ�ȫ���������á�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_TIMESTAMPS, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���û���� RFC 7413 TCP ���ٴ򿪣���ʹ������ڴ����ӵ��������ֽ׶ο�ʼ�������ݡ�
    ��ע�⣬��Ҫ���ÿ��ٴ򿪣�Ӧʹ�� ConnectEx ���г�ʼ���ӣ����ڸú����� lpSendBuffer ������ָ��Ҫ�����ֹ����д�������ݡ�
    lpSendBuffer �е�ĳЩ���ݽ����ݿ��ٴ�Э�鴫�䡣
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_FASTOPEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ȡ������ TCP �����ڽ�����̽�ⷢ�͵�Զ��֮ǰ���ֿ���״̬��������
    ע�⣺��ѡ��� Windows 10 �汾 1709 ��ʼ���á�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, TCP_KEEPIDLE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("BSD Urgent: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ȡ������ TCP �����ڷ�����һ�� keepalive ̽��֮ǰ�ȴ�������Ӧ��������
    ע�⣺��ѡ��� Windows 10 �汾 1709 ��ʼ���á�
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

    //�����Ը������󶨵ġ�

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


int get_sol_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/sol-socket-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int sock(int argc, wchar_t * argv[])
/*
Ŀ�ģ���ȡ�׽��ֵ����ԣ��꾡ʹ��getsockopt��

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