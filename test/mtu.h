/*
Windows api 获取 套接字 MTU IP_MTU
ChatGPT 说：
ChatGPT
在 Windows 中，使用 Windows API 获取套接字的 MTU（最大传输单元）通常涉及到通过 IOCTL
控制代码来获取与网络接口相关的特性。不过，Windows 套接字 API 本身并没有直接提供获取 MTU 的函数。要获得特定接口的
MTU，通常需要使用网络接口相关的函数，如 GetAdaptersInfo 或 GetIfTable，以及其他一些 IOCTL 调用。

1. 获取接口 MTU（通过 IOCTL）
IP_MTU 是指一个接口（如网络适配器）能够处理的最大 IP 数据包大小。在 Windows 中，可以使用 SIO_GET_INTERFACE_LIST 和
SIO_GET_INTERFACE_INFO 等 IOCTL 控制码来获取接口信息，并从中提取 MTU。

不过，获取套接字的 MTU 更直接的做法是获取网络适配器的 MTU。这个操作通常是通过与网络接口相关的 API 完成的。

2. 使用 GetAdaptersInfo 获取接口 MTU
GetAdaptersInfo 是一个 Windows API，可以帮助你获取有关系统上网络适配器的信息，其中就包括 MTU 值。

下面是如何通过 GetAdaptersInfo 来获取 MTU 值的示例：

cpp
复制代码
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
3. 使用 GetIfTable 获取接口 MTU
另一种方法是使用 GetIfTable 获取接口信息，该函数返回一个接口表，可以从中获取 MTU 信息。

以下是使用 GetIfTable 获取 MTU 的示例：

cpp
复制代码
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
4. 通过套接字获取 MTU（IOCTL）
如果你想通过套接字（例如，获取与特定套接字相关的 MTU）来进行操作，可以使用 WSAIoctl 函数来发出 IOCTL
请求，查询网络接口的 MTU。

cpp
复制代码
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define IOCTL_QUERY_IF_MTU 0x8004661E  // Example IOCTL value (check actual value for your case)

void GetSocketMTU(SOCKET s) //测试失败。
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
总结
使用 GetAdaptersInfo 或 GetIfTable 获取网络接口的 MTU，通常是获取接口 MTU 的常用方式。
使用 WSAIoctl 发出 IOCTL 请求，可以通过套接字获取 MTU。
如果你需要更低级的接口访问，可能需要使用更复杂的 IOCTL 调用或直接通过原始套接字来进行操作。
*/

/*
Windows api 获取 套接字 MTU IP_MTU


Windows API 获取套接字 MTU (IP_MTU) 的方法
概述
在 Windows 平台上，我们可以通过 Winsock 提供的 API 来获取套接字的最大传输单元 (MTU)。MTU
是指网络路径上允许通过的最大数据包长度，了解 MTU 的大小对于优化网络传输、避免分片等问题非常重要。

获取 MTU 的步骤
创建套接字：
C++

SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == INVALID_SOCKET) {
    // 处理错误
}
连接到目标主机：
C++

sockaddr_in addr;
// ... 初始化 addr 结构 ...
if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
    // 处理错误
}
获取套接字选项：
C++

DWORD mtu;
int optlen = sizeof(mtu);
if (getsockopt(sock, IPPROTO_IP, IP_MTU, (char*)&mtu, &optlen) == SOCKET_ERROR) {//测试成功。
    // 处理错误
}
代码示例
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

    // ... 连接到目标主机 ...

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
注意事项
IPPROTO_IP： 指定协议为 IP 协议。
IP_MTU： 获取套接字的 MTU。
optlen： 用于接收返回数据的缓冲区大小。
错误处理： 务必对每个函数调用进行错误处理。
异步操作： 对于高性能应用，可以考虑使用异步操作来获取 MTU。
IPv6： 对于 IPv6 套接字，可以使用类似的方法，但可能需要设置不同的协议级别和选项。
其他方法
ICMP ping： 通过发送不同大小的 ICMP echo 请求包，根据返回的错误信息来推断 MTU。
Path MTU Discovery (PMTUD)： 操作系统会自动尝试发现路径上的 MTU，并在数据包被丢弃时调整分片大小。
总结
通过上述方法，我们可以方便地获取 Windows 平台上套接字的
MTU，从而为网络应用程序的优化提供参考。在实际应用中，根据具体需求选择合适的方法。

温馨提示：

MTU 的值可能会动态变化，因此在需要高精度的情况下，建议在每次连接建立时都重新获取。
如果对网络编程不熟悉，建议查阅相关文档或寻求专业人士的帮助。
希望这个回答对您有所帮助！

如果您还有其他问题，欢迎随时提出。

关键词： Windows API, 套接字, MTU, IP_MTU, Winsock, getsockopt
*/


/*
WSAIoctl MTU
ChatGPT 说：
ChatGPT
在 Windows 中，使用 WSAIoctl 来获取或设置套接字的 MTU 需要与网络接口相关的 IOCTL
控制代码。具体来说，如果你想获取与网络接口相关的 MTU，你可以使用 WSAIoctl 函数，并传递适当的 IOCTL 控制码。

但是，需要注意的是，WSAIoctl 本身并没有直接提供一个简单的控制码来查询 MTU。你可以通过一些间接方法来获得
MTU，例如查询网络接口的配置信息，而不是直接通过套接字来获取。

WSAIoctl 介绍
WSAIoctl 是 Windows 套接字 API 中的一个函数，允许你对套接字执行一些控制操作。它的原型如下：

cpp
复制代码
int WSAIoctl(
    SOCKET s,                   // 套接字句柄
    DWORD dwIoControlCode,      // 控制码
    LPVOID lpvInBuffer,         // 输入缓冲区
    DWORD cbInBuffer,           // 输入缓冲区大小
    LPVOID lpvOutBuffer,        // 输出缓冲区
    DWORD cbOutBuffer,          // 输出缓冲区大小
    LPDWORD lpcbBytesReturned,  // 返回的字节数
    LPOVERLAPPED lpOverlapped,  // 异步操作
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // 异步完成回调
);
获取 MTU 的方法
WSAIoctl 可以用来通过一些 IOCTL 控制代码与网络接口交互。但对于获取 MTU，我们通常是通过 IOCTL
控制代码来查询网络接口的属性，如 SIO_GET_INTERFACE_LIST、SIO_GET_INTERFACE_INFO 等。

尽管没有专门的 WSAIoctl 控制码直接查询 MTU，但你可以通过 SIO_GET_INTERFACE_LIST 或其他 IOCTL
控制码获取网络接口的信息，并从中提取 MTU。

示例：通过 WSAIoctl 获取 MTU（间接方法）
以下示例代码通过 WSAIoctl 调用获取网络接口的 MTU。

cpp
复制代码
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define IOCTL_QUERY_IF_MTU 0x8004661E  // 用于查询接口 MTU 的 IOCTL 控制码（具体值取决于接口类型）

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

    // 初始化 Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 创建一个套接字（可以使用任何合适的套接字类型）
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 获取 MTU
    GetMTU(s);

    // 关闭套接字并清理
    closesocket(s);
    WSACleanup();

    return 0;
}
重要说明
IOCTL_QUERY_IF_MTU 控制码是一个例子，具体的控制码应该根据你的实际需求来选择。不同的网络接口可能会使用不同的 IOCTL
控制码来获取 MTU。 SIO_GET_INTERFACE_LIST 和 SIO_GET_INTERFACE_INFO
是用于获取网络接口信息的控制码。你可以通过这些控制码间接获取 MTU 信息，但你需要解析接口表来提取 MTU。 通过
SIO_GET_INTERFACE_INFO 获取 MTU（推荐方法） 获取 MTU 的另一种方法是通过 SIO_GET_INTERFACE_INFO
控制码。你可以使用此控制码获得网络接口的详细信息，并从中提取 MTU。以下是示例代码：

cpp
复制代码
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

void GetMTU()
{
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IP_INTERFACE_INFO *pIfInfo = NULL;

    // 获取所需的缓冲区大小
    dwRetVal = WSAIoctl(INVALID_SOCKET, SIO_GET_INTERFACE_INFO, NULL, 0, NULL, 0, &dwSize, NULL, NULL);
    if (dwRetVal == SOCKET_ERROR)
    {
        printf("WSAIoctl failed with error: %d\n", WSAGetLastError());
        return;
    }

    // 分配缓冲区
    pIfInfo = (IP_INTERFACE_INFO *)malloc(dwSize);
    if (pIfInfo == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    // 获取接口信息
    dwRetVal = WSAIoctl(INVALID_SOCKET, SIO_GET_INTERFACE_INFO, NULL, 0, pIfInfo, dwSize, &dwSize, NULL, NULL);
    if (dwRetVal == SOCKET_ERROR)
    {
        printf("WSAIoctl failed with error: %d\n", WSAGetLastError());
        free(pIfInfo);
        return;
    }

    // 打印接口信息，包括 MTU
    for (int i = 0; i < pIfInfo->NumAdapters; i++)
    {
        printf("Adapter index: %d\n", pIfInfo->Adapter[i].Index);
        printf("MTU: %lu\n", pIfInfo->Adapter[i].Mtu);  // MTU 信息
    }

    free(pIfInfo);
}

int main()
{
    WSADATA wsaData;

    // 初始化 Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 获取并打印 MTU
    GetMTU();

    WSACleanup();

    return 0;
}
总结
WSAIoctl 可以用于套接字级别的 IOCTL 操作，但没有专门的控制码用于直接获取 MTU。
通过使用适当的控制码（如 SIO_GET_INTERFACE_LIST 或 SIO_GET_INTERFACE_INFO），可以间接获取与网络接口相关的 MTU
信息。 使用 WSAIoctl 来查询 MTU 的具体方法依赖于你正在处理的网络接口类型和环境。 如果你只是需要获取接口的
MTU，通常使用 GetAdaptersInfo 或 GetIfTable 这些更高层的 API 是更简单和常见的选择。
*/


#pragma once

#include "pch.h"
