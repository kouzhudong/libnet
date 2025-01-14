/*
Windows api ��ȡ �׽��� MTU IP_MTU
ChatGPT ˵��
ChatGPT
�� Windows �У�ʹ�� Windows API ��ȡ�׽��ֵ� MTU������䵥Ԫ��ͨ���漰��ͨ�� IOCTL
���ƴ�������ȡ������ӿ���ص����ԡ�������Windows �׽��� API ����û��ֱ���ṩ��ȡ MTU �ĺ�����Ҫ����ض��ӿڵ�
MTU��ͨ����Ҫʹ������ӿ���صĺ������� GetAdaptersInfo �� GetIfTable���Լ�����һЩ IOCTL ���á�

1. ��ȡ�ӿ� MTU��ͨ�� IOCTL��
IP_MTU ��ָһ���ӿڣ����������������ܹ��������� IP ���ݰ���С���� Windows �У�����ʹ�� SIO_GET_INTERFACE_LIST ��
SIO_GET_INTERFACE_INFO �� IOCTL ����������ȡ�ӿ���Ϣ����������ȡ MTU��

��������ȡ�׽��ֵ� MTU ��ֱ�ӵ������ǻ�ȡ������������ MTU���������ͨ����ͨ��������ӿ���ص� API ��ɵġ�

2. ʹ�� GetAdaptersInfo ��ȡ�ӿ� MTU
GetAdaptersInfo ��һ�� Windows API�����԰������ȡ�й�ϵͳ����������������Ϣ�����оͰ��� MTU ֵ��

���������ͨ�� GetAdaptersInfo ����ȡ MTU ֵ��ʾ����

cpp
���ƴ���
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

void GetMTU()
{
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IP_ADAPTER_INFO *pAdapterInfo = NULL;
    IP_ADAPTER_INFO *pAdapter = NULL;

    // First, make an initial call to GetAdaptersInfo to get the necessary size
    dwRetVal = GetAdaptersInfo(pAdapterInfo, &dwSize);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(dwSize);
        if (pAdapterInfo == NULL)
        {
            printf("Error allocating memory needed to call GetAdaptersInfo\n");
            return;
        }
    }

    // Get the adapter information
    dwRetVal = GetAdaptersInfo(pAdapterInfo, &dwSize);
    if (dwRetVal != NO_ERROR)
    {
        printf("GetAdaptersInfo failed with error: %u\n", dwRetVal);
        if (pAdapterInfo) free(pAdapterInfo);
        return;
    }

    // Iterate through the list of adapters
    pAdapter = pAdapterInfo;
    while (pAdapter)
    {
        printf("Adapter Name: %s\n", pAdapter->AdapterName);
        printf("MTU: %lu\n", pAdapter->Mtu);  // Get the MTU value
        pAdapter = pAdapter->Next;
    }

    if (pAdapterInfo) free(pAdapterInfo);
}

int main()
{
    GetMTU();
    return 0;
}
3. ʹ�� GetIfTable ��ȡ�ӿ� MTU
��һ�ַ�����ʹ�� GetIfTable ��ȡ�ӿ���Ϣ���ú�������һ���ӿڱ����Դ��л�ȡ MTU ��Ϣ��

������ʹ�� GetIfTable ��ȡ MTU ��ʾ����

cpp
���ƴ���
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

void GetMTU()
{
    MIB_IFTABLE* pIfTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Get the necessary size for the table
    dwRetVal = GetIfTable(NULL, &dwSize, FALSE);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER)
    {
        pIfTable = (MIB_IFTABLE*)malloc(dwSize);
        if (pIfTable == NULL)
        {
            printf("Error allocating memory needed to call GetIfTable\n");
            return;
        }
    }

    // Get the table of interfaces
    dwRetVal = GetIfTable(pIfTable, &dwSize, FALSE);
    if (dwRetVal != NO_ERROR)
    {
        printf("GetIfTable failed with error: %u\n", dwRetVal);
        if (pIfTable) free(pIfTable);
        return;
    }

    // Iterate through the table of interfaces
    for (DWORD i = 0; i < pIfTable->dwNumEntries; i++)
    {
        printf("Interface Index: %lu\n", pIfTable->table[i].dwIndex);
        printf("MTU: %lu\n", pIfTable->table[i].dwMtu);  // MTU value for this interface
    }

    if (pIfTable) free(pIfTable);
}

int main()
{
    GetMTU();
    return 0;
}
4. ͨ���׽��ֻ�ȡ MTU��IOCTL��
�������ͨ���׽��֣����磬��ȡ���ض��׽�����ص� MTU�������в���������ʹ�� WSAIoctl ���������� IOCTL
���󣬲�ѯ����ӿڵ� MTU��

cpp
���ƴ���
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define IOCTL_QUERY_IF_MTU 0x8004661E  // Example IOCTL value (check actual value for your case)

void GetSocketMTU(SOCKET s) //����ʧ�ܡ�
{
    ULONG mtu = 0;
    DWORD bytesReturned = 0;
    int result = WSAIoctl(s, IOCTL_QUERY_IF_MTU, NULL, 0, &mtu, sizeof(mtu), &bytesReturned, NULL, NULL);
    if (result == SOCKET_ERROR)
    {
        printf("WSAIoctl failed with error: %d\n", WSAGetLastError());
    }
    else
    {
        printf("MTU: %lu\n", mtu);
    }
}

int main()
{
    WSADATA wsaData;
    SOCKET s;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    GetSocketMTU(s);

    closesocket(s);
    WSACleanup();
    return 0;
}
�ܽ�
ʹ�� GetAdaptersInfo �� GetIfTable ��ȡ����ӿڵ� MTU��ͨ���ǻ�ȡ�ӿ� MTU �ĳ��÷�ʽ��
ʹ�� WSAIoctl ���� IOCTL ���󣬿���ͨ���׽��ֻ�ȡ MTU��
�������Ҫ���ͼ��Ľӿڷ��ʣ�������Ҫʹ�ø����ӵ� IOCTL ���û�ֱ��ͨ��ԭʼ�׽��������в�����
*/

/*
Windows api ��ȡ �׽��� MTU IP_MTU


Windows API ��ȡ�׽��� MTU (IP_MTU) �ķ���
����
�� Windows ƽ̨�ϣ����ǿ���ͨ�� Winsock �ṩ�� API ����ȡ�׽��ֵ�����䵥Ԫ (MTU)��MTU
��ָ����·��������ͨ����������ݰ����ȣ��˽� MTU �Ĵ�С�����Ż����紫�䡢�����Ƭ������ǳ���Ҫ��

��ȡ MTU �Ĳ���
�����׽��֣�
C++

SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == INVALID_SOCKET) {
    // �������
}
���ӵ�Ŀ��������
C++

sockaddr_in addr;
// ... ��ʼ�� addr �ṹ ...
if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
    // �������
}
��ȡ�׽���ѡ�
C++

DWORD mtu;
int optlen = sizeof(mtu);
if (getsockopt(sock, IPPROTO_IP, IP_MTU, (char*)&mtu, &optlen) == SOCKET_ERROR) {//���Գɹ���
    // �������
}
����ʾ��
C++

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("socket failed.\n");
        WSACleanup();
        return 1;
    }

    // ... ���ӵ�Ŀ������ ...

    DWORD mtu;
    int optlen = sizeof(mtu);
    if (getsockopt(sock, IPPROTO_IP, IP_MTU, (char*)&mtu, &optlen) == SOCKET_ERROR) {
        printf("getsockopt failed.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("MTU: %lu\n", mtu);

    closesocket(sock);
    WSACleanup();
    return 0;
}
ע������
IPPROTO_IP�� ָ��Э��Ϊ IP Э�顣
IP_MTU�� ��ȡ�׽��ֵ� MTU��
optlen�� ���ڽ��շ������ݵĻ�������С��
������ ��ض�ÿ���������ý��д�����
�첽������ ���ڸ�����Ӧ�ã����Կ���ʹ���첽��������ȡ MTU��
IPv6�� ���� IPv6 �׽��֣�����ʹ�����Ƶķ�������������Ҫ���ò�ͬ��Э�鼶���ѡ�
��������
ICMP ping�� ͨ�����Ͳ�ͬ��С�� ICMP echo ����������ݷ��صĴ�����Ϣ���ƶ� MTU��
Path MTU Discovery (PMTUD)�� ����ϵͳ���Զ����Է���·���ϵ� MTU���������ݰ�������ʱ������Ƭ��С��
�ܽ�
ͨ���������������ǿ��Է���ػ�ȡ Windows ƽ̨���׽��ֵ�
MTU���Ӷ�Ϊ����Ӧ�ó�����Ż��ṩ�ο�����ʵ��Ӧ���У����ݾ�������ѡ����ʵķ�����

��ܰ��ʾ��

MTU ��ֵ���ܻᶯ̬�仯���������Ҫ�߾��ȵ�����£�������ÿ�����ӽ���ʱ�����»�ȡ��
����������̲���Ϥ�������������ĵ���Ѱ��רҵ��ʿ�İ�����
ϣ������ش��������������

����������������⣬��ӭ��ʱ�����

�ؼ��ʣ� Windows API, �׽���, MTU, IP_MTU, Winsock, getsockopt
*/


/*
WSAIoctl MTU
ChatGPT ˵��
ChatGPT
�� Windows �У�ʹ�� WSAIoctl ����ȡ�������׽��ֵ� MTU ��Ҫ������ӿ���ص� IOCTL
���ƴ��롣������˵����������ȡ������ӿ���ص� MTU�������ʹ�� WSAIoctl �������������ʵ��� IOCTL �����롣

���ǣ���Ҫע����ǣ�WSAIoctl ����û��ֱ���ṩһ���򵥵Ŀ���������ѯ MTU�������ͨ��һЩ��ӷ��������
MTU�������ѯ����ӿڵ�������Ϣ��������ֱ��ͨ���׽�������ȡ��

WSAIoctl ����
WSAIoctl �� Windows �׽��� API �е�һ����������������׽���ִ��һЩ���Ʋ���������ԭ�����£�

cpp
���ƴ���
int WSAIoctl(
    SOCKET s,                   // �׽��־��
    DWORD dwIoControlCode,      // ������
    LPVOID lpvInBuffer,         // ���뻺����
    DWORD cbInBuffer,           // ���뻺������С
    LPVOID lpvOutBuffer,        // ���������
    DWORD cbOutBuffer,          // �����������С
    LPDWORD lpcbBytesReturned,  // ���ص��ֽ���
    LPOVERLAPPED lpOverlapped,  // �첽����
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // �첽��ɻص�
);
��ȡ MTU �ķ���
WSAIoctl ��������ͨ��һЩ IOCTL ���ƴ���������ӿڽ����������ڻ�ȡ MTU������ͨ����ͨ�� IOCTL
���ƴ�������ѯ����ӿڵ����ԣ��� SIO_GET_INTERFACE_LIST��SIO_GET_INTERFACE_INFO �ȡ�

����û��ר�ŵ� WSAIoctl ������ֱ�Ӳ�ѯ MTU���������ͨ�� SIO_GET_INTERFACE_LIST ������ IOCTL
�������ȡ����ӿڵ���Ϣ����������ȡ MTU��

ʾ����ͨ�� WSAIoctl ��ȡ MTU����ӷ�����
����ʾ������ͨ�� WSAIoctl ���û�ȡ����ӿڵ� MTU��

cpp
���ƴ���
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define IOCTL_QUERY_IF_MTU 0x8004661E  // ���ڲ�ѯ�ӿ� MTU �� IOCTL �����루����ֵȡ���ڽӿ����ͣ�

void GetMTU(SOCKET s)
{
    ULONG mtu = 0;
    DWORD bytesReturned = 0;
    int result = WSAIoctl(s, IOCTL_QUERY_IF_MTU, NULL, 0, &mtu, sizeof(mtu), &bytesReturned, NULL, NULL);

    if (result == SOCKET_ERROR)
    {
        printf("WSAIoctl failed with error: %d\n", WSAGetLastError());
    }
    else
    {
        printf("MTU: %lu\n", mtu);
    }
}

int main()
{
    WSADATA wsaData;
    SOCKET s;

    // ��ʼ�� Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // ����һ���׽��֣�����ʹ���κκ��ʵ��׽������ͣ�
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // ��ȡ MTU
    GetMTU(s);

    // �ر��׽��ֲ�����
    closesocket(s);
    WSACleanup();

    return 0;
}
��Ҫ˵��
IOCTL_QUERY_IF_MTU ��������һ�����ӣ�����Ŀ�����Ӧ�ø������ʵ��������ѡ�񡣲�ͬ������ӿڿ��ܻ�ʹ�ò�ͬ�� IOCTL
����������ȡ MTU�� SIO_GET_INTERFACE_LIST �� SIO_GET_INTERFACE_INFO
�����ڻ�ȡ����ӿ���Ϣ�Ŀ����롣�����ͨ����Щ�������ӻ�ȡ MTU ��Ϣ��������Ҫ�����ӿڱ�����ȡ MTU�� ͨ��
SIO_GET_INTERFACE_INFO ��ȡ MTU���Ƽ������� ��ȡ MTU ����һ�ַ�����ͨ�� SIO_GET_INTERFACE_INFO
�����롣�����ʹ�ô˿�����������ӿڵ���ϸ��Ϣ����������ȡ MTU��������ʾ�����룺

cpp
���ƴ���
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

void GetMTU()
{
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IP_INTERFACE_INFO *pIfInfo = NULL;

    // ��ȡ����Ļ�������С
    dwRetVal = WSAIoctl(INVALID_SOCKET, SIO_GET_INTERFACE_INFO, NULL, 0, NULL, 0, &dwSize, NULL, NULL);
    if (dwRetVal == SOCKET_ERROR)
    {
        printf("WSAIoctl failed with error: %d\n", WSAGetLastError());
        return;
    }

    // ���仺����
    pIfInfo = (IP_INTERFACE_INFO *)malloc(dwSize);
    if (pIfInfo == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    // ��ȡ�ӿ���Ϣ
    dwRetVal = WSAIoctl(INVALID_SOCKET, SIO_GET_INTERFACE_INFO, NULL, 0, pIfInfo, dwSize, &dwSize, NULL, NULL);
    if (dwRetVal == SOCKET_ERROR)
    {
        printf("WSAIoctl failed with error: %d\n", WSAGetLastError());
        free(pIfInfo);
        return;
    }

    // ��ӡ�ӿ���Ϣ������ MTU
    for (int i = 0; i < pIfInfo->NumAdapters; i++)
    {
        printf("Adapter index: %d\n", pIfInfo->Adapter[i].Index);
        printf("MTU: %lu\n", pIfInfo->Adapter[i].Mtu);  // MTU ��Ϣ
    }

    free(pIfInfo);
}

int main()
{
    WSADATA wsaData;

    // ��ʼ�� Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // ��ȡ����ӡ MTU
    GetMTU();

    WSACleanup();

    return 0;
}
�ܽ�
WSAIoctl ���������׽��ּ���� IOCTL ��������û��ר�ŵĿ���������ֱ�ӻ�ȡ MTU��
ͨ��ʹ���ʵ��Ŀ����루�� SIO_GET_INTERFACE_LIST �� SIO_GET_INTERFACE_INFO�������Լ�ӻ�ȡ������ӿ���ص� MTU
��Ϣ�� ʹ�� WSAIoctl ����ѯ MTU �ľ��巽�������������ڴ��������ӿ����ͺͻ����� �����ֻ����Ҫ��ȡ�ӿڵ�
MTU��ͨ��ʹ�� GetAdaptersInfo �� GetIfTable ��Щ���߲�� API �Ǹ��򵥺ͳ�����ѡ��
*/


#pragma once

#include "pch.h"
