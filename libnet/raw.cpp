#include "pch.h"
#include "raw.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void InitEthernetHeader(IN PBYTE SrcMac, IN UINT16 Type, OUT PETHERNET_HEADER eth_hdr)
/*
���ܣ���д��̫ͷ��

������
Type��ȡֵ���磺ETHERNET_TYPE_IPV4��ETHERNET_TYPE_IPV6�� ETHERNET_TYPE_ARP�ȡ�

ע�ͣ���д��ٵ�Ŀ��MAC��Ҳ�ɷ��ͳ�ȥ��
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
                    IN UINT8 Protocol, //ȡֵ���磺IPPROTO_TCP�ȡ�
                    IN UINT16 TotalLength,//�ϸ�������ݵĴ�С��
                    OUT PIPV4_HEADER IPv4Header
)
/*
���ܣ���װIPv4ͷ��
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
���ܣ���in_ipv4��SYN�����ipv4��Ϣ��װΪbuffer��Ҫ������ACK��ipv4��

��;����ƭ��ɨ�裩��������ɨ��͹�����

������
IsCopy���Ǹ��ƻ��ǻظ���

ע�⣺����ǻظ��İ���Ҫ��Դ��Ŀ�Ļ�һ�¡�
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


void InitTcpHeader(IN UINT16 th_sport, //���������������������htonsת���¡�
                   IN UINT16 th_dport, //���������������������htonsת���¡�
                   IN SEQ_NUM th_ack,  //���������������������htonlת���¡�
                   IN UINT8 th_flags,  //TH_ACK, TH_SYN��ֵ����ϡ�
                   IN UINT8 OptLen,
                   OUT PTCP_HDR tcp_hdr
)
/*
���ܣ���װTCPͷ���ܹ�ʮ����Ա����

ע�⣺
1.����Ҫ��ֵ��Ĭ��Ϊ0.
2.ĳЩֵΪ�Լ��趨�Ĺ̶���ֵ��
3.У���Ϊ0�������ټ��㡣
*/
{
    RtlZeroMemory(tcp_hdr, sizeof(TCP_HDR));

    tcp_hdr->th_sport = th_sport;
    tcp_hdr->th_dport = th_dport;

    tcp_hdr->th_seq = ntohl(0);
    tcp_hdr->th_ack = th_ack;

    UINT8 x = (sizeof(TCP_HDR) + OptLen) / 4;
    ASSERT(x <= 0xf);//����������ᷢ����������벻���Ľ����    
    tcp_hdr->th_len = x;

    tcp_hdr->th_flags = th_flags;
    tcp_hdr->th_win = ntohs(65535);

    tcp_hdr->th_sum = 0;
    tcp_hdr->th_urp = 0;
    tcp_hdr->th_sum = 0;
}


void InitTcpHeaderBySyn(IN UINT16 th_sport, //���������������������htonsת���¡�
                        IN UINT16 th_dport, //���������������������htonsת���¡�
                        IN UINT8 OptLen,
                        OUT PTCP_HDR tcp_hdr
)
{
    InitTcpHeader(th_sport, th_dport, 0, TH_SYN, OptLen, tcp_hdr);
}


void InitTcpHeaderWithAck(IN PTCP_HDR tcp, IN bool IsCopy, OUT PTCP_HDR tcp_hdr)
/*


��;����ƭ��ɨ�裩��������ɨ��͹�����
*/
{
    if (IsCopy) {
        InitTcpHeader(tcp->th_sport,
                      tcp->th_dport,
                      tcp->th_seq + 1,//�յ��ĺż�һ��
                      TH_ACK | TH_SYN,
                      0,
                      tcp_hdr);
    } else {
        InitTcpHeader(tcp->th_dport,
                      tcp->th_sport,
                      tcp->th_seq + 1,//�յ��ĺż�һ��
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
���ܣ����㲢����tcp��У��͡�

������
OptLen����tcp����չѡ�TCP_OPT�����߶��⸽�������ݣ���http��html��)��
        ������ETHERNET_HEADER��IPV4_HEADER��TCP_HDR��
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
void WINAPI PacketizeSyn4(IN PBYTE SrcMac,    //6�ֽڳ��ı��ص�MAC��
                          IN PIN_ADDR SourceAddress,
                          IN PIN_ADDR DestinationAddress,
                          IN UINT16 th_sport, //���������������������htonsת���¡�
                          IN UINT16 th_dport, //���������������������htonsת���¡�
                          OUT PBYTE buffer    //������sizeof(RAW_TCP) + sizeof(TCP_OPT_MSS)��
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
                    IN UINT8 NextHeader, //ȡֵ���磺IPPROTO_TCP�ȡ�
                    OUT PIPV6_HEADER IPv6Header
)
/*
���ܣ���װIPv6Э���TCPͷ��
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
���ܣ���in_ipv6��SYN�����ipv6��Ϣ��װΪbuffer��Ҫ������ACK��ipv6��

��;����ƭ��ɨ�裩��������ɨ��͹�����

������
IsCopy���Ǹ��ƻ��ǻظ���

ע�⣺����ǻظ��İ���Ҫ��Դ��Ŀ�Ļ�һ�¡�
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
                          IN UINT16 th_sport, //���������������������htonsת���¡�
                          IN UINT16 th_dport, //���������������������htonsת���¡�
                          OUT PBYTE buffer    //������sizeof(RAW6_TCP) + sizeof(TCP_OPT_MSS)��
)
{
    PRAW6_TCP tcp6 = (PRAW6_TCP)buffer;

    InitEthernetHeader(SrcMac, ETHERNET_TYPE_IPV6, &tcp6->eth_hdr);

    InitIpv6Header(SourceAddress, DestinationAddress, IPPROTO_TCP, &tcp6->ip_hdr);

    InitTcpHeaderBySyn(th_sport, th_dport, 0, &tcp6->tcp_hdr);

    CalculationTcp6Sum(buffer, 0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
