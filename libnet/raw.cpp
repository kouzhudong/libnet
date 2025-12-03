#include "pch.h"
#include "raw.h"
#include "udp.h"


#pragma warning(disable : 4366) //一元“&”运算符的结果可能是未对齐的


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
USHORT WINAPI checksum(USHORT * buffer, int size)
//摘自Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c
// Description:
//    This function calculates the 16-bit one's complement sum for the supplied buffer.
{
    unsigned long cksum = 0;

    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }

    // If the buffer was not a multiple of 16-bits, add the last byte
    if (size) {
        cksum += *(UCHAR *)buffer;
    }

    // Add the low order 16-bits to the high order 16-bits
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    // Take the 1's complement
    return (USHORT)(~cksum);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI InitEthernetHeader(IN PBYTE SrcMac, IN PBYTE DesMac, IN UINT16 Type, OUT PETHERNET_HEADER eth_hdr)
/*
功能：填写以太头。

参数：
Type，取值，如：ETHERNET_TYPE_IPV4，ETHERNET_TYPE_IPV6， ETHERNET_TYPE_ARP等。

注释：
1.填写虚假的目的MAC，也可发送出去。
2.如果是想接收包，还是建议填写正确的目标的MAC（局域网的）.
3.这个MAC需要计算，如网关的MAC。
*/
{
    eth_hdr->Destination.Byte[0] = DesMac[0];
    eth_hdr->Destination.Byte[1] = DesMac[1];
    eth_hdr->Destination.Byte[2] = DesMac[2];
    eth_hdr->Destination.Byte[3] = DesMac[3];
    eth_hdr->Destination.Byte[4] = DesMac[4];
    eth_hdr->Destination.Byte[5] = DesMac[5];

    eth_hdr->Source.Byte[0] = SrcMac[0];
    eth_hdr->Source.Byte[1] = SrcMac[1];
    eth_hdr->Source.Byte[2] = SrcMac[2];
    eth_hdr->Source.Byte[3] = SrcMac[3];
    eth_hdr->Source.Byte[4] = SrcMac[4];
    eth_hdr->Source.Byte[5] = SrcMac[5];

    eth_hdr->Type = ntohs(Type);
}


void InitIpv4Header(IN PIN_ADDR SourceAddress, IN PIN_ADDR DestinationAddress, IN UINT8 Protocol, IN UINT16 TotalLength, OUT PIPV4_HEADER IPv4Header)
/*
功能：组装IPv4头。

TotalLength 严格计算数据的大小。
*/
{
    IPv4Header->VersionAndHeaderLength = (4 << 4) | (sizeof(IPV4_HEADER) / sizeof(unsigned long));
    IPv4Header->TotalLength = ntohs(TotalLength);
    IPv4Header->Identification = ntohs(0);
    IPv4Header->DontFragment = TRUE;
    IPv4Header->TimeToLive = 128;
    IPv4Header->Protocol = Protocol;//取值，如：IPPROTO_TCP等。
    IPv4Header->SourceAddress.S_un.S_addr = SourceAddress->S_un.S_addr;
    IPv4Header->DestinationAddress.S_un.S_addr = DestinationAddress->S_un.S_addr;
    IPv4Header->HeaderChecksum = checksum(reinterpret_cast<unsigned short *>(IPv4Header), sizeof(IPV4_HEADER));
}


void InitIpv4Header(IN PIPV4_HEADER InIPv4Header, IN UINT16 TotalLength, IN bool IsCopy, OUT PIPV4_HEADER OutIPv4Header)
/*
功能：把in_ipv4的SYN包里的ipv4信息组装为buffer的要发生的ACK的ipv4。

用途：欺骗（扫描），而不是扫描和攻击。

参数：
IsCopy：是复制还是回复。

注意：如果是回复的包，要把源和目的换一下。
*/
{
    if (IsCopy) {
        InitIpv4Header(&InIPv4Header->SourceAddress, &InIPv4Header->DestinationAddress, InIPv4Header->Protocol, TotalLength, OutIPv4Header);
    } else {
        InitIpv4Header(&InIPv4Header->DestinationAddress, &InIPv4Header->SourceAddress, InIPv4Header->Protocol, TotalLength, OutIPv4Header);
    }
}


void InitTcpHeader(IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                   IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                   IN SEQ_NUM th_ack,  //网络序。如果是主机序，请用htonl转换下。
                   IN UINT8 th_flags,  // TH_ACK, TH_SYN等值的组合。
                   IN UINT8 OptLen, OUT PTCP_HDR tcp_hdr)
/*
功能：组装TCP头（总共十个成员）。

注意：
1.不重要的值，默认为0.
2.某些值为自己设定的固定的值。
3.校验和为0，后面再计算。
*/
{
    RtlZeroMemory(tcp_hdr, sizeof(TCP_HDR));

    tcp_hdr->th_sport = th_sport;
    tcp_hdr->th_dport = th_dport;

    tcp_hdr->th_seq = ntohl(0);
    tcp_hdr->th_ack = th_ack;

    UINT8 x = (sizeof(TCP_HDR) + OptLen) / 4;
    ASSERT(x <= 0xf); //大于这个数会发生溢出，有想不到的结果。
    tcp_hdr->th_len = x;

    tcp_hdr->th_flags = th_flags;
    tcp_hdr->th_win = ntohs(65535);

    tcp_hdr->th_sum = 0;
    tcp_hdr->th_urp = 0;
    tcp_hdr->th_sum = 0;
}


void InitTcpHeaderBySyn(IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                        IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                        IN UINT8 OptLen, OUT PTCP_HDR tcp_hdr)
{
    InitTcpHeader(th_sport, th_dport, 0, TH_SYN, OptLen, tcp_hdr);
}


void InitTcpHeaderWithAck(IN PTCP_HDR tcp, IN bool IsCopy, OUT PTCP_HDR tcp_hdr)
/*
用途：欺骗（扫描），而不是扫描和攻击。
*/
{
    if (IsCopy) {
        InitTcpHeader(tcp->th_sport,
                      tcp->th_dport,
                      tcp->th_seq + 1, //收到的号加一。
                      TH_ACK | TH_SYN,
                      0,
                      tcp_hdr);
    } else {
        InitTcpHeader(tcp->th_dport,
                      tcp->th_sport,
                      tcp->th_seq + 1, //收到的号加一。
                      TH_ACK | TH_SYN,
                      0,
                      tcp_hdr);
    }
}


void InitTcpMss(OUT TCP_OPT_MSS * mss)
{
    mss->Kind = TH_OPT_MSS;
    mss->Length = 4;
    mss->Mss = ntohs(1460);
}


void InitTcpMss(OUT PTCP_OPT tcp_opt)
{
    InitTcpMss(&tcp_opt->mss);

    tcp_opt->unuse1 = TH_OPT_NOP;
    tcp_opt->unuse2 = TH_OPT_NOP << 8 | TH_OPT_NOP;
}


void InitTcpWs(OUT TCP_OPT_WS * ws)
{
    ws->Kind = TH_OPT_WS;
    ws->Length = 3;
    ws->ShiftCnt = 8;
}


void InitTcpSp(OUT TCP_OPT_SACK_PERMITTED * sp)
{
    sp->Kind = TH_OPT_SACK_PERMITTED;
    sp->Length = 2;
}


void CalculationTcp4Sum(OUT PBYTE buffer, WORD OptLen)
/*
功能：计算并设置tcp的校验和。

参数：OptLen，是tcp的扩展选项（TCP_OPT）或者额外附带的数据（如http的html等)， 不包括ETHERNET_HEADER，IPV4_HEADER，TCP_HDR。
*/
{
    PRAW_TCP tcp4 = reinterpret_cast<PRAW_TCP>(buffer);

    PBYTE temp = reinterpret_cast<PBYTE>(MALLOC(sizeof(PSD_HEADER) + sizeof(TCP_HDR) + OptLen));
    _ASSERTE(temp);

    PSD_HEADER * PseudoHeader = reinterpret_cast<PSD_HEADER *>(temp);

    PseudoHeader->saddr = tcp4->ip_hdr.SourceAddress.S_un.S_addr;
    PseudoHeader->daddr = tcp4->ip_hdr.DestinationAddress.S_un.S_addr;
    PseudoHeader->mbz = 0;
    PseudoHeader->ptcl = IPPROTO_TCP;
    PseudoHeader->tcpl = ntohs(sizeof(TCP_HDR) + OptLen);

    PBYTE test = &temp[0] + sizeof(PSD_HEADER);
    RtlCopyMemory(test, &tcp4->tcp_hdr, sizeof(TCP_HDR));

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&tcp4->tcp_hdr + sizeof(TCP_HDR), OptLen);

    tcp4->tcp_hdr.th_sum = checksum(reinterpret_cast<USHORT *>(temp), sizeof(PSD_HEADER) + sizeof(TCP_HDR) + OptLen);

    FREE(temp);
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeAck4(IN PIPV4_HEADER IPv4Header, IN PBYTE SrcMac, IN PBYTE DesMac, OUT PRAW_TCP buffer)
{
    ASSERT(SrcMac);
    ASSERT(buffer);

    PTCP_HDR tcp = (PTCP_HDR)((PBYTE)IPv4Header + Ip4HeaderLengthInBytes(IPv4Header));

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV4, &buffer->eth_hdr);
    InitIpv4Header(IPv4Header, sizeof(IPV4_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT), false, &buffer->ip_hdr);
    InitTcpHeaderWithAck(tcp, false, &buffer->tcp_hdr);

    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW_TCP));

    InitTcpMss(tcp_opt);
    InitTcpWs(&tcp_opt->ws);
    InitTcpSp(&tcp_opt->sp);

    CalculationTcp4Sum((PBYTE)buffer, sizeof(TCP_OPT));
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeSyn4(IN PBYTE SrcMac, // 6字节长的本地的MAC。
                          IN PBYTE DesMac, IN PIN_ADDR SourceAddress, IN PIN_ADDR DestinationAddress,
                          IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                          IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                          OUT PBYTE buffer    //长度是sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)。
)
{
    PRAW_TCP tcp4 = (PRAW_TCP)buffer;

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV4, &tcp4->eth_hdr);

    InitIpv4Header(SourceAddress, DestinationAddress, IPPROTO_TCP, sizeof(IPV4_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT_MSS), &tcp4->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, sizeof(TCP_OPT_MSS), &tcp4->tcp_hdr);

    TCP_OPT_MSS * mss = (TCP_OPT_MSS *)(buffer + sizeof(RAW_TCP));
    InitTcpMss(mss);

    CalculationTcp4Sum(buffer, sizeof(TCP_OPT_MSS));
}


EXTERN_C
DLLEXPORT
void WINAPI packetize_icmpv4_echo_request(
    IN PBYTE SrcMac, // 6字节长的本地的MAC。
    IN PBYTE DesMac, IN PIN_ADDR SourceAddress, IN PIN_ADDR DestinationAddress,
    OUT PBYTE buffer //长度是sizeof(ETHERNET_HEADER) + sizeof(IPV4_HEADER) + sizeof(ICMP_MESSAGE)
)
{
    // BYTE icmpv4_echo_request[sizeof(ETHERNET_HEADER) + sizeof(IPV4_HEADER) +
    // sizeof(ICMP_MESSAGE)]{};//可以再附加数据。

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV4, (PETHERNET_HEADER)buffer);

    InitIpv4Header(SourceAddress, DestinationAddress, IPPROTO_ICMP, sizeof(IPV4_HEADER) + sizeof(ICMP_MESSAGE), (PIPV4_HEADER)(buffer + sizeof(ETHERNET_HEADER)));

    PICMP_MESSAGE icmp_message = (PICMP_MESSAGE)(buffer + sizeof(ETHERNET_HEADER) + sizeof(IPV4_HEADER));
    icmp_message->Header.Type = 8; // ntohs ICMP6_ECHO_REQUEST;
    icmp_message->Header.Code = 0;
    icmp_message->Header.Checksum = 0;
    icmp_message->icmp6_id = (USHORT)GetCurrentProcessId();
    icmp_message->icmp6_seq = (USHORT)GetTickCount64();
    icmp_message->Header.Checksum = checksum((USHORT *)icmp_message, sizeof(ICMP_MESSAGE));
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI InitIpv6Header(IN PIN6_ADDR SourceAddress, IN PIN6_ADDR DestinationAddress, IN UINT8 NextHeader, IN UINT16 OptLen, OUT PIPV6_HEADER IPv6Header)
{
    IPv6Header->VersionClassFlow = ntohl((6 << 28) | (0 << 20) | 0); // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    IPv6Header->PayloadLength = ntohs(OptLen);
    IPv6Header->NextHeader = NextHeader;//取值，如：IPPROTO_TCP等。
    IPv6Header->HopLimit = 128;
    IPv6Header->SourceAddress = *SourceAddress;
    IPv6Header->DestinationAddress = *DestinationAddress;
    //RtlCopyMemory(&IPv6Header->SourceAddress, SourceAddress, sizeof(IN6_ADDR));
    //RtlCopyMemory(&IPv6Header->DestinationAddress, DestinationAddress, sizeof(IN6_ADDR));
}


void InitIpv6HeaderForTcp(IN PIN6_ADDR SourceAddress, IN PIN6_ADDR DestinationAddress, IN UINT8 NextHeader, IN UINT16 OptLen, OUT PIPV6_HEADER IPv6Header)
/*
功能：组装IPv6协议的TCP头。
*/
{
    IPv6Header->VersionClassFlow = ntohl((6 << 28) | (0 << 20) | 0); // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    IPv6Header->PayloadLength = ntohs(sizeof(TCP_HDR) + OptLen);
    IPv6Header->NextHeader = NextHeader;//取值，如：IPPROTO_TCP等。
    IPv6Header->HopLimit = 128;

    RtlCopyMemory(&IPv6Header->SourceAddress, SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&IPv6Header->DestinationAddress, DestinationAddress, sizeof(IN6_ADDR));
}


void InitIpv6Header(IN PIPV6_HEADER InIPv6Header, IN bool IsCopy, IN UINT16 OptLen, OUT PIPV6_HEADER OutIPv6Header)
/*
功能：把in_ipv6的SYN包里的ipv6信息组装为buffer的要发生的ACK的ipv6。

用途：欺骗（扫描），而不是扫描和攻击。

参数：
IsCopy：是复制还是回复。

注意：如果是回复的包，要把源和目的换一下。
*/
{
    if (IsCopy) {
        InitIpv6HeaderForTcp(&InIPv6Header->SourceAddress, &InIPv6Header->DestinationAddress, InIPv6Header->NextHeader, OptLen, OutIPv6Header);
    } else {
        InitIpv6HeaderForTcp(&InIPv6Header->DestinationAddress, &InIPv6Header->SourceAddress, InIPv6Header->NextHeader, OptLen, OutIPv6Header);
    }
}


void CalculationTcp6Sum(OUT PBYTE buffer, IN int OptLen)
{
    PRAW6_TCP tcp6 = reinterpret_cast<PRAW6_TCP>(buffer);

    PBYTE temp = reinterpret_cast<PBYTE>(MALLOC(sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + OptLen));
    _ASSERTE(temp);

    PSD6_HEADER * PseudoHeader = reinterpret_cast<PSD6_HEADER *>(temp);

    RtlCopyMemory(&PseudoHeader->saddr, &tcp6->ip_hdr.SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&PseudoHeader->daddr, &tcp6->ip_hdr.DestinationAddress, sizeof(IN6_ADDR));
    PseudoHeader->length = ntohl(sizeof(TCP_HDR) + OptLen);
    PseudoHeader->unused1 = 0;
    PseudoHeader->unused2 = 0;
    PseudoHeader->unused3 = 0;
    PseudoHeader->proto = IPPROTO_TCP;

    PBYTE test = &temp[0] + sizeof(PSD6_HEADER);
    RtlCopyMemory(test, &tcp6->tcp_hdr, sizeof(TCP_HDR));

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&tcp6->tcp_hdr + sizeof(TCP_HDR), OptLen);

    tcp6->tcp_hdr.th_sum = checksum(reinterpret_cast<USHORT *>(temp), sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + OptLen);

    FREE(temp);
}


void calculation_icmpv6_echo_request_checksum(OUT PBYTE buffer, IN int OptLen)
/*


数据结构的布局是：sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(ICMP_MESSAGE) + 0x20
*/
{
    UNREFERENCED_PARAMETER(OptLen);

    PIPV6_HEADER ip_hdr = (PIPV6_HEADER)(buffer + sizeof(ETHERNET_HEADER));
    PICMP_MESSAGE icmp_message = (PICMP_MESSAGE)(buffer + sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER));

    BYTE temp[sizeof(PSD6_HEADER) + sizeof(ICMP_MESSAGE) + 0x20]{};

    PSD6_HEADER * PseudoHeader = reinterpret_cast<PSD6_HEADER *>(temp);
    RtlCopyMemory(&PseudoHeader->saddr, &ip_hdr->SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&PseudoHeader->daddr, &ip_hdr->DestinationAddress, sizeof(IN6_ADDR));
    PseudoHeader->length = ntohl(sizeof(ICMP_MESSAGE) + 0x20);
    PseudoHeader->unused1 = 0;
    PseudoHeader->unused2 = 0;
    PseudoHeader->unused3 = 0;
    PseudoHeader->proto = IPPROTO_ICMPV6;

    PBYTE test = temp + sizeof(PSD6_HEADER);
    RtlCopyMemory(test, icmp_message, sizeof(ICMP_MESSAGE) + 0x20);

    icmp_message->Header.Checksum = checksum(reinterpret_cast<USHORT *>(temp), sizeof(temp));
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeAck6(IN PIPV6_HEADER IPv6Header, IN PBYTE SrcMac, IN PBYTE DesMac, OUT PRAW6_TCP buffer)
{
    ASSERT(SrcMac);
    ASSERT(buffer);

    PTCP_HDR tcp = (PTCP_HDR)((PBYTE)IPv6Header + sizeof(IPV6_HEADER));

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV6, &buffer->eth_hdr);
    InitIpv6Header(IPv6Header, false, sizeof(TCP_OPT), &buffer->ip_hdr);
    InitTcpHeaderWithAck(tcp, false, &buffer->tcp_hdr);

    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW6_TCP));

    InitTcpMss(tcp_opt);
    InitTcpWs(&tcp_opt->ws);
    InitTcpSp(&tcp_opt->sp);

    CalculationTcp6Sum((PBYTE)buffer, sizeof(TCP_OPT));
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeSyn6(IN PBYTE SrcMac, IN PBYTE DesMac, IN PIN6_ADDR SourceAddress,
                          IN PIN6_ADDR DestinationAddress,
                          IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                          IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                          OUT PBYTE buffer    //长度是sizeof(RAW6_TCP)。
)
{
    PRAW6_TCP tcp6 = (PRAW6_TCP)buffer;

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV6, &tcp6->eth_hdr);

    InitIpv6HeaderForTcp(SourceAddress, DestinationAddress, IPPROTO_TCP, 0, &tcp6->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, 0, &tcp6->tcp_hdr);

    CalculationTcp6Sum(buffer, 0);
}


EXTERN_C
DLLEXPORT
void WINAPI packetize_icmpv6_echo_request(
    IN PBYTE SrcMac, // 6字节长的本地的MAC。
    IN PBYTE DesMac, IN PIN6_ADDR SourceAddress, IN PIN6_ADDR DestinationAddress,
    OUT PBYTE buffer //长度是sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(ICMP_MESSAGE) + 0x20
)
{
    // BYTE icmpv4_echo_request[sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) +
    // sizeof(ICMP_MESSAGE)]{};//可以再附加数据。

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV6, (PETHERNET_HEADER)buffer);

    InitIpv6Header(SourceAddress, DestinationAddress, IPPROTO_ICMPV6, sizeof(ICMP_MESSAGE) + 0x20, (PIPV6_HEADER)(buffer + sizeof(ETHERNET_HEADER)));

    PICMP_MESSAGE icmp_message = (PICMP_MESSAGE)(buffer + sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER));
    icmp_message->Header.Type = ICMP6_ECHO_REQUEST;
    icmp_message->Header.Code = 0;
    icmp_message->Header.Checksum = 0;
    icmp_message->icmp6_id = (USHORT)GetCurrentProcessId();
    icmp_message->icmp6_seq = (USHORT)GetTickCount64();
    // icmp_message->Header.Checksum =
    calculation_icmpv6_echo_request_checksum(buffer, sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(ICMP_MESSAGE) + 0x20);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
USHORT WINAPI calc_udp4_sum(USHORT * buffer, int size)
/*
功能：获取IPv4下的udp的校验和。

参考：
Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c的ComputeUdpPseudoHeaderChecksumV4。
*/
{
    USHORT sum = 0;
    // PETHERNET_HEADER peh = (PETHERNET_HEADER)buffer;
    PIPV4_HEADER IPv4 = (PIPV4_HEADER)((PBYTE)buffer + ETH_LENGTH_OF_HEADER);
    PUDP_HDR udp = (PUDP_HDR)((PBYTE)IPv4 + Ip4HeaderLengthInBytes(IPv4));

    int len = size - ETH_LENGTH_OF_HEADER;
    len -= Ip4HeaderLengthInBytes(IPv4);
    len -= sizeof(UDP_HDR);

    PSD_HEADER * buf = (PSD_HEADER *)MALLOC(sizeof(PSD_HEADER) + sizeof(UDP_HDR) + len);
    if (!buf) {

        return sum;
    }

    buf->saddr = IPv4->SourceAddress.S_un.S_addr;
    buf->daddr = IPv4->DestinationAddress.S_un.S_addr;
    buf->mbz = 0;
    buf->ptcl = IPPROTO_UDP;
    buf->tcpl = udp->udp_length; // ntohs(sizeof(UDP_HDR) + (u_short)len);

    PUDP_HDR tmp = (PUDP_HDR)((PBYTE)buf + sizeof(PSD_HEADER));
    RtlCopyMemory(tmp, udp, sizeof(UDP_HDR));
    tmp->udp_checksum = 0;

    RtlCopyMemory((PBYTE)buf + sizeof(PSD_HEADER) + sizeof(UDP_HDR), (PBYTE)udp + sizeof(UDP_HDR), len);

    sum = checksum((USHORT *)buf, sizeof(PSD_HEADER) + sizeof(UDP_HDR) + len);

    FREE(buf);

    return sum;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
USHORT WINAPI calc_icmp4_sum(PICMP_HEADER icmp, int size)
/*
功能：计算ICMPv4的校验和。

适用场景：修改了ICMPv4（头部及后面的内容）的情况。
*/
{
    PICMP_HEADER buf = (PICMP_HEADER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    if (!buf) {

        return 0;
    }

    RtlCopyMemory(buf, icmp, size);
    buf->Checksum = 0;

    USHORT sum = checksum((USHORT *)buf, size);

    HeapFree(GetProcessHeap(), 0, buf);

    return sum;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
PVOID WINAPI PacketizeUdp4(PUINT8 SrcMac, PBYTE DesMac, PIN_ADDR SourceAddress, PIN_ADDR DestinationAddress, WORD SourcePort, WORD DestinationPort, PBYTE Data,
                          WORD DataLen)
/*
AI生成的函数：名字是自己起的，参数和代码及注释都是AI生成的，甚至名字都猜到了。人工改进了，有待测试。
功能：构造一个走IPv4的UDP包。
*/
{
    int Length = sizeof(ETHERNET_HEADER) + sizeof(IPV4_HEADER) + sizeof(UDP_HDR) + DataLen;
    PETHERNET_HEADER eth_hdr = (PETHERNET_HEADER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Length);
    if (!eth_hdr) {
        fprintf(stderr, "FILE:%hs, LINE:%d，申请内存失败:%d。\n", __FILE__, __LINE__, Length);
        return eth_hdr;
    }

    memcpy((void *)&eth_hdr->Source, (void *)SrcMac, sizeof(DL_EUI48));
    memcpy((void *)&eth_hdr->Destination, (void *)DesMac, sizeof(DL_EUI48));
    eth_hdr->Type = ntohs(ETHERNET_TYPE_IPV4);

    PIPV4_HEADER ipv4_header = (PIPV4_HEADER)((PBYTE)eth_hdr + ETH_LENGTH_OF_HEADER);
    ipv4_header->VersionAndHeaderLength = 0x45;
    ipv4_header->TypeOfServiceAndEcnField = 0;
    ipv4_header->TotalLength = htons((UINT16)Length - sizeof(ETHERNET_HEADER));
    ipv4_header->Identification = htons((UINT16)rand()); // 最佳做法：ipv4->Identification + 1;
    ipv4_header->FlagsAndOffset = 0;
    ipv4_header->TimeToLive = 64;
    ipv4_header->Protocol = IPPROTO_UDP;
    ipv4_header->SourceAddress.S_un.S_addr = SourceAddress->S_un.S_addr;
    ipv4_header->DestinationAddress.S_un.S_addr = DestinationAddress->S_un.S_addr;
    ipv4_header->HeaderChecksum = 0;
    ipv4_header->HeaderChecksum = checksum((USHORT *)ipv4_header, sizeof(IPV4_HEADER)); // 要不要转换字节序？

    PUDP_HDR udp_hdr = (PUDP_HDR)((PBYTE)ipv4_header + sizeof(IPV4_HEADER));
    PVOID udp_payload = (PUDP_HDR)((PBYTE)udp_hdr + sizeof(UDP_HDR));
    memcpy(udp_payload, Data, DataLen);

    udp_hdr->dst_portno = DestinationPort;
    udp_hdr->src_portno = SourcePort;
    udp_hdr->udp_length = htons(DataLen + sizeof(UDP_HDR));
    udp_hdr->udp_checksum = 0;
    udp_hdr->udp_checksum = calc_udp4_sum((USHORT *)eth_hdr, Length); // 要不要转换字节序？

    return eth_hdr;
}


EXTERN_C
DLLEXPORT
PVOID WINAPI PacketizeUdp6(PUINT8 SrcMac, PBYTE DesMac, PIN6_ADDR SourceAddress, PIN6_ADDR DestinationAddress, WORD SourcePort, WORD DestinationPort, PBYTE Data,
                          WORD DataLen)
/*
AI生成的函数：名字是自己起的，参数和代码及注释都是AI生成的，甚至名字都猜到了。人工改进了，有待测试。
功能：构造一个走IPv6的UDP包。
*/
{
    int Length = sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(UDP_HDR) + DataLen;
    PETHERNET_HEADER eth_hdr = (PETHERNET_HEADER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Length);
    if (!eth_hdr) {
        fprintf(stderr, "FILE:%hs, LINE:%d，申请内存失败:%d。\n", __FILE__, __LINE__, Length);
        return eth_hdr;
    }

    memcpy((void *)&eth_hdr->Source, (void *)SrcMac, sizeof(DL_EUI48));
    memcpy((void *)&eth_hdr->Destination, (void *)DesMac, sizeof(DL_EUI48));
    eth_hdr->Type = ntohs(ETHERNET_TYPE_IPV6);

    PIPV6_HEADER ipv6_hdr = (PIPV6_HEADER)((PBYTE)eth_hdr + ETH_LENGTH_OF_HEADER);

    // 99.9% 的情况下，直接写死 0x60000000 就完事了，连随机 Flow Label 都不需要，除非你做反检测、扫描器之类的高级用途。
    ipv6_hdr->VersionClassFlow = htonl(0x60000000); // 高优先级（WebRTC、游戏加速）htonl(0x60B80000UL) // DSCP=46

    ipv6_hdr->PayloadLength = htons((UINT16)(sizeof(UDP_HDR) + DataLen));
    ipv6_hdr->NextHeader = IPPROTO_UDP;
    ipv6_hdr->HopLimit = 64;
    ipv6_hdr->SourceAddress = *SourceAddress;
    ipv6_hdr->DestinationAddress = *DestinationAddress;

    PUDP_HDR udp_hdr = (PUDP_HDR)((PBYTE)ipv6_hdr + sizeof(IPV6_HEADER));
    PVOID udp_payload = (PUDP_HDR)((PBYTE)udp_hdr + sizeof(UDP_HDR));
    memcpy(udp_payload, Data, DataLen);

    udp_hdr->dst_portno = DestinationPort;
    udp_hdr->src_portno = SourcePort;
    udp_hdr->udp_length = htons(DataLen + sizeof(UDP_HDR));
    udp_hdr->udp_checksum = 0;
    udp_hdr->udp_checksum = calc_udp4_sum((USHORT *)eth_hdr, Length); // 要不要转换字节序？

    return eth_hdr;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
