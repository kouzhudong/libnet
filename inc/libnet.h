#pragma once

#pragma warning(disable:28251)
#pragma warning(disable:28301)


/////////////////////////////////////////////////////////////////////////////////////////////////
//һЩϵͳ��ͷ�ļ��Ϳ�İ�����


//#define _WIN32_WINNT 0x0501

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#ifndef UNICODE
#define UNICODE
#endif

#include <Winsock2.h>
#include <windows.h>
#include <strsafe.h>
#include <assert.h>
#include <crtdbg.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <winioctl.h>
#include <string.h>
#include <fltuser.h>
#include <locale.h>
#include <Lmserver.h>
#include <stdarg.h>
#include <wincrypt.h>
#include <intrin.h>
#include <TlHelp32.h>
#include <aclapi.h>
#include <VersionHelpers.h>
#include <ShlDisp.h>
#include <Shlobj.h>
#include <Softpub.h>
#include <mscat.h>
#include <WinUser.h>
#include <direct.h>
#include <sddl.h>
#include <ws2tcpip.h>
#include <fwpsu.h>
#include <atlbase.h>
#include <mbnapi.h>
#include <iostream>
#include <netfw.h>
#include <atlcomcli.h>
#include <objbase.h>
#include <oleauto.h>
#include <atlconv.h>
#define _WS2DEF_
#include <mstcpip.h>
#include <Intshcut.h>
//#include <winternl.h>
#include <SubAuth.h>
//#include <NTSecAPI.h>
//#include <ntdef.h>
//#include <netioapi.h>
//#include <LsaLookup.h>
#include <netiodef.h>
#include <atlstr.h>
#include <comutil.h>
#include <wbemidl.h>
#include <dbt.h>
#include <lm.h>
#include <winnetwk.h>
#include <ws2spi.h>
#include <comdef.h>

#include <initguid.h> //ע��ǰ��˳�򡣾�̬����UUID�õģ�����error LNK2001��
#include <usbioctl.h>
#include <usbiodef.h>
//#include <usbctypes.h>
#include <intsafe.h>
#include <specstrings.h>
#include <usb.h>
#include <usbuser.h>

#include <wincon.h> 
#include <time.h> 
#include <fwpmu.h>
#include <conio.h>
#include <nb30.h>

#pragma comment(lib, "fwpuclnt.lib") 
#pragma comment(lib, "Rpcrt4.lib")

#pragma comment(lib, "mpr.lib")

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Version.lib") 
//#pragma comment (lib,"Url.lib")
#pragma comment(lib, "wbemuuid.lib")

#include <bcrypt.h>
#pragma comment (lib, "Bcrypt.lib")

#include <shellapi.h>
#pragma comment (lib, "Shell32.lib")

#include <ncrypt.h>
#pragma comment (lib, "Ncrypt.lib")

#include <wintrust.h>
#pragma comment (lib, "wintrust.lib")

#include <Setupapi.h>
#pragma comment (lib,"Setupapi.lib")

#include <Shlwapi.h>
#pragma comment (lib,"Shlwapi.lib")

#include <DbgHelp.h>
#pragma comment (lib,"DbgHelp.lib")

#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

#include <Sfc.h>
#pragma comment(lib, "Sfc.lib")

//#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")

#pragma comment(lib,"Netapi32.lib")

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include <Userenv.h>
#pragma comment(lib, "Userenv.lib")

#include <Sensapi.h>
#pragma comment (lib,"Sensapi.lib")

#include <string>
#include <list>
#include <regex>
#include <map>
#include <set>

using namespace std;


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
int WINAPI RunEstats();

__declspec(dllimport)
int WINAPI GetMacByIPv4(int argc, char ** argv);


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI EnumIPv4ByMask(const char * ipv4, const char * mask);


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


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI EnumTcpTable();

__declspec(dllimport)
void WINAPI EnumEntityArray();

__declspec(dllimport)
int WINAPI EnumExtendedTcp4TableByPid();

__declspec(dllimport)
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass);

__declspec(dllimport)
int WINAPI EnumExtendedUdpTable4();

__declspec(dllimport)
int WINAPI EnumExtendedUdpTable6();

__declspec(dllimport)
int WINAPI EnumdaptersInfo();


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI PacketizeAck4(IN PIPV4_HEADER in_ipv4, IN PBYTE SrcMac, OUT PRAW_TCP buffer);

__declspec(dllimport)
void WINAPI PacketizeAck6(IN PIPV6_HEADER ipv6, IN PBYTE SrcMac, OUT PRAW6_TCP buffer);

__declspec(dllimport)
void WINAPI PacketizeSyn4(IN PBYTE SrcMac,   //6�ֽڳ��ı��ص�MAC��
                          IN PIN_ADDR SourceAddress,
                          IN PIN_ADDR DestinationAddress,
                          IN UINT16 th_sport,//���������������������htonsת���¡�
                          IN UINT16 th_dport,//���������������������htonsת���¡�
                          OUT PBYTE buffer   //������sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)��
);

__declspec(dllimport)
void WINAPI PacketizeSyn6(IN PBYTE SrcMac,
                          IN PIN6_ADDR SourceAddress,
                          IN PIN6_ADDR DestinationAddress,
                          IN UINT16 th_sport,//���������������������htonsת���¡�
                          IN UINT16 th_dport,//���������������������htonsת���¡�
                          OUT PBYTE buffer    //������sizeof(RAW6_TCP)��
);


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
