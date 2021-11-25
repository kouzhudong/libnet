#include "pch.h"
#include "raw.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void InitEthernetHeader(IN PBYTE SrcMac, IN UINT16 Type, OUT PETHERNET_HEADER eth_hdr)
/*
功能：填写以太头。

参数：
Type，取值，如：ETHERNET_TYPE_IPV4，ETHERNET_TYPE_IPV6， ETHERNET_TYPE_ARP等。

注释：填写虚假的目的MAC，也可发送出去。
*/
{
    eth_hdr->Destination.Byte[0] = 0x99;
    eth_hdr->Destination.Byte[1] = 0x99;
    eth_hdr->Destination.Byte[2] = 0x99;
    eth_hdr->Destination.Byte[3] = 0x99;
    eth_hdr->Destination.Byte[4] = 0x99;
    eth_hdr->Destination.Byte[5] = 0x99;

    eth_hdr->Source.Byte[0] = SrcMac[0];
    eth_hdr->Source.Byte[1] = SrcMac[1];
    eth_hdr->Source.Byte[2] = SrcMac[2];
    eth_hdr->Source.Byte[3] = SrcMac[3];
    eth_hdr->Source.Byte[4] = SrcMac[4];
    eth_hdr->Source.Byte[5] = SrcMac[5];

    eth_hdr->Type = ntohs(Type);
}


void InitIpv4Header(IN PIN_ADDR SourceAddress,
                    IN PIN_ADDR DestinationAddress,
                    IN UINT8 Protocol, //取值，如：IPPROTO_TCP等。
                    IN UINT16 TotalLength,//严格计算数据的大小。
                    OUT PIPV4_HEADER IPv4Header
)
/*
功能：组装IPv4头。
*/
{
    _ASSERTE(TotalLength >= sizeof(IPV4_HEADER) + sizeof(TCP_HDR));

    IPv4Header->VersionAndHeaderLength = (4 << 4) | (sizeof(IPV4_HEADER) / sizeof(unsigned long));
    IPv4Header->TotalLength = ntohs(TotalLength);
    IPv4Header->Identification = ntohs(0);
    IPv4Header->DontFragment = TRUE;
    IPv4Header->TimeToLive = 128;
    IPv4Header->Protocol = Protocol;
    IPv4Header->SourceAddress.S_un.S_addr = SourceAddress->S_un.S_addr;
    IPv4Header->DestinationAddress.S_un.S_addr = DestinationAddress->S_un.S_addr;
    IPv4Header->HeaderChecksum = checksum((unsigned short *)IPv4Header, sizeof(IPV4_HEADER));
}


void InitIpv4Header(IN PIPV4_HEADER InIPv4Header,
                    IN UINT16 TotalLength,
                    IN bool IsCopy,
                    OUT PIPV4_HEADER OutIPv4Header
)
/*
功能：把in_ipv4的SYN包里的ipv4信息组装为buffer的要发生的ACK的ipv4。

用途：欺骗（扫描），而不是扫描和攻击。

参数：
IsCopy：是复制还是回复。

注意：如果是回复的包，要把源和目的换一下。
*/
{
    if (IsCopy) {
        InitIpv4Header(&InIPv4Header->SourceAddress,
                       &InIPv4Header->DestinationAddress,
                       InIPv4Header->Protocol,
                       TotalLength,
                       OutIPv4Header);
    } else {
        InitIpv4Header(&InIPv4Header->DestinationAddress,
                       &InIPv4Header->SourceAddress,
                       InIPv4Header->Protocol,
                       TotalLength,
                       OutIPv4Header);
    }
}


void InitTcpHeader(IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                   IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                   IN SEQ_NUM th_ack,  //网络序。如果是主机序，请用htonl转换下。
                   IN UINT8 th_flags,  //TH_ACK, TH_SYN等值的组合。
                   IN UINT8 OptLen,
                   OUT PTCP_HDR tcp_hdr
)
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
    ASSERT(x <= 0xf);//大于这个数会发生溢出，有想不到的结果。    
    tcp_hdr->th_len = x;

    tcp_hdr->th_flags = th_flags;
    tcp_hdr->th_win = ntohs(65535);

    tcp_hdr->th_sum = 0;
    tcp_hdr->th_urp = 0;
    tcp_hdr->th_sum = 0;
}


void InitTcpHeaderBySyn(IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                        IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                        IN UINT8 OptLen,
                        OUT PTCP_HDR tcp_hdr
)
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
                      tcp->th_seq + 1,//收到的号加一。
                      TH_ACK | TH_SYN,
                      0,
                      tcp_hdr);
    } else {
        InitTcpHeader(tcp->th_dport,
                      tcp->th_sport,
                      tcp->th_seq + 1,//收到的号加一。
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

参数：
OptLen，是tcp的扩展选项（TCP_OPT）或者额外附带的数据（如http的html等)，
        不包括ETHERNET_HEADER，IPV4_HEADER，TCP_HDR。
*/
{
    PRAW_TCP tcp4 = (PRAW_TCP)buffer;

    PBYTE temp = (PBYTE)HeapAlloc(GetProcessHeap(),
                                  HEAP_ZERO_MEMORY,
                                  sizeof(PSD_HEADER) + sizeof(TCP_HDR) + OptLen);
    _ASSERTE(temp);

    PSD_HEADER * PseudoHeader = (PSD_HEADER *)temp;

    PseudoHeader->saddr = tcp4->ip_hdr.SourceAddress.S_un.S_addr;
    PseudoHeader->daddr = tcp4->ip_hdr.DestinationAddress.S_un.S_addr;
    PseudoHeader->mbz = 0;
    PseudoHeader->ptcl = IPPROTO_TCP;
    PseudoHeader->tcpl = ntohs(sizeof(TCP_HDR) + OptLen);

    PBYTE test = &temp[0] + sizeof(PSD_HEADER);
    RtlCopyMemory(test, &tcp4->tcp_hdr, sizeof(TCP_HDR));

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&tcp4->tcp_hdr + sizeof(TCP_HDR), OptLen);

    tcp4->tcp_hdr.th_sum = checksum((USHORT *)temp, sizeof(PSD_HEADER) + sizeof(TCP_HDR) + OptLen);

    HeapFree(GetProcessHeap(), 0, temp);
}


EXTERN_C
__declspec(dllexport)
void WINAPI PacketizeAck4(IN PIPV4_HEADER IPv4Header, IN PBYTE SrcMac, OUT PRAW_TCP buffer)
{
    ASSERT(SrcMac);
    ASSERT(buffer);

    PTCP_HDR tcp = (PTCP_HDR)((PBYTE)IPv4Header + Ip4HeaderLengthInBytes(IPv4Header));

    InitEthernetHeader(SrcMac, ETHERNET_TYPE_IPV4, &buffer->eth_hdr);
    InitIpv4Header(IPv4Header,
                   sizeof(IPV4_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT),
                   false,
                   &buffer->ip_hdr);
    InitTcpHeaderWithAck(tcp, false, &buffer->tcp_hdr);

    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW_TCP));

    InitTcpMss(tcp_opt);
    InitTcpWs(&tcp_opt->ws);
    InitTcpSp(&tcp_opt->sp);

    CalculationTcp4Sum((PBYTE)buffer, sizeof(TCP_OPT));
}


EXTERN_C
__declspec(dllexport)
void WINAPI PacketizeSyn4(IN PBYTE SrcMac,    //6字节长的本地的MAC。
                          IN PIN_ADDR SourceAddress,
                          IN PIN_ADDR DestinationAddress,
                          IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                          IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                          OUT PBYTE buffer    //长度是sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)。
)
{
    PRAW_TCP tcp4 = (PRAW_TCP)buffer;

    InitEthernetHeader(SrcMac, ETHERNET_TYPE_IPV4, &tcp4->eth_hdr);

    InitIpv4Header(SourceAddress,
                   DestinationAddress,
                   IPPROTO_TCP,
                   sizeof(IPV4_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT_MSS),
                   &tcp4->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, sizeof(TCP_OPT_MSS), &tcp4->tcp_hdr);

    TCP_OPT_MSS * mss = (TCP_OPT_MSS *)(buffer + sizeof(RAW_TCP));
    InitTcpMss(mss);

    CalculationTcp4Sum(buffer, sizeof(TCP_OPT_MSS));
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void InitIpv6Header(IN PIN6_ADDR SourceAddress,
                    IN PIN6_ADDR DestinationAddress,
                    IN UINT8 NextHeader, //取值，如：IPPROTO_TCP等。
                    OUT PIPV6_HEADER IPv6Header
)
/*
功能：组装IPv6协议的TCP头。
*/
{
    IPv6Header->VersionClassFlow = ntohl((6 << 28) | (0 << 20) | 0);// IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    IPv6Header->PayloadLength = ntohs(sizeof(TCP_HDR) + sizeof(TCP_OPT));
    IPv6Header->NextHeader = NextHeader;
    IPv6Header->HopLimit = 128;

    RtlCopyMemory(&IPv6Header->SourceAddress, SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&IPv6Header->DestinationAddress, DestinationAddress, sizeof(IN6_ADDR));
}


void InitIpv6Header(IN PIPV6_HEADER InIPv6Header, IN bool IsCopy, OUT PIPV6_HEADER OutIPv6Header)
/*
功能：把in_ipv6的SYN包里的ipv6信息组装为buffer的要发生的ACK的ipv6。

用途：欺骗（扫描），而不是扫描和攻击。

参数：
IsCopy：是复制还是回复。

注意：如果是回复的包，要把源和目的换一下。
*/
{
    if (IsCopy) {
        InitIpv6Header(&InIPv6Header->SourceAddress,
                       &InIPv6Header->DestinationAddress,
                       InIPv6Header->NextHeader,
                       OutIPv6Header);
    } else {
        InitIpv6Header(&InIPv6Header->DestinationAddress,
                       &InIPv6Header->SourceAddress,
                       InIPv6Header->NextHeader,
                       OutIPv6Header);
    }
}


void CalculationTcp6Sum(OUT PBYTE buffer, IN int OptLen)
{
    PRAW6_TCP tcp6 = (PRAW6_TCP)buffer;

    PBYTE temp = (PBYTE)HeapAlloc(GetProcessHeap(),
                                  HEAP_ZERO_MEMORY,
                                  sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + OptLen);
    _ASSERTE(temp);

    PSD6_HEADER * PseudoHeader = (PSD6_HEADER *)temp;

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

    tcp6->tcp_hdr.th_sum = checksum((USHORT *)temp, sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + OptLen);

    HeapFree(GetProcessHeap(), 0, temp);
}


EXTERN_C
__declspec(dllexport)
void WINAPI PacketizeAck6(IN PIPV6_HEADER IPv6Header, IN PBYTE SrcMac, OUT PRAW6_TCP buffer)
{
    ASSERT(SrcMac);
    ASSERT(buffer);

    PTCP_HDR tcp = (PTCP_HDR)((PBYTE)IPv6Header + sizeof(IPV6_HEADER));

    InitEthernetHeader(SrcMac, ETHERNET_TYPE_IPV6, &buffer->eth_hdr);
    InitIpv6Header(IPv6Header, false, &buffer->ip_hdr);
    InitTcpHeaderWithAck(tcp, false, &buffer->tcp_hdr);

    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW6_TCP));

    InitTcpMss(tcp_opt);
    InitTcpWs(&tcp_opt->ws);
    InitTcpSp(&tcp_opt->sp);

    CalculationTcp6Sum((PBYTE)buffer, sizeof(TCP_OPT));
}


EXTERN_C
__declspec(dllexport)
void WINAPI PacketizeSyn6(IN PBYTE SrcMac,
                          IN PIN6_ADDR SourceAddress,
                          IN PIN6_ADDR DestinationAddress,
                          IN UINT16 th_sport, //网络序。如果是主机序，请用htons转换下。
                          IN UINT16 th_dport, //网络序。如果是主机序，请用htons转换下。
                          OUT PBYTE buffer    //长度是sizeof(RAW6_TCP) + sizeof(TCP_OPT_MSS)。
)
{
    PRAW6_TCP tcp6 = (PRAW6_TCP)buffer;

    InitEthernetHeader(SrcMac, ETHERNET_TYPE_IPV6, &tcp6->eth_hdr);

    InitIpv6Header(SourceAddress, DestinationAddress, IPPROTO_TCP, &tcp6->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, 0, &tcp6->tcp_hdr);

    CalculationTcp6Sum(buffer, 0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
