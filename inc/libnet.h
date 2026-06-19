/*
此文件支持c/cpp，但是不支持cpp的特性。
为了支持rust(bindgen工具转换)，此文件采用utf-8编码，尽管也有cpp转rust的办法，还是用c为好。
为了使生成的rs文件更小，这里精简了依赖的头文件。注意：还有依赖的lib.
*/

#pragma once

#pragma warning(push)
#pragma warning(disable:28251) // SAL inconsistency between declaration and definition — definition is in DLL, no SAL there
#pragma warning(disable:28301) // SAL inconsistency between declaration and definition — same reason
#pragma warning(disable:4819)  // file contains characters not representable in current code page — file is intentionally UTF-8 for Rust bindgen


/////////////////////////////////////////////////////////////////////////////////////////////////
//一些系统的头文件和库的包含。


//#define _WIN32_WINNT 0x0501

#pragma warning(pop)

// ---------------------------------------------------------------------------
// LIBNET_API: dllexport when building the DLL, dllimport for consumers.
// Define LIBNET_EXPORTS before including this header only in the DLL project.
// ---------------------------------------------------------------------------
#ifdef LIBNET_EXPORTS
#define LIBNET_API __declspec(dllexport)
#else
#define LIBNET_API __declspec(dllimport)
#endif

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
    ULONG          saddr;//Source Address
    ULONG          daddr;//Destination Address
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


LIBNET_API
int WINAPI GetMacByIPv4Test(int argc, char ** argv);

LIBNET_API int WINAPI GetMacByIPv4(IPAddr DestIp, PDL_EUI48 MacAddr);

LIBNET_API int WINAPI EnumUnicastIpAddressTable();

LIBNET_API int WINAPI EnumAnycastIpAddress();

LIBNET_API int WINAPI EnumMulticastIpAddress();

LIBNET_API int WINAPI EnumStackInterface();

LIBNET_API int WINAPI EnumInvertedStackInterface();

LIBNET_API int WINAPI EnumIpInterface();

LIBNET_API int WINAPI EnumIpPath();

LIBNET_API
int WINAPI SendIcmpEcho(int argc, char ** argv);

LIBNET_API
int WINAPI SendIcmpEcho2(int argc, char ** argv);

LIBNET_API DWORD WINAPI Icmp6Test();

LIBNET_API
int WINAPI GetNetworkParamsTest();


//////////////////////////////////////////////////////////////////////////////////////////////////


LIBNET_API DWORD WINAPI DnsQueryTest(int argc, char * argv[]);

LIBNET_API DWORD WINAPI ModifyRecords(int argc, wchar_t * argv[]);

LIBNET_API
VOID WINAPI PrintDnsRecordList(PDNS_RECORD DnsRecord);


//////////////////////////////////////////////////////////////////////////////////////////////////


LIBNET_API DWORD WINAPI EnumIPv4ByMask(_In_z_ const char * ipv4, _In_z_ const char * mask);

LIBNET_API DWORD WINAPI EnumIPv4ByMasks(_In_z_ const char * ipv4, _In_ BYTE mask);

LIBNET_API
int WINAPI AddIPv4Address(int argc, char ** argv);

LIBNET_API
int WINAPI SetStaticIPv4(PCWSTR Ipv4, PCWSTR SubnetMask);


//////////////////////////////////////////////////////////////////////////////////////////////////


LIBNET_API DWORD WINAPI HttpReadData();

LIBNET_API
int WINAPI IWinHttpRequestSend();

LIBNET_API DWORD WINAPI GetDefaultProxyConfiguration();

LIBNET_API DWORD WINAPI GetIEProxyConfigForCurrentUser();

LIBNET_API DWORD WINAPI DetectAutoProxyConfigUrl();

LIBNET_API DWORD WINAPI GetProxyForUrl();

LIBNET_API DWORD WINAPI Sessions();

//LIBNET_API
//DWORD WINAPI RegisterProxyChangeNotification();

//LIBNET_API
//DWORD WINAPI UnregisterProxyChangeNotification();

LIBNET_API DWORD WINAPI SetDefaultProxyConfiguration();

LIBNET_API
int WINAPI SetProxy();

LIBNET_API
int WINAPI WriteProxySettings();


//////////////////////////////////////////////////////////////////////////////////////////////////
//TCP相关的。


LIBNET_API int WINAPI EnumTcpTable();

LIBNET_API
int WINAPI EnumTcpTable2();

LIBNET_API
int WINAPI EnumTcp6Table();

LIBNET_API
int WINAPI EnumTcp6Table2();

LIBNET_API
int WINAPI EnumTcpStatistics();

LIBNET_API
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass);

LIBNET_API
int WINAPI RunEstats();


//////////////////////////////////////////////////////////////////////////////////////////////////
//UDP相关的。


LIBNET_API
int WINAPI EnumUdpTable();

LIBNET_API
int WINAPI EnumUdp6Table();

LIBNET_API
int WINAPI EnumExtendedUdpTable4();

LIBNET_API
int WINAPI EnumExtendedUdpTable6();


//////////////////////////////////////////////////////////////////////////////////////////////////


LIBNET_API int WINAPI EnumEntityArray();

LIBNET_API
int WINAPI TestPersistentTcpPortReservation(int argc, WCHAR ** argv);

LIBNET_API
int WINAPI GetPortReservation(int argc, WCHAR ** argv);

LIBNET_API
int WINAPI TestLookupPersistentTcpPortReservation(int argc, WCHAR ** argv);

LIBNET_API
int WINAPI Exclusiveaddruse(int argc, wchar_t ** argv);

LIBNET_API
int WINAPI EnumAdaptersInfo();

LIBNET_API
int WINAPI EnumInterfaceInfo();

LIBNET_API
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway);

LIBNET_API int WINAPI GetGatewayMacByIPv6(const char * IPv6, PDL_EUI48 GatewayMac);

LIBNET_API int WINAPI GetGatewayMacByIPv4(const char * IPv4, PDL_EUI48 GatewayMac);

LIBNET_API
int WINAPI EnumIpNetTable();

LIBNET_API
BOOL WINAPI GetIPv4ByMac(_In_ PDL_EUI48 Mac, _Inout_ PIN_ADDR IPv4);

LIBNET_API
int WINAPI EnumIpNetTable2(_In_ ADDRESS_FAMILY Family);

LIBNET_API DWORD WINAPI ResolveIpNetEntry2Test(_In_z_ const char * ip);

LIBNET_API DWORD WINAPI GetMacByIPv6(_In_z_ const char * IPv6, _Out_ PDL_EUI48 Mac);

LIBNET_API BOOL WINAPI GetMacByGatewayIPv6(const char * ipv6, PDL_EUI48 mac); // c没有bool.

LIBNET_API
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family);


//////////////////////////////////////////////////////////////////////////////////////////////////
//RAW相关的。


LIBNET_API void WINAPI PacketizeAck4(_In_ PIPV4_HEADER IPv4Header, _In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac, _Out_ PRAW_TCP buffer);

LIBNET_API void WINAPI calculation_icmpv6_echo_request_checksum(_Out_writes_bytes_(OptLen) PBYTE buffer, _In_ int OptLen);

LIBNET_API void WINAPI PacketizeAck6(_In_ PIPV6_HEADER IPv6Header, _In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac, _Out_ PRAW6_TCP buffer);

LIBNET_API void WINAPI PacketizeSyn4(_In_ PDL_EUI48 SrcMac,
                                     _In_ PDL_EUI48 DesMac,
                                     _In_ PIN_ADDR SourceAddress,
                                     _In_ PIN_ADDR DestinationAddress,
                                     _In_ UINT16 th_sport,
                                     _In_ UINT16 th_dport,
                                     _Out_ PBYTE buffer);

LIBNET_API void WINAPI PacketizeSyn6(_In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac,
                                     _In_ PIN6_ADDR SourceAddress,
                                     _In_ PIN6_ADDR DestinationAddress,
                                     _In_ UINT16 th_sport,
                                     _In_ UINT16 th_dport,
                                     _Out_ PBYTE buffer);

LIBNET_API void WINAPI packetize_icmpv4_echo_request(_In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac,
                                                     _In_ PIN_ADDR SourceAddress, _In_ PIN_ADDR DestinationAddress,
                                                     _Out_ PBYTE buffer);

LIBNET_API void WINAPI InitIpv6Header(_In_ PIN6_ADDR SourceAddress, _In_ PIN6_ADDR DestinationAddress,
                                      _In_ UINT8 NextHeader, _In_ UINT16 OptLen,
                                      _Out_ PIPV6_HEADER IPv6Header);

LIBNET_API void WINAPI packetize_icmpv6_echo_request(_In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac,
                                                     _In_ PIN6_ADDR SourceAddress, _In_ PIN6_ADDR DestinationAddress,
                                                     _Out_ PBYTE buffer);

LIBNET_API USHORT WINAPI calc_udp4_sum(_In_reads_(size) USHORT * buffer, _In_ int size);

LIBNET_API USHORT WINAPI calc_udp6_sum(_In_reads_(size) USHORT * buffer, _In_ int size);

LIBNET_API USHORT WINAPI calc_icmp4_sum(_In_ PICMP_HEADER icmp, _In_ int size);

LIBNET_API USHORT WINAPI checksum(_In_reads_(size) USHORT * buffer, _In_ int size);

LIBNET_API void WINAPI InitEthernetHeader(_In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac, _In_ UINT16 Type, _Out_ PETHERNET_HEADER eth_hdr);

LIBNET_API void WINAPI InitIpv4Header(_In_ PIN_ADDR SourceAddress, _In_ PIN_ADDR DestinationAddress,
                                      _In_ UINT8 Protocol, _In_ UINT16 TotalLength,
                                      _Out_ PIPV4_HEADER IPv4Header);

LIBNET_API _Ret_writes_maybenull_z_(1) PVOID WINAPI PacketizeUdp4(_In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac,
                                                                   _In_ PIN_ADDR SourceAddress, _In_ PIN_ADDR DestinationAddress,
                                                                   _In_ WORD SourcePort, _In_ WORD DestinationPort,
                                                                   _In_reads_bytes_(DataLen) PBYTE Data, _In_ WORD DataLen);

LIBNET_API _Ret_writes_maybenull_z_(1) PVOID WINAPI PacketizeUdp6(_In_ PDL_EUI48 SrcMac, _In_ PDL_EUI48 DesMac,
                                                                   _In_ PIN6_ADDR SourceAddress, _In_ PIN6_ADDR DestinationAddress,
                                                                   _In_ WORD SourcePort, _In_ WORD DestinationPort,
                                                                   _In_reads_bytes_(DataLen) PBYTE Data, _In_ WORD DataLen);


//////////////////////////////////////////////////////////////////////////////////////////////////
//防火墙相关的。


LIBNET_API
int WINAPI GettingFirewallSettings();


//////////////////////////////////////////////////////////////////////////////////////////////////


LIBNET_API int WINAPI getaddrinfo_test(int argc, char ** argv);
LIBNET_API int WINAPI GetAddrInfoW_test(int argc, wchar_t ** argv);


//////////////////////////////////////////////////////////////////////////////////////////////////


// Returns a heap-allocated null-terminated ASCII string containing the public IPv4 address.
// Caller must free the returned pointer with HeapFree(GetProcessHeap(), 0, ptr).
// Returns NULL on failure.
LIBNET_API _Ret_maybenull_ _Post_writable_byte_size_(16) char * WINAPI GetPublicIPv4();


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
