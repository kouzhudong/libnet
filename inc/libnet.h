/*
此文件支持c/cpp，但是不支持cpp的特性。
为了支持rust(bindgen工具转换)，此文件采用utf-8编码，尽管也有cpp转rust的办法，还是用c为好。
为了使生成的rs文件更小，这里精简了依赖的头文件。注意：还有依赖的lib.
*/

#pragma once

#pragma warning(disable:28251)
#pragma warning(disable:28301)
#pragma warning(disable:4819) //该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失


/////////////////////////////////////////////////////////////////////////////////////////////////
//一些系统的头文件和库的包含。


//#define _WIN32_WINNT 0x0501

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <netiodef.h>
#include <iphlpapi.h>
#include <windns.h>   //DNS api's


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
可复制或参考
\Windows-classic-samples\Samples\Win7Samples\netds\winsock\rcvall\iphdr.h
\Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\iphdr.h
*/


#include <pshpack1.h>


typedef struct tsd_hdr {
    unsigned long  saddr;//Source Address
    unsigned long  daddr;//Destination Address 
    char           mbz;  //zero
    char           ptcl; //PTCL 
    unsigned short tcpl; //TCP Length
}PSD_HEADER;


//typedef __declspec(align(4)) struct _tcp_opt {
//    TCP_OPT_MSS mss;//4字节对齐
//    TCP_OPT_WS ws;//4字节对齐
//    TCP_OPT_SACK_PERMITTED sp;//4字节对齐
//
//    //还可考虑再添加别的。
//} TCP_OPT, * PTCP_OPT;


typedef struct _tcp_opt {
    TCP_OPT_MSS mss;//4字节对齐
    BYTE unuse1;
    TCP_OPT_WS ws;//4字节对齐
    WORD unuse2;
    TCP_OPT_SACK_PERMITTED sp;//4字节对齐
} TCP_OPT, * PTCP_OPT;


#pragma warning(push)
#pragma warning(disable : 4200) //使用了非标准扩展: 结构/联合中的零大小数组
typedef struct raw_tcp {
    ETHERNET_HEADER eth_hdr;
    IPV4_HEADER ip_hdr;
    TCP_HDR tcp_hdr;

    //微软的tcp opt 的对齐为4字节，和这里的对齐大小不一致。
    //不过，不是4字节也行（测试通过），但是最好4字节，因为：TCP的th_len是4的倍数。

    //TCP_OPT_MSS mss;//4字节对齐
    //TCP_OPT_WS ws;//4字节对齐
    //TCP_OPT_SACK_PERMITTED sp;//4字节对齐
    //BYTE data[0];
} RAW_TCP, * PRAW_TCP;
#pragma warning(pop)  


/*
the TCP and UDP "pseudo-header" for IPv6

https://www.ietf.org/rfc/rfc2460.txt
https://www.microsoftpressstore.com/articles/article.aspx?p=2225063&seqNum=6
深入解析IPv6(第三版)的4.6章节。

亦可参考：
\Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c
的ComputeUdpPseudoHeaderChecksumV6函数。
或者\Windows-classic-samples\Samples\Win7Samples\netds\winsock\ping\Ping.cpp
的ComputeIcmp6PseudoHeaderChecksum函数。
*/
typedef struct tsd6_hdr {
    IN6_ADDR      saddr;//Source Address
    IN6_ADDR      daddr;//Destination Address 
    unsigned long length;
    char          unused1;//zero
    char          unused2;//zero
    char          unused3;//zero
    char          proto;
}PSD6_HEADER;


#pragma warning(push)
#pragma warning(disable : 4200) //使用了非标准扩展: 结构/联合中的零大小数组
typedef struct raw6_tcp {
    ETHERNET_HEADER eth_hdr;
    IPV6_HEADER ip_hdr;
    TCP_HDR tcp_hdr;

    //tcp opt 的对齐为4字节，和这里的不一致。
    //BYTE data[0];
} RAW6_TCP, * PRAW6_TCP;
#pragma warning(pop)  


#include <poppack.h>


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllimport)
void WINAPI RegistersNotifyIpInterfaceChange();

__declspec(dllimport)
void WINAPI DeregisterNotifyIpInterfaceChange();

__declspec(dllimport)
void WINAPI RegistersNotifyNetworkConnectivityHintChange();

__declspec(dllimport)
void WINAPI DeregisterNotifyRouteChange2();

__declspec(dllimport)
void WINAPI RegistersNotifyRouteChange2();

__declspec(dllimport)
void WINAPI DeregisterNotifyStableUnicastIpAddressTable();

__declspec(dllimport)
void WINAPI RegistersNotifyStableUnicastIpAddressTable();

__declspec(dllimport)
void WINAPI DeregisterNotifyTeredoPortChange();

__declspec(dllimport)
void WINAPI RegistersNotifyTeredoPortChange();

__declspec(dllimport)
void WINAPI DeregisterNotifyUnicastIpAddressChange();

__declspec(dllimport)
void WINAPI RegistersNotifyUnicastIpAddressChange();

__declspec(dllimport)
int WINAPI GetMacByIPv4Test(int argc, char ** argv);

__declspec(dllimport)
int WINAPI GetMacByIPv4(IPAddr DestIp, PBYTE MacAddr);

__declspec(dllimport)
int WINAPI EnumUnicastIpAddressTable();

__declspec(dllimport)
void WINAPI Icmp6Test();

__declspec(dllimport)
int WINAPI GetNetworkParamsTest();

__declspec(dllimport)
void WINAPI NotifyAddrChangeTest();


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
int WINAPI tracert(int argc, char ** argv);

__declspec(dllimport)
int WINAPI pathping(int argc, char ** argv);


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI DnsQueryTest(int argc, char * argv[]);

__declspec(dllimport)
void WINAPI ModifyRecords(int argc, wchar_t * argv[]);

__declspec(dllimport)
VOID WINAPI PrintDnsRecordList(PDNS_RECORD DnsRecord);


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void EnumWfpInfo(int argc, wchar_t * argv[]);


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI EnumIPv4ByMask(const char * ipv4, const char * mask);

__declspec(dllimport)
void WINAPI EnumIPv4ByMasks(const char * ipv4, BYTE mask);


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI HttpReadData();

__declspec(dllimport)
int WINAPI IWinHttpRequestSend();

__declspec(dllimport)
void WINAPI GetDefaultProxyConfiguration();

__declspec(dllimport)
void WINAPI GetIEProxyConfigForCurrentUser();

__declspec(dllimport)
void WINAPI DetectAutoProxyConfigUrl();

__declspec(dllimport)
void WINAPI GetProxyForUrl();

__declspec(dllimport)
void WINAPI Sessions();

//__declspec(dllimport)
//DWORD WINAPI RegisterProxyChangeNotification();

//__declspec(dllimport)
//DWORD WINAPI UnregisterProxyChangeNotification();

__declspec(dllexport)
void WINAPI SetDefaultProxyConfiguration();

__declspec(dllexport)
int WINAPI SetProxy();

__declspec(dllexport)
int WINAPI WriteProxySettings();


//////////////////////////////////////////////////////////////////////////////////////////////////
//TCP相关的。


__declspec(dllimport)
void WINAPI EnumTcpTable();

__declspec(dllimport)
int WINAPI EnumTcpTable2();

__declspec(dllimport)
int WINAPI EnumTcp6Table();

__declspec(dllimport)
int WINAPI EnumTcp6Table2();

__declspec(dllimport)
int WINAPI EnumTcpStatistics();

__declspec(dllimport)
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass);

__declspec(dllimport)
int WINAPI RunEstats();


//////////////////////////////////////////////////////////////////////////////////////////////////
//UDP相关的。


__declspec(dllimport)
int WINAPI EnumUdpTable();

__declspec(dllimport)
int WINAPI EnumUdp6Table();

__declspec(dllimport)
int WINAPI EnumExtendedUdpTable4();

__declspec(dllimport)
int WINAPI EnumExtendedUdpTable6();


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI EnumEntityArray();

__declspec(dllimport)
int WINAPI TestPersistentTcpPortReservation(int argc, WCHAR ** argv);

__declspec(dllimport)
int WINAPI GetPortReservation(int argc, WCHAR ** argv);

__declspec(dllimport)
int WINAPI TestLookupPersistentTcpPortReservation(int argc, WCHAR ** argv);

__declspec(dllimport)
int WINAPI Exclusiveaddruse(int argc, wchar_t ** argv);

__declspec(dllimport)
int WINAPI EnumAdaptersInfo();

__declspec(dllimport)
int WINAPI EnumInterfaceInfo();

__declspec(dllimport)
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway);

__declspec(dllimport)
int WINAPI GetGatewayMacByIPv6(const char * IPv6, PBYTE GatewayMac);

__declspec(dllimport)
int WINAPI GetGatewayMacByIPv4(const char * IPv4, PBYTE GatewayMac);

__declspec(dllimport)
int WINAPI EnumIpNetTable();

__declspec(dllimport)
BOOL WINAPI GetIPv4ByMac(_In_ PDL_EUI48 Mac, _Inout_ PIN_ADDR IPv4);

__declspec(dllimport)
int WINAPI EnumIpNetTable2(_In_ ADDRESS_FAMILY Family);

__declspec(dllimport)
void WINAPI ResolveIpNetEntry2Test(const char * ip);

__declspec(dllimport)
void WINAPI GetMacByIPv6(const char * IPv6, PBYTE Mac);

__declspec(dllimport)
BOOL WINAPI GetMacByGatewayIPv6(const char * ipv6, PBYTE mac);//c没有bool.

__declspec(dllimport)
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family);


//////////////////////////////////////////////////////////////////////////////////////////////////
//spi


__declspec(dllimport)
int WINAPI EnumProtocols_WSC();

__declspec(dllimport)
int WINAPI EnumProtocols32_WSC();


//////////////////////////////////////////////////////////////////////////////////////////////////
//RAW相关的。


__declspec(dllimport)
void WINAPI PacketizeAck4(IN PIPV4_HEADER IPv4Header, IN PBYTE SrcMac, IN PBYTE DesMac, OUT PRAW_TCP buffer);

__declspec(dllimport)
void WINAPI PacketizeAck6(IN PIPV6_HEADER IPv6Header, IN PBYTE SrcMac, IN PBYTE DesMac, OUT PRAW6_TCP buffer);

__declspec(dllimport)
void WINAPI PacketizeSyn4(IN PBYTE SrcMac,   //6字节长的本地的MAC。
                          IN PBYTE DesMac,
                          IN PIN_ADDR SourceAddress,
                          IN PIN_ADDR DestinationAddress,
                          IN UINT16 th_sport,//网络序。如果是主机序，请用htons转换下。
                          IN UINT16 th_dport,//网络序。如果是主机序，请用htons转换下。
                          OUT PBYTE buffer   //长度是sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)。
);

__declspec(dllimport)
void WINAPI PacketizeSyn6(IN PBYTE SrcMac,
                          IN PBYTE DesMac,
                          IN PIN6_ADDR SourceAddress,
                          IN PIN6_ADDR DestinationAddress,
                          IN UINT16 th_sport,//网络序。如果是主机序，请用htons转换下。
                          IN UINT16 th_dport,//网络序。如果是主机序，请用htons转换下。
                          OUT PBYTE buffer    //长度是sizeof(RAW6_TCP)。
);

__declspec(dllimport)
USHORT WINAPI calc_udp4_sum(USHORT * buffer, int size);

__declspec(dllimport)
USHORT WINAPI calc_icmp4_sum(PICMP_HEADER icmp, int size);

__declspec(dllimport)
USHORT WINAPI checksum(USHORT * buffer, int size);


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI TestNetworkListManager();

__declspec(dllimport)
void WINAPI TestNetworkListManagerEvents();

__declspec(dllimport)
void WINAPI ListenToNetworkConnectivityChangesSample(BOOL optedIn);


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
