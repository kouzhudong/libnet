#include "sock.h"
#include "common_utils.h"


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

    WinsockInitializer winsock(MAKEWORD(2, 2));
    _ASSERTE(winsock.IsInitialized());
    

    SOCKET s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //�����Ը������󶨵ġ�

    get_all_sock_opt(s4);
    get_all_sock_opt(s6);

    closesocket(s4);

    // Winsock cleanup is handled automatically by WinsockInitializer destructor

    return ret;
}


int get_ipv4_sock_opt(_In_ SOCKET s, _In_ int level)
/*

�еĵط�ȡֵ��IPPROTO_IP��

https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-ip-socket-options
https://learn.microsoft.com/zh-cn/windows/win32/winsock/ip-pktinfo
*/
{
    int ret = ERROR_SUCCESS;

    /*
    ���ӿ��������ӵ��� IP_IFLIST ѡ������� IFLIST��
    ���ֻ֧�����ò�֧�ֻ�ȡ��
    */
    int optVal{};//IF_INDEX
    int optLen = sizeof(int);
    int iResult = getsockopt(s, level, IP_ADD_IFLIST, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���׽��ּ���ָ���ӿ����ṩ�Ķಥ�顣
    ���ֻ֧�����ò�֧�ֻ�ȡ��
    */
    ip_mreq tmp{};
    //optLen = sizeof(ip_mreq);
    //iResult = getsockopt(s, level, IP_ADD_MEMBERSHIP, reinterpret_cast<char *>(&tmp), &optLen);
    //if (iResult != SOCKET_ERROR) {
    //    
    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    /*
    ��������ӿ����ṩ�Ķಥ�飬�����������ṩ��Դ��ַ�����ݡ�
    ���ֻ֧�����ò�֧�ֻ�ȡ��
    */
    ip_mreq_source IpMreqSource{};
    //optLen = sizeof(ip_mreq_source);
    //iResult = getsockopt(s, level, IP_ADD_SOURCE_MEMBERSHIP, reinterpret_cast<char *>(&IpMreqSource), &optLen);
    //if (iResult != SOCKET_ERROR) {
    //     
    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    //ɾ������Դ��Ϊ�ṩ�Ķಥ��ͽӿڵķ��ͷ���
    //���ֻ֧�����ò�֧�ֻ�ȡ��
    //optLen = sizeof(ip_mreq_source);
    //iResult = getsockopt(s, level, IP_BLOCK_SOURCE, reinterpret_cast<char *>(&IpMreqSource), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    /*
    ���� IP_IFLIST ѡ������� IFLIST ��ɾ���ӿ������� 
    ��Ŀֻ����Ӧ�ó���ɾ���������ע�⣬һ��ɾ���ӿڣ���Ŀ���ܻ��ʱ��
    ���ֻ֧�����ò�֧�ֻ�ȡ��
    */
    //optLen = sizeof(int);
    //iResult = getsockopt(s, level, IP_DEL_IFLIST, reinterpret_cast<char *>(&optVal), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    /*
    ָʾ���۱��� MTU ��Σ�����Ӧ�����ݽ�����Ƭ������ ����������Ϣ��Э����Ч��
    ���� UDP �� ICMP��Microsoft TCP/IP �ṩ������ѭ��ѡ�
    �����֧�ֻ�ȡҲ֧�����á�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_DONTFRAGMENT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �뿪ָ���ӿ��е�ָ���ಥ�顣 ֧�ֶಥʱ�������ṩ�̱���֧�ִ�ѡ� 
    ֧���� WSAEnumProtocols �������÷��ص�WSAPROTOCOL_INFO�ṹ��ָʾ��XPI_SUPPORT_MULTIPOINT=1��XP1_MULTIPOINT_CONTROL_PLANE=0��XP1_MULTIPOINT_DATA_PLANE=0��
    */
    //���ֻ֧�����ò�֧�ֻ�ȡ��
    //optLen = sizeof(ip_mreq);
    //iResult = getsockopt(s, level, IP_DROP_MEMBERSHIP, reinterpret_cast<char *>(&tmp), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    //ɾ�������ಥ�顢�ӿں�Դ��ַ�ĳ�Ա���ݡ�
    //���ֻ֧�����ò�֧�ֻ�ȡ��
    //optLen = sizeof(ip_mreq_source);
    //iResult = getsockopt(s, level, IP_DROP_SOURCE_MEMBERSHIP, reinterpret_cast<char *>(&IpMreqSource), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    //��ȡ�� IP_IFLIST ѡ������ĵ�ǰ IFLIST�� ���δ���� IP_IFLIST ���򷵻ش���
    //���ֻ�ܻ�ȡ�������á�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_GET_IFLIST, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ����Ϊ TRUE ʱ��ָʾӦ�ó����ṩ IP ��ͷ�� ��������SOCK_RAW�׽��֡�
    ���Ӧ�ó����ṩ��ֵΪ�㣬TCP/IP �����ṩ������ܻ����� ID �ֶΡ�
    IP_HDRINCLѡ���������SOCK_RAW���͵�Э�顣 
    ֧��SOCK_RAW�� TCP/IP �����ṩ����Ӧ֧��IP_HDRINCL��
    �����֧�ֻ�ȡҲ֧�����á�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_HDRINCL, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ȡ������ �׽���IP_IFLIST ״̬�� �����ѡ������Ϊ true�������ݱ����ս����� IFLIST �еĽӿڡ� 
    �������κ������ӿ����յ������ݱ��� IFLIST ��ʼΪ�ա� ʹ�� IP_ADD_IFLIST �� IP_DEL_IFLIST �༭ IFLIST��
    �����֧�ֻ�ȡҲ֧�����á�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_IFLIST, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //��ȡϵͳ��·�� MTU �Ĺ���ֵ�� �׽��ֱ��������ӡ�
    //���ֻ�ܻ�ȡ���������á�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_MTU, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ȡ�������׽��ֵ�·�� MTU ����״̬�� Ĭ��ֵΪ IP_PMTUDISC_NOT_SET�� 
    �������׽��֣� IP_PMTUDISC_NOT_SET �� IP_PMTUDISC_DO ��ִ��·�� MTU ���֡� 
    IP_PMTUDISC_DONT �� IP_PMTUDISC_PROBE ���ر�·�� MTU ���֡� 
    �������ݱ��׽��֣� IP_PMTUDISC_DO ��ǿ�����д������ݰ����� DF λ�����ҳ��Է��ʹ���·�� MTU �����ݰ������´���
    IP_PMTUDISC_DONT ��ǿ�����д������ݰ�δ���� DF λ���������ݰ������ݽӿ� MTU ���зֶΡ� 
    IP_PMTUDISC_PROBE ��ǿ�����д������ݰ����� DF λ�����ҳ��Է��ʹ��ڽӿ� MTU �����ݰ������´���
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);//PMTUD_STATE
    iResult = getsockopt(s, level, IP_MTU_DISCOVER, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ȡ���������ڷ��� IPv4 �ಥ�����Ĵ����ӿڡ� ��ѡ���������ڽ��� IPv4 �ಥ������Ĭ�Ͻӿڡ� 
    ���ô�ѡ�������ֵ�ǰ������ֽ�˳�����е� 4 �ֽ� IPv4 ��ַ�� 
    �� DWORD ����Ҳ�����ǰ������ֽ�˳�����еĽӿ�������
    �� IPv4 ��ַ 0.0.) 0.0 ֮�⣬0.x.x �� (��һ����λ�ֽڣ�0.0.0.0 ���⣩���κ� IP ��ַ������Ϊ�ӿ������� 
    �ӿ�������һ�� 24 λ���֣� (�˷�Χ����) ����ʹ�� 0.0.0.0/8 IPv4 ��ַ�顣 
    �ӿ�����������ָ�� IPv4 �Ķಥ������Ĭ�Ͻӿڡ� ��� optval Ϊ�㣬��Ϊ���Ͷಥ����ָ�����ڽ��նಥ��Ĭ�Ͻӿڡ� 
    ��ȡ��ѡ��ʱ�� optval �����ص�ǰĬ�Ͻӿ����������ڰ������ֽ�˳���Ͷಥ IPv4 ������
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_MULTICAST_IF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �������ӵ�һ�������ಥ����׽��֣��˿����Ƿ����ͨ����ѡ�ಥ�ӿڷ��͵���Щ�ಥ��� ���� ���ݰ��ĸ�����
    Ĭ������£� IP_MULTICAST_LOOP ���� (ֵ 1/TRUE) ������׽��� �� ���յ�ǰ��������͵�ƥ��ಥ���ݰ��� 
    ͨ������ѡ������Ϊ 0/FALSE) �����ô�ѡ�� (��ζ�ţ���ʹ���׽����ڻ��ؽӿ��ϴ򿪣�Ҳ������մӱ��ؼ�������͵Ķಥ��

    ���� POSIX �汾�� IP_MULTICAST_LOOP ������ - �����ڽ����׽��������� ѡ��;�������ڷ����׽��������� POSIX ѡ�
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_MULTICAST_LOOP, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //����/��ȡ���׽����ϵ� IP �ಥ���������� TTL ֵ��
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_MULTICAST_TTL, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ָ��Ҫ���봫�����ݰ��� IP ѡ� ������ѡ�������ǰָ��������ѡ� 
    �� optval ����Ϊ���ɾ����ǰָ��������ѡ� 
    ����ҪIP_OPTIONS֧��;��Ҫ����Ƿ�֧��IP_OPTIONS����ʹ�� getockopt ��ȡ��ǰѡ�
    ��� getsockopt ʧ�ܣ���֧��IP_OPTIONS��
    */
    //��ȡ�����ö�֧�֡�
    //optLen = sizeof(int);
    //iResult = getsockopt(s, level, IP_OPTIONS, reinterpret_cast<char *>(&optVal), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    /*
    ָʾ LPFN_WSARECVMSG (WSARecvMsg) �����Ƿ�Ӧ���ؿ�ѡ�������ݣ����а���Ϊ���ݱ��׽��ֽ������ݰ��ĵ���ӿڡ�
    ��ѡ�������� WSAMSG �ṹ�з��ؽ������ݰ��� IPv4 �ӿڡ� 
    ��ѡ��������ݱ���ԭʼ�׽�������Ч�� (�׽������ͱ���SOCK_DGRAM��SOCK_RAW) ��
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_ORIGINAL_ARRIVAL_IF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //ָʾ WSARecvMsg ����Ӧ�������ݰ���Ϣ��
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_PKTINFO, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //��������ֹ�㲥���ա�
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_RECEIVE_BROADCAST, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ָʾ IP ��ջ�Ƿ�Ӧʹ���й��ĸ��ӿ�ʹ�����ݱ��׽��ֽ������ݰ�����ϸ��Ϣ�����ƻ�������
    �����ֵΪ true�� LPFN_WSARECVMSG (WSARecvMsg) ���������ؿ�ѡ�������ݣ����а����ӿڣ�����Ϊ���ݱ��׽��ֽ��������ݰ���
    ��ѡ�������� WSAMSG �ṹ�з��ؽ������ݰ��� IPv4 �ӿڡ� 
    ��ѡ��������ݱ���ԭʼ�׽�������Ч�� (�׽������ͱ���SOCK_DGRAM��SOCK_RAW) ��
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_RECVIF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ָʾ IP ��ջ�Ƿ�Ӧʹ�ð����ѽ������ݱ��ϵķ������� (TOS) IPv4 ��ͷ�ֶε���Ϣ�����ƻ�������
    �����ֵΪ true�� LPFN_WSARECVMSG (WSARecvMsg) ���������ؿ�ѡ�Ŀ������ݣ����а������յ����ݱ��� TOS IPv4 ��ͷ�ֶ�ֵ��
    ��ѡ�������� WSAMSG �ṹ�з��ؽ��յ����ݱ��� TOS IPv4 ��ͷ�ֶΡ� ��IP_TOS���ص���Ϣ���͡� 
    ������ TOS �ֶε����� DSCP �� ECN λ�� ��ѡ��������ݱ��׽�������Ч�� (�׽������ͱ���SOCK_DGRAM) ��
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_RECVTOS, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ָʾӦ�� LPFN_WSARECVMSG (WSARecvMsg ) �����з���Ծ�� (TTL) ��Ϣ�� 
    ����ڵ��� setsockopt ʱ�� optval ����Ϊ 1�������ø�ѡ�
    �������Ϊ 0������ø�ѡ�
    ��ѡ��������ݱ���ԭʼ�׽�����Ч�� (�׽������ͱ���SOCK_DGRAM��SOCK_RAW) ��
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_RECVTTL, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ����ʹ�á� �������� (TOS) ����Ӧ��ʹ�÷������� API �������á� 
    �й���ϸ��Ϣ�������ƽ̨ SDK �ķ������������е� ���� ����
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_TOS, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���� TCP/IP �����ṩ�����ڴ������ݱ��� IP ��ͷ�� TTL �ֶ������õ�Ĭ��ֵ�� 
    ����ҪIP_TTL֧��;��Ҫ����Ƿ�֧��IP_TTL����ʹ�� getockopt ��ȡ��ǰѡ� 
    ��� getsockopt ʧ�ܣ���֧��IP_TTL��
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_TTL, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //������Դ��Ϊ���ͷ����ӵ��ṩ�Ķಥ��ͽӿڡ�
    //ֻ�����ò��ܻ�ȡ��
    //optLen = sizeof(ip_mreq_source);
    //iResult = getsockopt(s, level, IP_UNBLOCK_SOURCE, reinterpret_cast<char *>(&IpMreqSource), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    /*
    ��ȡ���������ڷ��� IPv4 �����Ĵ����ӿڡ� ��ѡ���������ڽ��� IPv4 ������Ĭ�Ͻӿڡ� 
    ��ѡ����ڶ�����������ǳ���Ҫ�� ���ô�ѡ�������ֵ�ǰ������ֽ�˳�����е� 4 �ֽ� IPv4 ��ַ��
    �� DWORD ���������ǰ������ֽ�˳�����еĽӿ�������
    �� IPv4 ��ַ 0.0.0.0 ֮�⣬) 0.x.x �� (��һ����λ�ֽ� 0.x.x.x �е��κ� IP ��ַ������Ϊ�ӿ�������
    �ӿ�������һ�� 24 λ���֣� (�˷�Χ����) ����ʹ�� 0.0.0.0/8 IPv4 ��ַ�顣 
    �ӿ�����������ָ�����ڷ��� IPv4 ������Ĭ�Ͻӿڡ� GetAdaptersAddresses ���������ڻ�ȡ�ӿ�������Ϣ�� 
    ��� optval Ϊ�㣬�����ڷ���������Ĭ�Ͻӿ�����Ϊ��δָ������
    ��ȡ��ѡ��ʱ�� optval �����ص�ǰĬ�Ͻӿ����������ڰ������ֽ�˳���� IPv4 ������
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_UNICAST_IF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    	��ȡ�����ø����׽��ֵ� IP �� MTU (�����ޣ����ֽ�) Ϊ��λ����
        �����ֵ����ϵͳ��·�� MTU (�Ĺ���ֵ������ͨ����ѯ IP_MTU �׽���ѡ��) �����ӵ��׽����ϼ�������ѡ����Ч��
        �����ֵ�ϵͣ�����ڴ�ֵ�ĳ�վ���ݰ������ֶΣ������޷����ͣ�����ȡ���� IP_DONTFRAGMENT��ֵ��
        Ĭ��ֵΪ MAXULONG) (IP_UNSPECIFIED_USER_MTU�� 
        Ϊ�˱������Ͱ�ȫ��Ӧʹ�� WSAGetIPUserMtu �� WSASetIPUserMtu ������������ֱ��ʹ���׽���ѡ�
    */
    //��ȡ�����ö�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, level, IP_USER_MTU, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���ݱ��׽��ָ����������� (cmsg_type) ָʾ�û�ģʽ Windows ɸѡƽ̨ (WFP) �ض������ʹ�õ� UDP �׽��ֵ��ض��������ġ�
    */
    //��ȡ�����ö�֧�֡�
    //���п������ݵ� WSACMSGHDR
    //optLen = sizeof(int);
    //iResult = getsockopt(s, level, IP_WFP_REDIRECT_CONTEXT, reinterpret_cast<char *>(&optVal), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    /*
    ���ݱ��׽��ָ����������� (cmsg_type) ָʾ�û�ģʽ Windows ɸѡƽ̨ (WFP) �ض������ʹ�õ� UDP �׽��ֵ��ض����¼��
    */
    //��ȡ�����ö�֧�֡�
    //���п������ݵ� WSACMSGHDR
    //optLen = sizeof(int);
    //iResult = getsockopt(s, level, IP_WFP_REDIRECT_RECORDS, reinterpret_cast<char *>(&optVal), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

    return ret;
}


int get_ipv4_sock_opt()
/*


https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-ip-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    WinsockInitializer winsock(MAKEWORD(2, 2));
    _ASSERTE(winsock.IsInitialized());

    SOCKET tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(tcp != INVALID_SOCKET);

    SOCKET udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    _ASSERTE(udp != INVALID_SOCKET);

    SOCKET ipv4 = socket(AF_INET, SOCK_RAW, IPPROTO_IPV4);
    _ASSERTE(udp != INVALID_SOCKET);

    //�����Ը������󶨵ġ�

    get_ipv4_sock_opt(tcp, SOL_SOCKET); printf("\r\n\r\n\r\n");
    get_ipv4_sock_opt(udp, SOL_SOCKET); printf("\r\n\r\n\r\n");
    get_ipv4_sock_opt(ipv4, SOL_SOCKET); printf("\r\n\r\n\r\n");

    get_ipv4_sock_opt(tcp, IPPROTO_IP); printf("\r\n\r\n\r\n");
    get_ipv4_sock_opt(udp, IPPROTO_IP); printf("\r\n\r\n\r\n");
    get_ipv4_sock_opt(ipv4, IPPROTO_IP); printf("\r\n\r\n\r\n");

    //������ϲ������ܷ���һЩ��ȷ�ɹ��ĵ��á�

    closesocket(tcp);
    closesocket(udp);
    closesocket(ipv4);

    // Winsock cleanup is handled automatically by WinsockInitializer destructor

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
�±�������IPPROTO_RM�׽���ѡ���Щѡ��������Ϊ IPv4 ��ַϵ�� (AF_INET) �������׽��֣�
��Щ�׽��־���ָ��Ϊ�ɿ��ಥ (IPPROTO_RM) ���׽��ֺ�����Э�������

https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-rm-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    return ret;
}


int get_tcp_sock_opt(_In_ SOCKET s)
{
    int ret = ERROR_SUCCESS;

    /*
    ��� Ϊ TRUE��������ṩ��ʵ�� Berkeley Software Distribution (BSD) ��ʽ�� (���ڴ����������ݵ�Ĭ��) ��
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

    WinsockInitializer winsock(MAKEWORD(2, 2));
    _ASSERTE(winsock.IsInitialized());

    SOCKET s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //�����Ը������󶨵ġ�

    get_tcp_sock_opt(s4);
    get_tcp_sock_opt(s6);

    closesocket(s4);

    // Winsock cleanup is handled automatically by WinsockInitializer destructor

    return ret;
}


int get_udp_sock_opt(_In_ SOCKET s)
{
    int ret = ERROR_SUCCESS;

    /*
    ��� Ϊ TRUE���� UDP ���ݱ�����У���һ���͡�
    */
    int optVal{};
    int optLen = sizeof(int);
    int iResult = getsockopt(s, SOL_SOCKET, UDP_CHECKSUM_COVERAGE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��� Ϊ TRUE������ UDP ���ݱ���У���Ϊ�㡣 ���ڷ����ṩ���Ǳ���ġ� 
    ��������ṩ��û�н��� UDP У��ͼ���Ļ��ƣ�������ֻ�洢��ѡ�����ִ���κβ����� IPv6 ��֧�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, UDP_NOCHECKSUM, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ����Ϊ����ֵʱ��������յ����ݱ����ܻ�����Ӧ�ó���ָʾ֮ǰ�ϲ��ɵ�����Ϣ�������� 
    ѡ��ֵ��ʾ������Ӧ�ó���ָʾ�ĺϲ���Ϣ�������Ϣ��С�����ֽ�Ϊ��λ���� 
    �����Ի�ָʾ����ѡ��ֵ��δ�ϲ���Ϣ�� Ĭ��ֵΪ 0 (�޺ϲ�) �� 
    �������ݱ�Դ��ͬһԴ��ַ�Ͷ˿�ʱ���Ż�ϲ����ݱ��� 
    �ϲ����������ݱ��Ĵ�С����ͬ�������һ�����ݱ����ܸ�С�� 
    ���Ӧ�ó�����Ҫ���������һ�����ݱ������ (���ݱ���С��) ����������ͬ����
    �����ʹ��֧�ֿ�����Ϣ (�Ľ��� API������ LPFN_WSARECVMSG (WSARecvMsg) ) �� 
    �����һ����Ϣ֮���������Ϣ�Ĵ�С�������� UDP_COALESCED_INFO �ؼ���Ϣ���ҵ�������Ϣ������Ϊ DWORD�� 
    Ϊ�������ͣ�Ӧ�ó���Ӧֱ��ʹ�� WSAGetUdpRecvMaxCoalescedSize �� WSASetUdpRecvMaxCoalescedSize ������
    ������ֱ��ʹ���׽���ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, UDP_RECV_MAX_COALESCED_SIZE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ������Ϊ����ֵʱ��Ӧ�ó����͵Ļ��������������ջϸ��Ϊ�����Ϣ�� ѡ��ֵ��ʾÿ���ֽ���Ϣ�Ĵ�С�� 
    ѡ��ֵ���ֽڱ�ʾ�� ���һ���εĴ�С����С��ѡ���ֵ�� Ĭ��ֵΪ 0 (�޷ֶ�) �� 
    Ӧ�ó���Ӧ����һ��С��Ŀ��·���� MTU ��ֵ�� () ���Ա��� IP ��Ƭ�� 
    Ϊ�������ͣ�Ӧ�ó���Ӧʹ�� WSAGetUdpSendMessageSize �� WSASetUdpSendMessageSize ������������ֱ��ʹ���׽���ѡ�
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
�±������� IPPROTO_UDP �׽���ѡ���Щѡ��������Ϊ IPv4 �� IPv6 ��ַϵ�д������׽��֣�
(AF_INET���� Э�� ����AF_INET6) ָ��Ϊ UDP (IPPROTO_UDP) �� �׽��� ������

https://learn.microsoft.com/zh-cn/windows/win32/winsock/ipproto-udp-socket-options
*/
{
    int ret = ERROR_SUCCESS;

    WinsockInitializer winsock(MAKEWORD(2, 2));
    _ASSERTE(winsock.IsInitialized());

    SOCKET s4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //�����Ը������󶨵ġ�

    get_udp_sock_opt(s4);
    get_udp_sock_opt(s6);

    closesocket(s4);

    // Winsock cleanup is handled automatically by WinsockInitializer destructor

    return ret;
}


int get_ipx_sock_opt()
/*

�� level ��������Ϊ NSPROTO_IPX ʱ��optname ������ȡֵ������IPX_PTYPE�ȡ�

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
    �׽�������������
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
    �׽�������Ϊ���ڴ���ͽ��չ㲥��Ϣ��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        printf("SO_BROADCAST: %ld\n", optVal);
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �����׽���ʹ�õı��ص�ַ�����ض˿ڡ�Զ�̵�ַ��Զ�̶˿ڡ��׽������ͺ�Э�顣
    */
    CSADDR_INFO AddrInfo{};
    optLen = sizeof(CSADDR_INFO);
    iResult = getsockopt(s, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char *>(&AddrInfo), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ����һ�ε��� setsockopt ��ϵͳĬ��ֵ���ص�ǰ�׽���״̬��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �����������׽��ֵ������� ���׽���ѡ����������������ӵ�Э�顣
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONNECT_TIME, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
         
    } else {
        DisplayError(WSAGetLastError());
    }

    //�����õ��ԡ�
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DEBUG, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //��� Ϊ TRUE�������SO_LINGERѡ�
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DONTLINGER, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ·�ɹ��ܱ����á� 
    �����óɹ�������AF_INET�׽����Ͻ�������;ʹ�� WSAENOPROTOOPT AF_INET6�׽���ʧ�ܡ� 
    ATM �׽��ֲ�֧�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DONTROUTE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //��������״̬�������
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ֹ�κ������׽��ְ󶨵�ͬһ��ַ�Ͷ˿ڡ� �ڵ��� �� ����֮ǰ���������ô�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //������
    GROUP Group{};
    optLen = sizeof(GROUP);
    iResult = getsockopt(s, SOL_SOCKET, SO_GROUP_ID, reinterpret_cast<char *>(&Group), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //������
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_GROUP_PRIORITY, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //���ڷ��͡��������ӡ��� ATM �׽��ֲ�֧�֡�
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //���ص�ǰ�Ķ���ѡ�
    LINGER Linger{};
    optLen = sizeof(LINGER);
    iResult = getsockopt(s, SOL_SOCKET, SO_LINGER, reinterpret_cast<char *>(&Linger), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ������Ϣ���׽������͵���Ϣ������С (���磬SOCK_DGRAM) �� �������������׽���û�����塣
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���������������н��� OOB ���ݡ� (�йر���������ۣ������ Windows �׽��� 1.1 ��ֹ���̺� EINPROGRESS ���֡�)
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_OOBINLINE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ͨ��Ϊ���ؼ�����ϵĲ�ͬ���ص�ַ�˿ڶԶ�η���ͨ����˿ڣ�������󻯶˿ڷ��䣬�Ӷ�Ϊ�׽������ñ��ض˿ڿ������ԡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_PORT_SCALABILITY, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //�󶨵����׽��ֵ�Э���Э����Ϣ��˵����
    WSAPROTOCOL_INFO ProtocalInfo{};
    optLen = sizeof(WSAPROTOCOL_INFO);
    iResult = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, reinterpret_cast<char *>(&ProtocalInfo), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    Ϊ���ձ�����ÿ���׽��ֵ��ܻ������ռ䡣 ����SO_MAX_MSG_SIZE�޹أ�Ҳ��һ����Ӧ�� TCP ���մ��ڵĴ�С��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //���Խ��׽��ְ󶨵�����ʹ���еĵ�ַ�� �������� ATM ������
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    Ϊ���ͱ�����ÿ���׽��ֻ������ܿռ䡣 ����SO_MAX_MSG_SIZE�޹أ�Ҳ��һ����Ӧ�� TCP ���ʹ��ڵĴ�С��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    //�׽��ֵ����� (���磬SOCK_STREAM) ��
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_TYPE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �� �׽��ֹ����ķ����ṩ�����еĲ�͸�����ݽṹ���� �˶���洢�����ṩ����ĵ�ǰ������Ϣ�� 
    �����ݽṹ��ȷ�и�ʽ�ض��ڷ����ṩ����
    */
    //optLen = sizeof(int);
    //iResult = getsockopt(s, SOL_SOCKET, PVD_CONFIG, reinterpret_cast<char *>(&optVal), &optLen);
    //if (iResult != SOCKET_ERROR) {

    //} else {
    //    DisplayError(WSAGetLastError());
    //}

     /*
    �Ӵ��׽��ַ�������ʱ����ʹ�ñ��ػ��ص�ַ�� �������͵���������Ҳ���ڱ��ؽ���ʱ����Ӧʹ�ô�ѡ�
    Windows TCP/IP �ṩ����֧�ִ�ѡ�
    ����� Windows Vista �����߰汾��ʹ�ô�ѡ� �� getsockopt �� setsockopt ������ʧ�ܲ���ʾ WSAEINVAL��
    �����ڰ汾�� Windows �ϣ���Щ����ʧ�ܲ���ʾ WSAENOPROTOOPT��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_USELOOPBACK, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���� BSD UNIX �е��׽���ѡ����������ݡ�
    ��ѡ������Ҫ�������׽��������������С�ֽ����� Windows TCP/IP �ṩ����֧�ִ�ѡ�
    ����� Windows Vista �����߰汾��ʹ�ô�ѡ� �� getsockopt �� setsockopt ������ʧ�ܲ����� WSAEINVAL��
    �����ڰ汾�� Windows �ϣ���Щ����ʧ�ܲ����� WSAENOPROTOOPT��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_SNDLOWAT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���� BSD UNIX �е��׽���ѡ����������ݡ� ��ѡ�������׽����������Ҫ��������С�ֽ�����
    Windows TCP/IP �ṩ����֧�ִ�ѡ�
    ����� Windows Vista �����߰汾��ʹ�ô�ѡ� �� getsockopt �� setsockopt ������ʧ�ܲ����� WSAEINVAL��
    �����ڰ汾�� Windows �ϣ���Щ����ʧ�ܲ����� WSAENOPROTOOPT��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RCVLOWAT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ֹ���͵��õĳ�ʱ���Ժ���Ϊ��λ����
    ��ѡ���Ĭ��ֵΪ�㣬��ʾ���Ͳ������ᳬʱ�������ֹ���͵��ó�ʱ�������Ӵ��ڲ�ȷ��״̬��Ӧ�رա�
    ���ʹ�� WSASocket ���������׽��֣��� dwFlags ������������ WSA_FLAG_OVERLAPPED ���Բ���ʹ��ʱ����������
    ���򣬳�ʱ��Զ������Ч��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ֹ���յ��õĳ�ʱ���Ժ���Ϊ��λ����
    ��ѡ���Ĭ��ֵΪ�㣬��ʾ���ղ������ᳬʱ�������ֹ���յ��ó�ʱ�������Ӵ��ڲ�ȷ��״̬��Ӧ�رա�
    ���ʹ�� WSASocket ���������׽��֣��� dwFlags ������������ WSA_FLAG_OVERLAPPED ���Բ���ʹ��ʱ����������
    ���򣬳�ʱ��Զ������Ч��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    //���ظ����׽��� ��WSAPROTOCOL_INFOA �ṹ
    WSAPROTOCOL_INFOA ProtocalInfoA{};
    optLen = sizeof(WSAPROTOCOL_INFOA);
    iResult = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFOA, reinterpret_cast<char *>(&ProtocalInfoA), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    //���ظ����׽��� ��WSAPROTOCOL_INFOW �ṹ
    WSAPROTOCOL_INFOW ProtocalInfoW{};
    optLen = sizeof(WSAPROTOCOL_INFOW);
    iResult = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFOW, reinterpret_cast<char *>(&ProtocalInfoW), &optLen);
    if (iResult != SOCKET_ERROR) {
        
    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �������ݣ��������������������У�����������һ�����Խ������ӡ� �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� 
    Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    //SO_CONNDATA


    /*
    �������ݵĳ��ȣ����ֽ�Ϊ��λ�������������������������У�����������һ�����Խ������ӡ�
    �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONNDATALEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��������ѡ�����ݣ��������������������У�����������һ�����Խ������ӡ� 
    �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    //SO_CONNOPT


    /*
    ����ѡ�����ݵĳ��ȣ����ֽ�Ϊ��λ���������������������������У�����������������һ�����Խ������ӡ� 
    �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_CONNOPTLEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �������ݣ��������������������У�����������һ���ͣ��ԶϿ����ӡ�
    �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    //SO_DISCDATA


    /*
    	�������ݵĳ��ȣ����ֽ�Ϊ��λ�����ó��Ȳ��������������������У�����������һ���ͣ��ԶϿ����ӡ� 
        �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DISCDATALEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    �����Ͽ�����ѡ�����ݣ������������������������У�����������һ���ͣ��ԶϿ����ӡ� 
    �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    //SO_DISCOPT

    /*
    ���ӶϿ�����ѡ�����ݣ������������������������У��ĳ��ȣ����ֽ�Ϊ��λ��������������������һ�����ԶϿ����ӡ�
    �ɰ�Э�飨�� DECNet��OSI TP4 �ȣ�ʹ�ô�ѡ� Windows �е� TCP/IP Э�鲻֧�ִ�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_DISCOPTLEN, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ����Э��֧�ֵĳ�վ���ݱ�������С�����ֽ�Ϊ��λ���� ���׽���ѡ��������������׽���û�����塣
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_MAXDG, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ����Э��֧�ֵ�����Ŀ���ַ�ĳ�վ���ݱ�������С�����ֽ�Ϊ��λ���� ���׽���ѡ��������������׽���û�����塣
    Microsoft �ṩ�̿��ܻ�������ʾ��ʽ������ΪSO_MAXDG��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_MAXPATHDG, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���ú󣬽�Ӱ�촴���ĺ����׽����Ƿ��ص��� ��ѡ��Ŀ���ֵΪSO_SYNCHRONOUS_ALERT��SO_SYNCHRONOUS_NONALERT�� 
    ��Ӧʹ�ô�ѡ� ����� WSASocket �������ر� dwFlags �����е�WSA_FLAG_OVERLAPPEDλ��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_OPENTYPE, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ʹ�ô�ѡ�������׽��֡� ���� ѡ����׽��ֽ�ʹ�� RST ��Ӧ���д������ӣ������ǽ������ǡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_PAUSE_ACCEPT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    Ӧ��δ�󶨵��׽��������ô�ѡ� ����SO_RANDOMIZE_PORT�����׽�����ѡ����ʱ�˿�ʱ����������˿ںš�
    ʹ�� SO_REUSE_UNICASTPORT) ѡ��Ķ˿� (�Զ����ö˿�Ҳ����������صĶ˿ڣ���ˣ����Ӧ�ó���SO_REUSE_UNICASTPORTȻ��������SO_RANDOMIZE_PORT����ڶ� �� setockopt ���ý�ʧ�ܡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_RANDOMIZE_PORT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���ú����� Winsock API ���Ӻ����ظ�ʹ����Ҫ��ʽ�󶨵���ʱ�˿ڣ����� ConnectEx�� 
    ��ע�⣬������ʽ�� (�����Ӻ��������磬��û����ʽ�󶨵�����½�������) Ĭ�������˴�ѡ� 
    ʹ�ô�ѡ������������߶����õ�ƽ̨�� SO_PORT_SCALABILITY ��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_REUSE_UNICASTPORT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ���׽��������ô�ѡ��ʱ����ʾ��Զ����ʹ�ø��׽��ֽ��յ������ݰ�����ˣ���˿ڿ������������ಥӦ�ó�������
    ��ֵ����Ϊ 1 ��ʼ���ڶ˿��Ϲ����ಥ������ ��ֵ����Ϊ 0 (Ĭ��) ����ô���Ϊ��
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_REUSE_MULTICASTPORT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ѡ���� AcceptEx ����һ��ʹ�á� ��ѡ����´������׽��ּ̳е��׽��ֵ����ԡ� 
    ���Ҫ�ڽ��ܵ��׽�����ʹ�� getpeername�� getockname�� getockopt �� setsockopt ��������Ӧ���ô�ѡ�
    */
    optLen = sizeof(int);
    iResult = getsockopt(s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char *>(&optVal), &optLen);
    if (iResult != SOCKET_ERROR) {

    } else {
        DisplayError(WSAGetLastError());
    }

    /*
    ��ѡ���� ConnectEx�� WSAConnectByList �� WSAConnectByName ����һ��ʹ�á� �������Ӻ󣬴�ѡ������׽��ֵ����ԡ�
    ���Ҫ�����ӵ��׽�����ʹ�� getpeername�� getockname�� getockopt�� setsockopt �� shutdown ��������Ӧ���ô�ѡ�
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

    WinsockInitializer winsock(MAKEWORD(2, 2));
    _ASSERTE(winsock.IsInitialized());

    SOCKET s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s4 != INVALID_SOCKET);

    SOCKET s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    _ASSERTE(s6 != INVALID_SOCKET);

    //�����Ը������󶨵ġ�

    get_sol_sock_opt(s4);
    get_sol_sock_opt(s6);

    closesocket(s4);

    // Winsock cleanup is handled automatically by WinsockInitializer destructor

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