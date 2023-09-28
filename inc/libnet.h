/*
���ļ�֧��c/cpp�����ǲ�֧��cpp�����ԡ�
Ϊ��֧��rust(bindgen����ת��)�����ļ�����utf-8���룬����Ҳ��cppתrust�İ취��������cΪ�á�
Ϊ��ʹ���ɵ�rs�ļ���С�����ﾫ����������ͷ�ļ���ע�⣺����������lib.
*/

#pragma once

#pragma warning(disable:28251)
#pragma warning(disable:28301)
#pragma warning(disable:4819) //���ļ����������ڵ�ǰ����ҳ(936)�б�ʾ���ַ����뽫���ļ�����Ϊ Unicode ��ʽ�Է�ֹ���ݶ�ʧ


/////////////////////////////////////////////////////////////////////////////////////////////////
//һЩϵͳ��ͷ�ļ��Ϳ�İ�����


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
�ɸ��ƻ�ο�
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
//    TCP_OPT_MSS mss;//4�ֽڶ���
//    TCP_OPT_WS ws;//4�ֽڶ���
//    TCP_OPT_SACK_PERMITTED sp;//4�ֽڶ���
//
//    //���ɿ�������ӱ�ġ�
//} TCP_OPT, * PTCP_OPT;


typedef struct _tcp_opt {
    TCP_OPT_MSS mss;//4�ֽڶ���
    BYTE unuse1;
    TCP_OPT_WS ws;//4�ֽڶ���
    WORD unuse2;
    TCP_OPT_SACK_PERMITTED sp;//4�ֽڶ���
} TCP_OPT, * PTCP_OPT;


#pragma warning(push)
#pragma warning(disable : 4200) //ʹ���˷Ǳ�׼��չ: �ṹ/�����е����С����
typedef struct raw_tcp {
    ETHERNET_HEADER eth_hdr;
    IPV4_HEADER ip_hdr;
    TCP_HDR tcp_hdr;

    //΢���tcp opt �Ķ���Ϊ4�ֽڣ�������Ķ����С��һ�¡�
    //����������4�ֽ�Ҳ�У�����ͨ�������������4�ֽڣ���Ϊ��TCP��th_len��4�ı�����

    //TCP_OPT_MSS mss;//4�ֽڶ���
    //TCP_OPT_WS ws;//4�ֽڶ���
    //TCP_OPT_SACK_PERMITTED sp;//4�ֽڶ���
    //BYTE data[0];
} RAW_TCP, * PRAW_TCP;
#pragma warning(pop)  


/*
the TCP and UDP "pseudo-header" for IPv6

https://www.ietf.org/rfc/rfc2460.txt
https://www.microsoftpressstore.com/articles/article.aspx?p=2225063&seqNum=6
�������IPv6(������)��4.6�½ڡ�

��ɲο���
\Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c
��ComputeUdpPseudoHeaderChecksumV6������
����\Windows-classic-samples\Samples\Win7Samples\netds\winsock\ping\Ping.cpp
��ComputeIcmp6PseudoHeaderChecksum������
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
#pragma warning(disable : 4200) //ʹ���˷Ǳ�׼��չ: �ṹ/�����е����С����
typedef struct raw6_tcp {
    ETHERNET_HEADER eth_hdr;
    IPV6_HEADER ip_hdr;
    TCP_HDR tcp_hdr;

    //tcp opt �Ķ���Ϊ4�ֽڣ�������Ĳ�һ�¡�
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
//TCP��صġ�


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
//UDP��صġ�


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
BOOL WINAPI GetMacByGatewayIPv6(const char * ipv6, PBYTE mac);//cû��bool.

__declspec(dllimport)
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family);


//////////////////////////////////////////////////////////////////////////////////////////////////
//spi


__declspec(dllimport)
int WINAPI EnumProtocols_WSC();

__declspec(dllimport)
int WINAPI EnumProtocols32_WSC();


//////////////////////////////////////////////////////////////////////////////////////////////////
//RAW��صġ�


__declspec(dllimport)
void WINAPI PacketizeAck4(IN PIPV4_HEADER IPv4Header, IN PBYTE SrcMac, IN PBYTE DesMac, OUT PRAW_TCP buffer);

__declspec(dllimport)
void WINAPI PacketizeAck6(IN PIPV6_HEADER IPv6Header, IN PBYTE SrcMac, IN PBYTE DesMac, OUT PRAW6_TCP buffer);

__declspec(dllimport)
void WINAPI PacketizeSyn4(IN PBYTE SrcMac,   //6�ֽڳ��ı��ص�MAC��
                          IN PBYTE DesMac,
                          IN PIN_ADDR SourceAddress,
                          IN PIN_ADDR DestinationAddress,
                          IN UINT16 th_sport,//���������������������htonsת���¡�
                          IN UINT16 th_dport,//���������������������htonsת���¡�
                          OUT PBYTE buffer   //������sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)��
);

__declspec(dllimport)
void WINAPI PacketizeSyn6(IN PBYTE SrcMac,
                          IN PBYTE DesMac,
                          IN PIN6_ADDR SourceAddress,
                          IN PIN6_ADDR DestinationAddress,
                          IN UINT16 th_sport,//���������������������htonsת���¡�
                          IN UINT16 th_dport,//���������������������htonsת���¡�
                          OUT PBYTE buffer    //������sizeof(RAW6_TCP)��
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
