#include "pch.h"
#include "raw.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void InitEthernet4Header(IN PBYTE SrcMac, OUT PRAW_TCP buffer)
/*
���ܣ���дIPv4�õ���̫ͷ��

ע�ͣ���д��ٵ�Ŀ��MAC��Ҳ�ɷ��ͳ�ȥ��
*/
{
    buffer->eth_hdr.Destination.Byte[0] = 0x99;
    buffer->eth_hdr.Destination.Byte[1] = 0x99;
    buffer->eth_hdr.Destination.Byte[2] = 0x99;
    buffer->eth_hdr.Destination.Byte[3] = 0x99;
    buffer->eth_hdr.Destination.Byte[4] = 0x99;
    buffer->eth_hdr.Destination.Byte[5] = 0x99;

    buffer->eth_hdr.Source.Byte[0] = SrcMac[0];
    buffer->eth_hdr.Source.Byte[1] = SrcMac[1];
    buffer->eth_hdr.Source.Byte[2] = SrcMac[2];
    buffer->eth_hdr.Source.Byte[3] = SrcMac[3];
    buffer->eth_hdr.Source.Byte[4] = SrcMac[4];
    buffer->eth_hdr.Source.Byte[5] = SrcMac[5];

    buffer->eth_hdr.Type = ntohs(ETHERNET_TYPE_IPV4);
}


void InitIpv4Header(IN PIN_ADDR SourceAddress,
                    IN PIN_ADDR DestinationAddress,
                    IN UINT8 Protocol, //ȡֵ���磺IPPROTO_TCP�ȡ�
                    OUT PRAW_TCP buffer
)
/*
���ܣ���װIPv4ͷ��
*/
{
    ASSERT(buffer);

#pragma warning( push )
#pragma warning( disable : 4366 ) //һԪ��&��������Ľ��������δ�����
    PIPV4_HEADER out_ipv4 = &buffer->ip_hdr;
#pragma warning( pop )

    out_ipv4->VersionAndHeaderLength = (4 << 4) | (sizeof(IPV4_HEADER) / sizeof(unsigned long));
    out_ipv4->TotalLength = ntohs(sizeof(IPV4_HEADER) +
                                  sizeof(TCP_HDR) +
                                  sizeof(TCP_OPT)); //�������ݣ������ټӱ�ġ�
    out_ipv4->DontFragment = TRUE;
    out_ipv4->TimeToLive = 128;
    out_ipv4->Protocol = Protocol;// IPPROTO_TCP;
    out_ipv4->SourceAddress.S_un.S_addr = SourceAddress->S_un.S_addr; 
    out_ipv4->DestinationAddress.S_un.S_addr = DestinationAddress->S_un.S_addr;
    out_ipv4->HeaderChecksum = checksum((unsigned short *)out_ipv4, sizeof(IPV4_HEADER));
}


void InitIpv4Header(IN PIPV4_HEADER in_ipv4, OUT PRAW_TCP buffer)
/*
���ܣ���packet��SYN�����ipv4��Ϣ��װΪbuffer��Ҫ������ACK��ipv4��

ע�⣺����ǻظ��İ���Ҫ��Դ��Ŀ�Ļ�һ�¡�
*/
{
    InitIpv4Header(&in_ipv4->SourceAddress, &in_ipv4->DestinationAddress, in_ipv4->Protocol, buffer);
}


void InitTcp4HeaderWithAck(PIPV4_HEADER ipv4, OUT PRAW_TCP buffer)
{
    PVOID NextHeader = ((PBYTE)ipv4 + Ip4HeaderLengthInBytes(ipv4));

    PTCP_HDR tcp = (PTCP_HDR)NextHeader;
    UINT16 Source_port = ntohs(tcp->th_sport);
    UINT16 Destination_port = ntohs(tcp->th_dport);

    ASSERT(buffer);
    ASSERT(NextHeader);
    ASSERT(TH_SYN == tcp->th_flags);

    LONG th_ack = ntohl(tcp->th_seq);
    th_ack++;

    PTCP_HDR tcp_hdr = &buffer->tcp_hdr;

    tcp_hdr->th_sport = ntohs(Destination_port);
    tcp_hdr->th_dport = ntohs(Source_port);
    tcp_hdr->th_seq = ntohl(0);
    tcp_hdr->th_ack = ntohl(th_ack);//�յ��ĺż�һ��

    UINT8 x = (sizeof(TCP_HDR) + sizeof(TCP_OPT)) / 4;
    ASSERT(x <= 0xf);//����������ᷢ����������벻���Ľ����    
    tcp_hdr->th_len = x;

    tcp_hdr->th_flags = TH_ACK | TH_SYN;
    tcp_hdr->th_win = ntohs(65535);
    tcp_hdr->th_sum = 0;
    tcp_hdr->th_urp = 0;

    tcp_hdr->th_sum = 0;
}


void InitTcpMss(OUT PRAW_TCP buffer)
{
    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW_TCP));

    tcp_opt->mss.Kind = TH_OPT_MSS;
    tcp_opt->mss.Length = 4;
    tcp_opt->mss.Mss = ntohs(1460);

    tcp_opt->unuse1 = TH_OPT_NOP;
    tcp_opt->unuse2 = TH_OPT_NOP << 8 | TH_OPT_NOP;
}


void InitTcpWs(OUT PRAW_TCP buffer)
{
    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW_TCP));

    tcp_opt->ws.Kind = TH_OPT_WS;
    tcp_opt->ws.Length = 3;
    tcp_opt->ws.ShiftCnt = 8;
}


void InitTcpSp(OUT PRAW_TCP buffer)
{
    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW_TCP));

    tcp_opt->sp.Kind = TH_OPT_SACK_PERMITTED;
    tcp_opt->sp.Length = 2;
}


void CalculationTcp4Sum(OUT PRAW_TCP buffer)
{
    BYTE temp[sizeof(PSD_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT)] = {0};

    PSD_HEADER * pseudo_header = (PSD_HEADER *)temp;

    pseudo_header->saddr = buffer->ip_hdr.SourceAddress.S_un.S_addr;
    pseudo_header->daddr = buffer->ip_hdr.DestinationAddress.S_un.S_addr;
    pseudo_header->mbz = 0;
    pseudo_header->ptcl = IPPROTO_TCP;
    pseudo_header->tcpl = ntohs(sizeof(TCP_HDR) + sizeof(TCP_OPT));

    PBYTE test = &temp[0] + sizeof(PSD_HEADER);
    RtlCopyMemory(test, &buffer->tcp_hdr, sizeof(TCP_HDR));

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&buffer->tcp_hdr + sizeof(TCP_HDR), sizeof(TCP_OPT));

    buffer->tcp_hdr.th_sum = checksum((USHORT *)temp, sizeof(PSD_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT));
}


void PacketizeAck4(IN PIPV4_HEADER in_ipv4, IN PBYTE SrcMac, OUT PRAW_TCP buffer)
{
    ASSERT(SrcMac);
    ASSERT(buffer);

    InitEthernet4Header(SrcMac, buffer);
    InitIpv4Header(in_ipv4, buffer);
    InitTcp4HeaderWithAck(in_ipv4, buffer);

    InitTcpMss(buffer);
    InitTcpWs(buffer);
    InitTcpSp(buffer);

    CalculationTcp4Sum(buffer);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void InitEthernet6Header(IN PBYTE SrcMac, OUT PRAW6_TCP buffer)
/*
���ܣ���дIPv6�õ���̫ͷ��

ע�ͣ���д��ٵ�Ŀ��MAC��Ҳ�ɷ��ͳ�ȥ��
*/
{
    buffer->eth_hdr.Destination.Byte[0] = 0x99;
    buffer->eth_hdr.Destination.Byte[1] = 0x99;
    buffer->eth_hdr.Destination.Byte[2] = 0x99;
    buffer->eth_hdr.Destination.Byte[3] = 0x99;
    buffer->eth_hdr.Destination.Byte[4] = 0x99;
    buffer->eth_hdr.Destination.Byte[5] = 0x99;

    buffer->eth_hdr.Source.Byte[0] = SrcMac[0];
    buffer->eth_hdr.Source.Byte[1] = SrcMac[1];
    buffer->eth_hdr.Source.Byte[2] = SrcMac[2];
    buffer->eth_hdr.Source.Byte[3] = SrcMac[3];
    buffer->eth_hdr.Source.Byte[4] = SrcMac[4];
    buffer->eth_hdr.Source.Byte[5] = SrcMac[5];

    buffer->eth_hdr.Type = ntohs(ETHERNET_TYPE_IPV6);
}


void InitIpv6Header(IN PIN6_ADDR SourceAddress,
                    IN PIN6_ADDR DestinationAddress,
                    IN UINT8 NextHeader, //ȡֵ���磺IPPROTO_TCP�ȡ�
                    OUT PRAW6_TCP buffer
)
/*
���ܣ���װIPv6Э���TCPͷ��
*/
{
    ASSERT(buffer);

//#pragma warning(suppress:4366) //һԪ��&��������Ľ��������δ�����
    PIPV6_HEADER ipv6_hdr = (PIPV6_HEADER)&buffer->ip_hdr;

    ipv6_hdr->VersionClassFlow = ntohl((6 << 28) | (0 << 20) | 0);// IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    ipv6_hdr->PayloadLength = ntohs(sizeof(TCP_HDR) + sizeof(TCP_OPT));
    ipv6_hdr->NextHeader = NextHeader;
    ipv6_hdr->HopLimit = 128;

    RtlCopyMemory(&ipv6_hdr->SourceAddress, SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&ipv6_hdr->DestinationAddress, DestinationAddress, sizeof(IN6_ADDR));
}


void InitIpv6Header(IN PIPV6_HEADER in_ipv6, OUT PRAW6_TCP buffer)
/*
���ܣ���packet��SYN�����ipv6��Ϣ��װΪbuffer��Ҫ������ACK��ipv6��

ע�⣺����ǻظ��İ���Ҫ��Դ��Ŀ�Ļ�һ�¡�
*/
{
    InitIpv6Header(&in_ipv6->SourceAddress, &in_ipv6->DestinationAddress, in_ipv6->NextHeader, buffer);
}


void InitTcp6HeaderWithAck(PIPV6_HEADER ipv6, OUT PRAW6_TCP buffer)
//��ipv4��ࡣ
{
    ASSERT(buffer);

    PVOID NextHeader = ((PBYTE)ipv6 + sizeof(IPV6_HEADER));
    PTCP_HDR tcp = (PTCP_HDR)NextHeader;
    UINT16 Source_port = ntohs(tcp->th_sport);
    UINT16 Destination_port = ntohs(tcp->th_dport);

    ASSERT(TH_SYN == tcp->th_flags);

    LONG th_ack = ntohl(tcp->th_seq);
    th_ack++;

    PTCP_HDR tcp_hdr = &buffer->tcp_hdr;

    tcp_hdr->th_sport = ntohs(Destination_port);
    tcp_hdr->th_dport = ntohs(Source_port);
    tcp_hdr->th_seq = ntohl(0);
    tcp_hdr->th_ack = ntohl(th_ack);//�յ��ĺż�һ��

    UINT8 x = (sizeof(TCP_HDR) + sizeof(TCP_OPT)) / 4;
    ASSERT(x <= 0xf);//����������ᷢ����������벻���Ľ����    
    tcp_hdr->th_len = x;

    tcp_hdr->th_flags = TH_ACK | TH_SYN;
    tcp_hdr->th_win = ntohs(65535);
    tcp_hdr->th_sum = 0;
    tcp_hdr->th_urp = 0;

    tcp_hdr->th_sum = 0;
}


void InitTcp6Mss(OUT PRAW6_TCP buffer)
{
    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW6_TCP));

    tcp_opt->mss.Kind = TH_OPT_MSS;
    tcp_opt->mss.Length = 4;
    tcp_opt->mss.Mss = ntohs(1460);

    tcp_opt->unuse1 = TH_OPT_NOP;
    tcp_opt->unuse2 = TH_OPT_NOP << 8 | TH_OPT_NOP;
}


void InitTcp6Ws(OUT PRAW6_TCP buffer)
{
    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW6_TCP));

    tcp_opt->ws.Kind = TH_OPT_WS;
    tcp_opt->ws.Length = 3;
    tcp_opt->ws.ShiftCnt = 8;
}


void InitTcp6Sp(OUT PRAW6_TCP buffer)
{
    PTCP_OPT tcp_opt = (PTCP_OPT)((PBYTE)buffer + sizeof(RAW6_TCP));

    tcp_opt->sp.Kind = TH_OPT_SACK_PERMITTED;
    tcp_opt->sp.Length = 2;
}


void CalculationTcp6Sum(OUT PRAW6_TCP buffer)
{
    BYTE temp[sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT)] = {0};

    PSD6_HEADER * pseudo_header = (PSD6_HEADER *)temp;

    RtlCopyMemory(&pseudo_header->saddr, &buffer->ip_hdr.SourceAddress, sizeof(IN6_ADDR));
    RtlCopyMemory(&pseudo_header->daddr, &buffer->ip_hdr.DestinationAddress, sizeof(IN6_ADDR));
    pseudo_header->length = ntohl(sizeof(TCP_HDR) + sizeof(TCP_OPT));
    pseudo_header->unused1 = 0;
    pseudo_header->unused2 = 0;
    pseudo_header->unused3 = 0;
    pseudo_header->proto = IPPROTO_TCP;

    PBYTE test = &temp[0] + sizeof(PSD6_HEADER);
    RtlCopyMemory(test, &buffer->tcp_hdr, sizeof(TCP_HDR));

    test = test + sizeof(TCP_HDR);
    RtlCopyMemory(test, (PBYTE)&buffer->tcp_hdr + sizeof(TCP_HDR), sizeof(TCP_OPT));

    buffer->tcp_hdr.th_sum = checksum((USHORT *)temp, sizeof(PSD6_HEADER) + sizeof(TCP_HDR) + sizeof(TCP_OPT));
}


void PacketizeAck6(IN PIPV6_HEADER ipv6, IN PBYTE SrcMac, OUT PRAW6_TCP buffer)
{
    ASSERT(SrcMac);
    ASSERT(buffer);

    InitEthernet6Header(SrcMac, buffer);
    InitIpv6Header(ipv6, buffer);
    InitTcp6HeaderWithAck(ipv6, buffer);

    InitTcp6Mss(buffer);
    InitTcp6Ws(buffer);
    InitTcp6Sp(buffer);

    CalculationTcp6Sum(buffer);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
