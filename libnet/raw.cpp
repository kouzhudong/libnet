#include "pch.h"
#include "raw.h"
#include "udp.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
// 计算校验和相关的函数。


EXTERN_C
DLLEXPORT
USHORT WINAPI checksum(USHORT * buffer, int size)
// 摘自Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c
//  Description:This function calculates the 16-bit one's complement sum for the supplied buffer.
{
    unsigned long cksum = 0;

    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }

    if (size) { // If the buffer was not a multiple of 16-bits, add the last byte
        cksum += *(UCHAR *)buffer;
    }

    // Add the low order 16-bits to the high order 16-bits
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    return (USHORT)(~cksum); // Take the 1's complement
}


static BOOL CalculationTcp4Sum(OUT PBYTE buffer, WORD OptLen)
/*
功能：计算并设置tcp的校验和。

参数：OptLen，是tcp的扩展选项（TCP_OPT）或者额外附带的数据（如http的html等)， 不包括ETHERNET_HEADER，IPV4_HEADER，TCP_HDR。

返回：成功写入校验和返回TRUE；内存申请失败返回FALSE（此时校验和保持为0）。
*/
{
    PRAW_TCP tcp4 = reinterpret_cast<PRAW_TCP>(buffer);

    PBYTE temp = reinterpret_cast<PBYTE>(MALLOC(sizeof(PSD_HEADER) + sizeof(TCP_HDR) + OptLen));
    if (!temp) {
        return FALSE;
    }

    PSD_HEADER * PseudoHeader = reinterpret_cast<PSD_HEADER *>(temp);

    PseudoHeader->saddr = tcp4->ip_hdr.SourceAddress.S_un.S_addr;
    PseudoHeader->daddr = tcp4->ip_hdr.DestinationAddress.S_un.S_addr;
    PseudoHeader->mbz = 0;
    PseudoHeader->ptcl = IPPROTO_TCP;
    PseudoHeader->tcpl = ntohs(sizeof(TCP_HDR) + OptLen);

    PBYTE test = &temp[0] + sizeof(PSD_HEADER);
    RtlCopyMemory(test, &tcp4->tcp_hdr, sizeof(TCP_HDR));
    reinterpret_cast<PTCP_HDR>(test)->th_sum = 0;

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&tcp4->tcp_hdr + sizeof(TCP_HDR), OptLen);

    tcp4->tcp_hdr.th_sum = checksum(reinterpret_cast<USHORT *>(temp), sizeof(PSD_HEADER) + sizeof(TCP_HDR) + OptLen);

    FREE(temp);

    return TRUE;
}


static BOOL CalculationTcp6Sum(OUT PBYTE buffer, IN int OptLen)
{
    PRAW6_TCP tcp6 = reinterpret_cast<PRAW6_TCP>(buffer);

    PBYTE temp = reinterpret_cast<PBYTE>(MALLOC(sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + OptLen));
    if (!temp) {
        return FALSE;
    }

    PSD6_HEADER * PseudoHeader = reinterpret_cast<PSD6_HEADER *>(temp);

    RtlCopyMemory(&PseudoHeader->saddr, &tcp6->ip_hdr.SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&PseudoHeader->daddr, &tcp6->ip_hdr.DestinationAddress, sizeof(IN6_ADDR));
    PseudoHeader->length = htonl(sizeof(TCP_HDR) + OptLen);
    PseudoHeader->unused1 = 0;
    PseudoHeader->unused2 = 0;
    PseudoHeader->unused3 = 0;
    PseudoHeader->proto = IPPROTO_TCP;

    PBYTE test = &temp[0] + sizeof(PSD6_HEADER);
    RtlCopyMemory(test, &tcp6->tcp_hdr, sizeof(TCP_HDR));
    reinterpret_cast<PTCP_HDR>(test)->th_sum = 0;

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&tcp6->tcp_hdr + sizeof(TCP_HDR), OptLen);

    tcp6->tcp_hdr.th_sum = checksum(reinterpret_cast<USHORT *>(temp), sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + OptLen);

    FREE(temp);

    return TRUE;
}


EXTERN_C
DLLEXPORT
BOOL WINAPI calculation_icmpv6_echo_request_checksum(OUT PBYTE buffer, IN int OptLen)
/*
OptLen：整个包的字节数 = sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + ICMPv6载荷长度。

返回：成功写入校验和返回TRUE；参数非法或内存申请失败返回FALSE（此时未写入校验和）。
*/
{
    if (!buffer) {
        return FALSE;
    }

    int icmpv6_len = OptLen - (int)(sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER));
    if (icmpv6_len <= 0) {
        return FALSE;
    }

    PIPV6_HEADER ip_hdr = (PIPV6_HEADER)(buffer + sizeof(ETHERNET_HEADER));
    PICMP_MESSAGE icmp_message = (PICMP_MESSAGE)(buffer + sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER));

    PBYTE temp = reinterpret_cast<PBYTE>(MALLOC(sizeof(PSD6_HEADER) + icmpv6_len));
    if (!temp) {
        return FALSE;
    }

    PSD6_HEADER * PseudoHeader = reinterpret_cast<PSD6_HEADER *>(temp);
    RtlCopyMemory(&PseudoHeader->saddr, &ip_hdr->SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&PseudoHeader->daddr, &ip_hdr->DestinationAddress, sizeof(IN6_ADDR));
    PseudoHeader->length = htonl((ULONG)icmpv6_len);
    PseudoHeader->unused1 = 0;
    PseudoHeader->unused2 = 0;
    PseudoHeader->unused3 = 0;
    PseudoHeader->proto = IPPROTO_ICMPV6;

    PBYTE test = temp + sizeof(PSD6_HEADER);
    RtlCopyMemory(test, icmp_message, icmpv6_len);
    reinterpret_cast<PICMP_MESSAGE>(test)->Header.Checksum = 0;

    icmp_message->Header.Checksum = checksum(reinterpret_cast<USHORT *>(temp), sizeof(PSD6_HEADER) + icmpv6_len);

    FREE(temp);

    return TRUE;
}


EXTERN_C
DLLEXPORT
USHORT WINAPI calc_udp4_sum(USHORT * buffer, int size)
/*
功能：获取IPv4下的udp的校验和。
参考：Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c的ComputeUdpPseudoHeaderChecksumV4。
*/
{
    USHORT sum = 0;

    // 校验：至少要容纳 以太头 + 最小IPv4头 + UDP头，否则下面读取IHL/拷贝会越界。
    if (!buffer || size < (int)(ETH_LENGTH_OF_HEADER + sizeof(IPV4_HEADER) + sizeof(UDP_HDR))) {
        return sum;
    }

    // PETHERNET_HEADER peh = (PETHERNET_HEADER)buffer;
    PIPV4_HEADER IPv4 = (PIPV4_HEADER)((PBYTE)buffer + ETH_LENGTH_OF_HEADER);
    PUDP_HDR udp = (PUDP_HDR)((PBYTE)IPv4 + Ip4HeaderLengthInBytes(IPv4));

    int len = size - ETH_LENGTH_OF_HEADER;
    len -= Ip4HeaderLengthInBytes(IPv4);
    len -= sizeof(UDP_HDR);
    if (len < 0) { // IHL含选项时头更长，防止负长度导致MALLOC回绕 + RtlCopyMemory巨量拷贝。
        return sum;
    }

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
    if (sum == 0) {
        sum = 0xFFFF; // RFC 768：UDP校验和计算为0时须以0xFFFF发送（0表示未校验；IPv6下0非法）。
    }

    FREE(buf);

    return sum;
}


EXTERN_C
DLLEXPORT
USHORT WINAPI calc_udp6_sum(USHORT * buffer, int size)
/*
功能：获取IPv6下的udp的校验和。
参考：Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c的ComputeUdpPseudoHeaderChecksumV6。
*/
{
    USHORT sum = 0;

    // 校验：至少要容纳 以太头 + IPv6头 + UDP头，否则len为负会导致MALLOC回绕 + RtlCopyMemory巨量拷贝。
    if (!buffer || size < (int)(ETH_LENGTH_OF_HEADER + sizeof(IPV6_HEADER) + sizeof(UDP_HDR))) {
        return sum;
    }

    // PETHERNET_HEADER peh = (PETHERNET_HEADER)buffer;
    PIPV6_HEADER ipv6_hdr = (PIPV6_HEADER)((PBYTE)buffer + ETH_LENGTH_OF_HEADER);
    PUDP_HDR udp_hdr = (PUDP_HDR)((PBYTE)ipv6_hdr + sizeof(IPV6_HEADER));

    int len = size - ETH_LENGTH_OF_HEADER;
    len -= sizeof(IPV6_HEADER);
    len -= sizeof(UDP_HDR);

    PSD6_HEADER * buf = (PSD6_HEADER *)MALLOC(sizeof(PSD6_HEADER) + sizeof(UDP_HDR) + len);
    if (!buf) {

        return sum;
    }

    RtlCopyMemory(&buf->saddr, &ipv6_hdr->SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&buf->daddr, &ipv6_hdr->DestinationAddress, sizeof(IN6_ADDR));
    buf->length = udp_hdr->udp_length;
    buf->unused1 = 0;
    buf->unused2 = 0;
    buf->unused3 = 0;
    buf->proto = IPPROTO_UDP;

    PUDP_HDR tmp = (PUDP_HDR)((PBYTE)buf + sizeof(PSD6_HEADER));
    RtlCopyMemory(tmp, udp_hdr, sizeof(UDP_HDR));
    tmp->udp_checksum = 0;

    RtlCopyMemory((PBYTE)buf + sizeof(PSD6_HEADER) + sizeof(UDP_HDR), (PBYTE)udp_hdr + sizeof(UDP_HDR), len);

    sum = checksum((USHORT *)buf, sizeof(PSD6_HEADER) + sizeof(UDP_HDR) + len);
    if (sum == 0) {
        sum = 0xFFFF; // RFC 768/RFC 8200：UDP校验和计算为0时须以0xFFFF发送（IPv6下校验和0非法，接收方必丢弃）。
    }

    FREE(buf);

    return sum;
}


EXTERN_C
DLLEXPORT
USHORT WINAPI calc_icmp4_sum(PICMP_HEADER icmp, int size)
/*
功能：计算ICMPv4的校验和。

适用场景：修改了ICMPv4（头部及后面的内容）的情况。
*/
{
    if (!icmp || size < (int)sizeof(ICMP_HEADER)) { // 防止NULL解引用，以及size过小时 buf->Checksum=0 越界写。
        return 0;
    }

    PICMP_HEADER buf = (PICMP_HEADER)MALLOC(size);
    if (!buf) {

        return 0;
    }

    RtlCopyMemory(buf, icmp, size);
    buf->Checksum = 0;

    USHORT sum = checksum((USHORT *)buf, size);

    FREE(buf);

    return sum;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI InitEthernetHeader(IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, IN UINT16 Type, OUT PETHERNET_HEADER eth_hdr)
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
    if (!SrcMac || !DesMac || !eth_hdr) {
        return;
    }

    // eth_hdr->Destination.Byte[0] = DesMac[0];
    // eth_hdr->Destination.Byte[1] = DesMac[1];
    // eth_hdr->Destination.Byte[2] = DesMac[2];
    // eth_hdr->Destination.Byte[3] = DesMac[3];
    // eth_hdr->Destination.Byte[4] = DesMac[4];
    // eth_hdr->Destination.Byte[5] = DesMac[5];
    eth_hdr->Destination = *DesMac;

    // eth_hdr->Source.Byte[0] = SrcMac[0];
    // eth_hdr->Source.Byte[1] = SrcMac[1];
    // eth_hdr->Source.Byte[2] = SrcMac[2];
    // eth_hdr->Source.Byte[3] = SrcMac[3];
    // eth_hdr->Source.Byte[4] = SrcMac[4];
    // eth_hdr->Source.Byte[5] = SrcMac[5];
    eth_hdr->Source = *SrcMac;

    eth_hdr->Type = ntohs(Type);
}


EXTERN_C
DLLEXPORT
void WINAPI InitIpv4Header(IN PIN_ADDR SourceAddress, IN PIN_ADDR DestinationAddress, IN UINT8 Protocol, IN UINT16 TotalLength, OUT PIPV4_HEADER IPv4Header)
/*
功能：组装IPv4头。

TotalLength 严格计算数据的大小。
*/
{
    if (!SourceAddress || !DestinationAddress || !IPv4Header) {
        return;
    }

    // 先整体清零：TypeOfService(off1)、FlagsAndOffset(仅DontFragment是其中1个bit)、HeaderChecksum(off10)
    // 下面都不会逐一赋值；若调用方缓冲区未清零，残留值会导致头校验和错误、并可能被接收方当成IP分片。
    RtlZeroMemory(IPv4Header, sizeof(IPV4_HEADER));

    IPv4Header->VersionAndHeaderLength = (4 << 4) | (sizeof(IPV4_HEADER) / sizeof(unsigned long));
    IPv4Header->TotalLength = ntohs(TotalLength);
    IPv4Header->Identification = htons((UINT16)rand()); // 最佳做法：ipv4->Identification + 1; 不建议：ntohs(0);
    IPv4Header->DontFragment = TRUE;
    IPv4Header->TimeToLive = 128;
    IPv4Header->Protocol = Protocol; // 取值，如：IPPROTO_TCP等。
    IPv4Header->SourceAddress.S_un.S_addr = SourceAddress->S_un.S_addr;
    IPv4Header->DestinationAddress.S_un.S_addr = DestinationAddress->S_un.S_addr;
    IPv4Header->HeaderChecksum = checksum(reinterpret_cast<unsigned short *>(IPv4Header), sizeof(IPV4_HEADER));
}


static void InitIpv4Header(IN PIPV4_HEADER InIPv4Header, IN UINT16 TotalLength, IN bool IsCopy, OUT PIPV4_HEADER OutIPv4Header)
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


static void InitTcpHeader(IN UINT16 th_sport, IN UINT16 th_dport, IN SEQ_NUM th_ack, IN UINT8 th_flags, IN UINT8 OptLen, OUT PTCP_HDR tcp_hdr)
/*
功能：组装TCP头（总共十个成员）。

参数：
th_sport：源端口。网络序。如果是主机序，请用htons转换下。
th_dport：目的端口。网络序。如果是主机序，请用htons转换下。
th_ack：确认号。网络序。如果是主机序，请用htonl转换下。
th_flags：TH_ACK, TH_SYN等值的组合。

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
    if (x > 0xf) {
        return;
    }
    tcp_hdr->th_len = x;

    tcp_hdr->th_flags = th_flags;
    tcp_hdr->th_win = ntohs(65535);

    tcp_hdr->th_sum = 0;
    tcp_hdr->th_urp = 0;
}


static void InitTcpHeaderBySyn(IN UINT16 th_sport, IN UINT16 th_dport, IN UINT8 OptLen, OUT PTCP_HDR tcp_hdr)
/*
th_sport, //网络序。如果是主机序，请用htons转换下。
th_dport, //网络序。如果是主机序，请用htons转换下。
*/
{
    InitTcpHeader(th_sport, th_dport, 0, TH_SYN, OptLen, tcp_hdr);
}


static void InitTcpHeaderWithAck(IN PTCP_HDR tcp, IN bool IsCopy, IN UINT8 OptLen, OUT PTCP_HDR tcp_hdr)
/*
用途：欺骗（扫描），而不是扫描和攻击。

OptLen：TCP头之后追加的选项（TCP_OPT）字节数，必须计入th_len数据偏移，
否则接收方会按th_len=5(20字节)解析，把选项当成段数据，MSS/窗口缩放/SACK等选项不会被协商。
*/
{
    if (IsCopy) {
        InitTcpHeader(tcp->th_sport, tcp->th_dport, htonl(ntohl(tcp->th_seq) + 1), TH_ACK | TH_SYN, OptLen, tcp_hdr);
    } else {
        InitTcpHeader(tcp->th_dport, tcp->th_sport, htonl(ntohl(tcp->th_seq) + 1), TH_ACK | TH_SYN, OptLen, tcp_hdr);
    }
}


static void InitTcpMss(OUT TCP_OPT_MSS * mss)
{
    mss->Kind = TH_OPT_MSS;
    mss->Length = 4;
    mss->Mss = ntohs(1460);
}


static void InitTcpMss(OUT PTCP_OPT tcp_opt)
{
    InitTcpMss(&tcp_opt->mss);

    tcp_opt->unuse1 = TH_OPT_NOP;
    tcp_opt->unuse2 = TH_OPT_NOP << 8 | TH_OPT_NOP;
}


static void InitTcpWs(OUT TCP_OPT_WS * ws)
{
    ws->Kind = TH_OPT_WS;
    ws->Length = 3;
    ws->ShiftCnt = 8;
}


static void InitTcpSp(OUT TCP_OPT_SACK_PERMITTED * sp)
{
    sp->Kind = TH_OPT_SACK_PERMITTED;
    sp->Length = 2;
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeAck4(IN PIPV4_HEADER IPv4Header, IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, OUT PRAW_TCP buffer)
{
    if (!IPv4Header || !SrcMac || !DesMac || !buffer) {
        return;
    }

    PTCP_HDR tcp = (PTCP_HDR)((PBYTE)IPv4Header + Ip4HeaderLengthInBytes(IPv4Header));

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV4, &buffer->eth_hdr);
    // ip_hdr 是紧凑布局网络帧内的成员（紧随 14 字节以太网头），取址必然“未对齐”；x86/x64 允许非对齐访问，安全。
#pragma warning(suppress : 4366)
    InitIpv4Header(IPv4Header, sizeof(IPV4_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT), false, &buffer->ip_hdr);
    InitTcpHeaderWithAck(tcp, false, sizeof(TCP_OPT), &buffer->tcp_hdr);

    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW_TCP));

    InitTcpMss(tcp_opt);
    InitTcpWs(&tcp_opt->ws);
    InitTcpSp(&tcp_opt->sp);

    CalculationTcp4Sum((PBYTE)buffer, sizeof(TCP_OPT));
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeSyn4(IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, IN PIN_ADDR SourceAddress, IN PIN_ADDR DestinationAddress, IN UINT16 th_sport, IN UINT16 th_dport,
                          OUT PBYTE buffer)
/*
th_sport：网络序。如果是主机序，请用htons转换下。
th_dport：网络序。如果是主机序，请用htons转换下。
buffer：长度是sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)。
*/
{
    if (!SrcMac || !DesMac || !SourceAddress || !DestinationAddress || !buffer) {
        return;
    }

    PRAW_TCP tcp4 = (PRAW_TCP)buffer;

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV4, &tcp4->eth_hdr);

    // ip_hdr 是紧凑布局网络帧内的成员（紧随 14 字节以太网头），取址必然“未对齐”；x86/x64 允许非对齐访问，安全。
#pragma warning(suppress : 4366)
    InitIpv4Header(SourceAddress, DestinationAddress, IPPROTO_TCP, sizeof(IPV4_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT_MSS), &tcp4->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, sizeof(TCP_OPT_MSS), &tcp4->tcp_hdr);

    TCP_OPT_MSS * mss = (TCP_OPT_MSS *)(buffer + sizeof(RAW_TCP));
    InitTcpMss(mss);

    CalculationTcp4Sum(buffer, sizeof(TCP_OPT_MSS));
}


EXTERN_C
DLLEXPORT
void WINAPI packetize_icmpv4_echo_request(IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, IN PIN_ADDR SourceAddress, IN PIN_ADDR DestinationAddress, OUT PBYTE buffer)
/*
buffer：长度是sizeof(ETHERNET_HEADER) + sizeof(IPV4_HEADER) + sizeof(ICMP_MESSAGE)
*/
{
    if (!SrcMac || !DesMac || !SourceAddress || !DestinationAddress || !buffer) {
        return;
    }

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
    // 99.9% 的情况下，直接写死 0x60000000 就完事了，连随机 Flow Label 都不需要，除非你做反检测、扫描器之类的高级用途。
    // IPv6Header->VersionClassFlow = htonl(0x60000000); // 高优先级（WebRTC、游戏加速）htonl(0x60B80000UL) // DSCP=46

    IPv6Header->VersionClassFlow = ntohl((6 << 28) | (0 << 20) | 0); // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    IPv6Header->PayloadLength = ntohs(OptLen);
    IPv6Header->NextHeader = NextHeader; // 取值，如：IPPROTO_TCP等。
    IPv6Header->HopLimit = 128;
    IPv6Header->SourceAddress = *SourceAddress;
    IPv6Header->DestinationAddress = *DestinationAddress;
    // RtlCopyMemory(&IPv6Header->SourceAddress, SourceAddress, sizeof(IN6_ADDR));
    // RtlCopyMemory(&IPv6Header->DestinationAddress, DestinationAddress, sizeof(IN6_ADDR));
}


static void InitIpv6HeaderForTcp(IN PIN6_ADDR SourceAddress, IN PIN6_ADDR DestinationAddress, IN UINT8 NextHeader, IN UINT16 OptLen, OUT PIPV6_HEADER IPv6Header)
/*
功能：组装IPv6协议的TCP头。
*/
{
    IPv6Header->VersionClassFlow = ntohl((6 << 28) | (0 << 20) | 0); // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    IPv6Header->PayloadLength = ntohs(sizeof(TCP_HDR) + OptLen);
    IPv6Header->NextHeader = NextHeader; // 取值，如：IPPROTO_TCP等。
    IPv6Header->HopLimit = 128;

    RtlCopyMemory(&IPv6Header->SourceAddress, SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&IPv6Header->DestinationAddress, DestinationAddress, sizeof(IN6_ADDR));
}


static void InitIpv6Header(IN PIPV6_HEADER InIPv6Header, IN bool IsCopy, IN UINT16 OptLen, OUT PIPV6_HEADER OutIPv6Header)
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


EXTERN_C
DLLEXPORT
void WINAPI PacketizeAck6(IN PIPV6_HEADER IPv6Header, IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, OUT PRAW6_TCP buffer)
{
    if (!IPv6Header || !SrcMac || !DesMac || !buffer) {
        return;
    }

    PTCP_HDR tcp = (PTCP_HDR)((PBYTE)IPv6Header + sizeof(IPV6_HEADER));

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV6, &buffer->eth_hdr);
    // ip_hdr 是紧凑布局网络帧内的成员（紧随 14 字节以太网头），取址必然“未对齐”；x86/x64 允许非对齐访问，安全。
#pragma warning(suppress : 4366)
    InitIpv6Header(IPv6Header, false, sizeof(TCP_OPT), &buffer->ip_hdr);
    InitTcpHeaderWithAck(tcp, false, sizeof(TCP_OPT), &buffer->tcp_hdr);

    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW6_TCP));

    InitTcpMss(tcp_opt);
    InitTcpWs(&tcp_opt->ws);
    InitTcpSp(&tcp_opt->sp);

    CalculationTcp6Sum((PBYTE)buffer, sizeof(TCP_OPT));
}


EXTERN_C
DLLEXPORT
void WINAPI PacketizeSyn6(IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, IN PIN6_ADDR SourceAddress, IN PIN6_ADDR DestinationAddress, IN UINT16 th_sport, IN UINT16 th_dport,
                          OUT PBYTE buffer)
/*
th_sport：网络序。如果是主机序，请用htons转换下。
th_dport：网络序。如果是主机序，请用htons转换下。
buffer：长度是sizeof(RAW6_TCP)。
*/
{
    if (!SrcMac || !DesMac || !SourceAddress || !DestinationAddress || !buffer) {
        return;
    }

    PRAW6_TCP tcp6 = (PRAW6_TCP)buffer;

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV6, &tcp6->eth_hdr);

    // ip_hdr 是紧凑布局网络帧内的成员（紧随 14 字节以太网头），取址必然“未对齐”；x86/x64 允许非对齐访问，安全。
#pragma warning(suppress : 4366)
    InitIpv6HeaderForTcp(SourceAddress, DestinationAddress, IPPROTO_TCP, 0, &tcp6->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, 0, &tcp6->tcp_hdr);

    CalculationTcp6Sum(buffer, 0);
}


EXTERN_C
DLLEXPORT
void WINAPI packetize_icmpv6_echo_request(IN PDL_EUI48 SrcMac, IN PDL_EUI48 DesMac, IN PIN6_ADDR SourceAddress, IN PIN6_ADDR DestinationAddress, OUT PBYTE buffer)
/*
SrcMac：6字节长的本地的MAC。
buffer：长度是sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(ICMP_MESSAGE) + 0x20
*/
{
    if (!SrcMac || !DesMac || !SourceAddress || !DestinationAddress || !buffer) {
        return;
    }

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

    // ICMP_MESSAGE之后还有0x20字节载荷（PayloadLength已包含），本函数未写入。
    // 若调用方缓冲区未清零，这些栈/堆残留数据会被算入校验和并发送出去（信息泄露），故先清零。
    RtlZeroMemory((PBYTE)icmp_message + sizeof(ICMP_MESSAGE), 0x20);

    // icmp_message->Header.Checksum =
    calculation_icmpv6_echo_request_checksum(buffer, sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(ICMP_MESSAGE) + 0x20);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


static void InitUdpHeader(PETHERNET_HEADER eth_hdr, int Length, IN UINT16 SourcePort, IN UINT16 DestinationPort, OUT PUDP_HDR udp_hdr, PBYTE Data, WORD DataLen, bool IsIpv4)
/*
功能：组装UDP头。

只输入一个函数的返回类型，AI把函数的名字和参数都猜到了(不完全符合我的需求)，人工改进了，有待测试。
*/
{
    PVOID udp_payload = (PUDP_HDR)((PBYTE)udp_hdr + sizeof(UDP_HDR));
    if (Data && DataLen) {
        memcpy(udp_payload, Data, DataLen);
    }

    udp_hdr->dst_portno = DestinationPort;
    udp_hdr->src_portno = SourcePort;
    udp_hdr->udp_length = htons(DataLen + sizeof(UDP_HDR));
    udp_hdr->udp_checksum = 0;
    if (IsIpv4) {
        udp_hdr->udp_checksum = calc_udp4_sum((USHORT *)eth_hdr, Length); // 要不要转换字节序？
    } else {
        udp_hdr->udp_checksum = calc_udp6_sum((USHORT *)eth_hdr, Length); // 要不要转换字节序？
    }
}


EXTERN_C
DLLEXPORT
PVOID WINAPI PacketizeUdp4(PDL_EUI48 SrcMac, PDL_EUI48 DesMac, PIN_ADDR SourceAddress, PIN_ADDR DestinationAddress, WORD SourcePort, WORD DestinationPort, PBYTE Data,
                           WORD DataLen)
/*
AI生成的函数：名字是自己起的，参数和代码及注释都是AI生成的，甚至名字都猜到了。人工改进了，有待测试。
功能：构造一个走IPv4的UDP包。
*/
{
    int Length = sizeof(ETHERNET_HEADER) + sizeof(IPV4_HEADER) + sizeof(UDP_HDR) + DataLen;
    PETHERNET_HEADER eth_hdr = (PETHERNET_HEADER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Length);
    if (!eth_hdr) {
        return eth_hdr;
    }

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV4, eth_hdr);

    PIPV4_HEADER ipv4_header = (PIPV4_HEADER)((PBYTE)eth_hdr + ETH_LENGTH_OF_HEADER);
    InitIpv4Header(SourceAddress, DestinationAddress, IPPROTO_UDP, (UINT16)(Length - sizeof(ETHERNET_HEADER)), ipv4_header);

    PUDP_HDR udp_hdr = (PUDP_HDR)((PBYTE)ipv4_header + sizeof(IPV4_HEADER));
    InitUdpHeader(eth_hdr, Length, SourcePort, DestinationPort, udp_hdr, Data, DataLen, true);

    return eth_hdr;
}


EXTERN_C
DLLEXPORT
PVOID WINAPI PacketizeUdp6(PDL_EUI48 SrcMac, PDL_EUI48 DesMac, PIN6_ADDR SourceAddress, PIN6_ADDR DestinationAddress, WORD SourcePort, WORD DestinationPort, PBYTE Data,
                           WORD DataLen)
/*
AI生成的函数：名字是自己起的，参数和代码及注释都是AI生成的，甚至名字都猜到了。人工改进了，有待测试。
功能：构造一个走IPv6的UDP包。
*/
{
    int Length = sizeof(ETHERNET_HEADER) + sizeof(IPV6_HEADER) + sizeof(UDP_HDR) + DataLen;
    PETHERNET_HEADER eth_hdr = (PETHERNET_HEADER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Length);
    if (!eth_hdr) {
        return eth_hdr;
    }

    InitEthernetHeader(SrcMac, DesMac, ETHERNET_TYPE_IPV6, eth_hdr);

    PIPV6_HEADER ipv6_hdr = (PIPV6_HEADER)((PBYTE)eth_hdr + ETH_LENGTH_OF_HEADER);
    InitIpv6Header(SourceAddress, DestinationAddress, IPPROTO_UDP, (UINT16)(sizeof(UDP_HDR) + DataLen), ipv6_hdr);

    PUDP_HDR udp_hdr = (PUDP_HDR)((PBYTE)ipv6_hdr + sizeof(IPV6_HEADER));
    InitUdpHeader(eth_hdr, Length, SourcePort, DestinationPort, udp_hdr, Data, DataLen, false);

    return eth_hdr;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
