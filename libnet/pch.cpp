#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void DisplayError(_In_ DWORD dwMessageId)
{
    LPVOID lpMsgBuf = nullptr; /* Variables used to return error message */
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    if (FormatMessage(dwFlags,
                      nullptr,
                      dwMessageId,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR)&lpMsgBuf,
                      0,
                      nullptr)) {
        printf("\tError: %s", (char *)lpMsgBuf);
    }
    LocalFree(lpMsgBuf);
}


void GetTimeString(LARGE_INTEGER UtcTime, _Out_ LPWSTR TimeString)
/*
功能：把UTC时间转换为本地时间的字符串形式。
*/
{
    FILETIME ft;
    ft.dwHighDateTime = UtcTime.HighPart;
    ft.dwLowDateTime = UtcTime.LowPart;

    FILETIME lft;
    BOOL B = FileTimeToLocalFileTime(&ft, &lft);
    _ASSERTE(B);

    SYSTEMTIME st;
    // GetLocalTime(&st);
    B = FileTimeToSystemTime(&lft, &st);
    _ASSERTE(B);

    // SystemTimeToTzSpecificLocalTime

    //格式：2016-07-11 17:35:54
    wsprintfW(TimeString,
              L"%04u-%02u-%02u %02u:%02u:%02u.\r\n",
              st.wYear,
              st.wMonth,
              st.wDay,
              st.wHour,
              st.wMinute,
              st.wSecond);
}


void PrintTcpConnectionState(_In_ DWORD dwState)
// Print The state of the TCP connection
{
    switch (dwState) {
    case MIB_TCP_STATE_CLOSED:
        printf("CLOSED\n");
        break;
    case MIB_TCP_STATE_LISTEN:
        printf("LISTEN\n");
        break;
    case MIB_TCP_STATE_SYN_SENT:
        printf("SYN-SENT\n");
        break;
    case MIB_TCP_STATE_SYN_RCVD:
        printf("SYN-RECEIVED\n");
        break;
    case MIB_TCP_STATE_ESTAB:
        printf("ESTABLISHED\n");
        break;
    case MIB_TCP_STATE_FIN_WAIT1:
        printf("FIN-WAIT-1\n");
        break;
    case MIB_TCP_STATE_FIN_WAIT2:
        printf("FIN-WAIT-2 \n");
        break;
    case MIB_TCP_STATE_CLOSE_WAIT:
        printf("CLOSE-WAIT\n");
        break;
    case MIB_TCP_STATE_CLOSING:
        printf("CLOSING\n");
        break;
    case MIB_TCP_STATE_LAST_ACK:
        printf("LAST-ACK\n");
        break;
    case MIB_TCP_STATE_TIME_WAIT:
        printf("TIME-WAIT\n");
        break;
    case MIB_TCP_STATE_DELETE_TCB:
        printf("DELETE-TCB\n");
        break;
    default:
        wprintf(L"UNKNOWN dwState value: %u\n", dwState);
        break;
    }
}


void PrintInterfaceType(_In_ WORD Type)
{
    _ASSERTE(Type >= MIN_IF_TYPE);
    _ASSERTE(Type <= MAX_IF_TYPE);

    switch (Type) {
    case IF_TYPE_OTHER:
        printf("OTHER\n");
        break;
    case IF_TYPE_REGULAR_1822:
        printf("REGULAR_1822\n");
        break;
    case IF_TYPE_HDH_1822:
        printf("HDH_1822\n");
        break;
    case IF_TYPE_DDN_X25:
        printf("DDN_X25\n");
        break;
    case IF_TYPE_RFC877_X25:
        printf("RFC877_X25\n");
        break;
    case IF_TYPE_ETHERNET_CSMACD:
        printf("ETHERNET_CSMACD\n");
        break;
    case IF_TYPE_IS088023_CSMACD:
        printf("IS088023_CSMACD\n");
        break;
    case IF_TYPE_ISO88024_TOKENBUS:
        printf("ISO88024_TOKENBUS\n");
        break;
    case IF_TYPE_ISO88025_TOKENRING:
        printf("ISO88025_TOKENRING\n");
        break;
    case IF_TYPE_ISO88026_MAN:
        printf("ISO88026_MAN\n");
        break;
    case IF_TYPE_STARLAN:
        printf("STARLAN\n");
        break;
    case IF_TYPE_PROTEON_10MBIT:
        printf("PROTEON_10MBIT\n");
        break;
    case IF_TYPE_PROTEON_80MBIT:
        printf("PROTEON_80MBIT\n");
        break;
    case IF_TYPE_HYPERCHANNEL:
        printf("HYPERCHANNEL\n");
        break;
    case IF_TYPE_FDDI:
        printf("FDDI\n");
        break;
    case IF_TYPE_LAP_B:
        printf("LAP_B\n");
        break;
    case IF_TYPE_SDLC:
        printf("SDLC\n");
        break;
    case IF_TYPE_DS1:
        printf("DS1-MIB\n");
        break;
    case IF_TYPE_E1:
        printf("Obsolete; see DS1-MIB\n");
        break;
    case IF_TYPE_BASIC_ISDN:
        printf("BASIC_ISDN\n");
        break;
    case IF_TYPE_PRIMARY_ISDN:
        printf("PRIMARY_ISDN\n");
        break;
    case IF_TYPE_PROP_POINT2POINT_SERIAL:
        printf("proprietary serial\n");
        break;
    case IF_TYPE_PPP:
        printf("PPP\n");
        break;
    case IF_TYPE_SOFTWARE_LOOPBACK:
        printf("SOFTWARE_LOOPBACK\n");
        break;
    case IF_TYPE_EON:
        printf("CLNP over IP\n");
        break;
    case IF_TYPE_ETHERNET_3MBIT:
        printf("ETHERNET_3MBIT\n");
        break;
    case IF_TYPE_NSIP:
        printf("XNS over IP\n");
        break;
    case IF_TYPE_SLIP:
        printf("Generic Slip\n");
        break;
    case IF_TYPE_ULTRA:
        printf("ULTRA Technologies\n");
        break;
    case IF_TYPE_DS3:
        printf("DS3-MIB\n");
        break;
    case IF_TYPE_SIP:
        printf("SMDS, coffee\n");
        break;
    case IF_TYPE_FRAMERELAY:
        printf("DTE only\n");
        break;
    case IF_TYPE_RS232:
        printf("RS232\n");
        break;
    case IF_TYPE_PARA:
        printf("Parallel port\n");
        break;
    case IF_TYPE_ARCNET:
        printf("ARCNET\n");
        break;
    case IF_TYPE_ARCNET_PLUS:
        printf("ARCNET_PLUS\n");
        break;
    case IF_TYPE_ATM:
        printf("ATM cells\n");
        break;
    case IF_TYPE_MIO_X25:
        printf("MIO_X25\n");
        break;
    case IF_TYPE_SONET:
        printf("SONET or SDH\n");
        break;
    case IF_TYPE_X25_PLE:
        printf("X25_PLE\n");
        break;
    case IF_TYPE_ISO88022_LLC:
        printf("ISO88022_LLC\n");
        break;
    case IF_TYPE_LOCALTALK:
        printf("LOCALTALK\n");
        break;
    case IF_TYPE_SMDS_DXI:
        printf("SMDS_DXI\n");
        break;
    case IF_TYPE_FRAMERELAY_SERVICE:
        printf("FRNETSERV-MIB\n");
        break;
    case IF_TYPE_V35:
        printf("V35\n");
        break;
    case IF_TYPE_HSSI:
        printf("HSSI\n");
        break;
    case IF_TYPE_HIPPI:
        printf("HIPPI\n");
        break;
    case IF_TYPE_MODEM:
        printf("Generic Modem\n");
        break;
    case IF_TYPE_AAL5:
        printf("AAL5 over ATM\n");
        break;
    case IF_TYPE_SONET_PATH:
        printf("SONET_PATH\n");
        break;
    case IF_TYPE_SONET_VT:
        printf("SONET_VT\n");
        break;
    case IF_TYPE_SMDS_ICIP:
        printf("SMDS InterCarrier Interface\n");
        break;
    case IF_TYPE_PROP_VIRTUAL:
        printf("Proprietary virtual/internal\n");
        break;
    case IF_TYPE_PROP_MULTIPLEXOR:
        printf("Proprietary multiplexing\n");
        break;
    case IF_TYPE_IEEE80212:
        printf("100BaseVG\n");
        break;
    case IF_TYPE_FIBRECHANNEL:
        printf("FIBRECHANNEL\n");
        break;
    case IF_TYPE_HIPPIINTERFACE:
        printf("HIPPIINTERFACE\n");
        break;
    case IF_TYPE_FRAMERELAY_INTERCONNECT:
        printf("FRAMERELAY_INTERCONNECT\n");
        break;
    case IF_TYPE_AFLANE_8023:
        printf("ATM Emulated LAN for 802.3\n");
        break;
    case IF_TYPE_AFLANE_8025:
        printf("ATM Emulated LAN for 802.5\n");
        break;
    case IF_TYPE_CCTEMUL:
        printf("ATM Emulated circuit\n");
        break;
    case IF_TYPE_FASTETHER:
        printf("Fast Ethernet (100BaseT)\n");
        break;
    case IF_TYPE_ISDN:
        printf("ISDN and X.25\n");
        break;
    case IF_TYPE_V11:
        printf("CCITT V.11/X.21\n");
        break;
    case IF_TYPE_V36:
        printf("CCITT V.36\n");
        break;
    case IF_TYPE_G703_64K:
        printf("CCITT G703 at 64Kbps\n");
        break;
    case IF_TYPE_G703_2MB:
        printf("Obsolete; see DS1-MIB\n");
        break;
    case IF_TYPE_QLLC:
        printf("SNA QLLC\n");
        break;
    case IF_TYPE_FASTETHER_FX:
        printf("Fast Ethernet (100BaseFX)\n");
        break;
    case IF_TYPE_CHANNEL:
        printf("CHANNEL\n");
        break;
    case IF_TYPE_IEEE80211:
        printf("Radio spread spectrum\n");
        break;
    case IF_TYPE_IBM370PARCHAN:
        printf("IBM System 360/370 OEMI Channel\n");
        break;
    case IF_TYPE_ESCON:
        printf("IBM Enterprise Systems Connection\n");
        break;
    case IF_TYPE_DLSW:
        printf("Data Link Switching\n");
        break;
    case IF_TYPE_ISDN_S:
        printf("ISDN S/T interface\n");
        break;
    case IF_TYPE_ISDN_U:
        printf("ISDN U interface\n");
        break;
    case IF_TYPE_LAP_D:
        printf("Link Access Protocol D\n");
        break;
    case IF_TYPE_IPSWITCH:
        printf("IP Switching Objects\n");
        break;
    case IF_TYPE_RSRB:
        printf("Remote Source Route Bridging\n");
        break;
    case IF_TYPE_ATM_LOGICAL:
        printf("ATM Logical Port\n");
        break;
    case IF_TYPE_DS0:
        printf("Digital Signal Level 0\n");
        break;
    case IF_TYPE_DS0_BUNDLE:
        printf("Group of ds0s on the same ds1\n");
        break;
    case IF_TYPE_BSC:
        printf("Bisynchronous Protocol\n");
        break;
    case IF_TYPE_ASYNC:
        printf("Asynchronous Protocol\n");
        break;
    case IF_TYPE_CNR:
        printf("Combat Net Radio\n");
        break;
    case IF_TYPE_ISO88025R_DTR:
        printf("ISO 802.5r DTR\n");
        break;
    case IF_TYPE_EPLRS:
        printf("Ext Pos Loc Report Sys\n");
        break;
    case IF_TYPE_ARAP:
        printf("Appletalk Remote Access Protocol\n");
        break;
    case IF_TYPE_PROP_CNLS:
        printf("Proprietary Connectionless Proto\n");
        break;
    case IF_TYPE_HOSTPAD:
        printf("CCITT-ITU X.29 PAD Protocol\n");
        break;
    case IF_TYPE_TERMPAD:
        printf("CCITT-ITU X.3 PAD Facility\n");
        break;
    case IF_TYPE_FRAMERELAY_MPI:
        printf("Multiproto Interconnect over FR\n");
        break;
    case IF_TYPE_X213:
        printf("CCITT-ITU X213\n");
        break;
    case IF_TYPE_ADSL:
        printf("Asymmetric Digital Subscrbr Loop\n");
        break;
    case IF_TYPE_RADSL:
        printf("Rate-Adapt Digital Subscrbr Loop\n");
        break;
    case IF_TYPE_SDSL:
        printf("Symmetric Digital Subscriber Loop\n");
        break;
    case IF_TYPE_VDSL:
        printf("Very H-Speed Digital Subscrb Loop\n");
        break;
    case IF_TYPE_ISO88025_CRFPRINT:
        printf("ISO 802.5 CRFP\n");
        break;
    case IF_TYPE_MYRINET:
        printf("Myricom Myrinet\n");
        break;
    case IF_TYPE_VOICE_EM:
        printf("Voice recEive and transMit\n");
        break;
    case IF_TYPE_VOICE_FXO:
        printf("Voice Foreign Exchange Office\n");
        break;
    case IF_TYPE_VOICE_FXS:
        printf("Voice Foreign Exchange Station\n");
        break;
    case IF_TYPE_VOICE_ENCAP:
        printf("Voice encapsulation\n");
        break;
    case IF_TYPE_VOICE_OVERIP:
        printf("Voice over IP encapsulation\n");
        break;
    case IF_TYPE_ATM_DXI:
        printf("ATM DXI\n");
        break;
    case IF_TYPE_ATM_FUNI:
        printf("ATM FUNI\n");
        break;
    case IF_TYPE_ATM_IMA:
        printf("ATM IMA\n");
        break;
    case IF_TYPE_PPPMULTILINKBUNDLE:
        printf("PPP Multilink Bundle\n");
        break;
    case IF_TYPE_IPOVER_CDLC:
        printf("IBM ipOverCdlc\n");
        break;
    case IF_TYPE_IPOVER_CLAW:
        printf("IBM Common Link Access to Workstn\n");
        break;
    case IF_TYPE_STACKTOSTACK:
        printf("IBM stackToStack\n");
        break;
    case IF_TYPE_VIRTUALIPADDRESS:
        printf("IBM VIPA\n");
        break;
    case IF_TYPE_MPC:
        printf("IBM multi-proto channel support\n");
        break;
    case IF_TYPE_IPOVER_ATM:
        printf("IBM ipOverAtm\n");
        break;
    case IF_TYPE_ISO88025_FIBER:
        printf("ISO 802.5j Fiber Token Ring\n");
        break;
    case IF_TYPE_TDLC:
        printf("IBM twinaxial data link control\n");
        break;
    case IF_TYPE_GIGABITETHERNET:
        printf("GIGABITETHERNET\n");
        break;
    case IF_TYPE_HDLC:
        printf("HDLC\n");
        break;
    case IF_TYPE_LAP_F:
        printf("LAP_F\n");
        break;
    case IF_TYPE_V37:
        printf("V37\n");
        break;
    case IF_TYPE_X25_MLP:
        printf("Multi-Link Protocol\n");
        break;
    case IF_TYPE_X25_HUNTGROUP:
        printf("X.25 Hunt Group\n");
        break;
    case IF_TYPE_TRANSPHDLC:
        printf("TRANSPHDLC\n");
        break;
    case IF_TYPE_INTERLEAVE:
        printf("Interleave channel\n");
        break;
    case IF_TYPE_FAST:
        printf("Fast channel\n");
        break;
    case IF_TYPE_IP:
        printf("IP (for APPN HPR in IP networks)\n");
        break;
    case IF_TYPE_DOCSCABLE_MACLAYER:
        printf("CATV Mac Layer\n");
        break;
    case IF_TYPE_DOCSCABLE_DOWNSTREAM:
        printf("CATV Downstream interface\n");
        break;
    case IF_TYPE_DOCSCABLE_UPSTREAM:
        printf("CATV Upstream interface\n");
        break;
    case IF_TYPE_A12MPPSWITCH:
        printf("Avalon Parallel Processor\n");
        break;
    case IF_TYPE_TUNNEL:
        printf("Encapsulation interface\n");
        break;
    case IF_TYPE_COFFEE:
        printf("Coffee pot\n");
        break;
    case IF_TYPE_CES:
        printf("Circuit Emulation Service\n");
        break;
    case IF_TYPE_ATM_SUBINTERFACE:
        printf("ATM Sub Interface\n");
        break;
    case IF_TYPE_L2_VLAN:
        printf("Layer 2 Virtual LAN using 802.1Q\n");
        break;
    case IF_TYPE_L3_IPVLAN:
        printf("Layer 3 Virtual LAN using IP\n");
        break;
    case IF_TYPE_L3_IPXVLAN:
        printf("Layer 3 Virtual LAN using IPX\n");
        break;
    case IF_TYPE_DIGITALPOWERLINE:
        printf("IP over Power Lines\n");
        break;
    case IF_TYPE_MEDIAMAILOVERIP:
        printf("Multimedia Mail over IP\n");
        break;
    case IF_TYPE_DTM:
        printf("Dynamic syncronous Transfer Mode\n");
        break;
    case IF_TYPE_DCN:
        printf("Data Communications Network\n");
        break;
    case IF_TYPE_IPFORWARD:
        printf("IP Forwarding Interface\n");
        break;
    case IF_TYPE_MSDSL:
        printf("Multi-rate Symmetric DSL\n");
        break;
    case IF_TYPE_IEEE1394:
        printf("IEEE1394 High Perf Serial Bus\n");
        break;
    case IF_TYPE_IF_GSN:
        printf("IF_GSN\n");
        break;
    case IF_TYPE_DVBRCC_MACLAYER:
        printf("DVBRCC_MACLAYER\n");
        break;
    case IF_TYPE_DVBRCC_DOWNSTREAM:
        printf("DVBRCC_DOWNSTREAM\n");
        break;
    case IF_TYPE_DVBRCC_UPSTREAM:
        printf("DVBRCC_UPSTREAM\n");
        break;
    case IF_TYPE_ATM_VIRTUAL:
        printf("ATM_VIRTUAL\n");
        break;
    case IF_TYPE_MPLS_TUNNEL:
        printf("IF_TYPE_MPLS_TUNNEL\n");
        break;
    case IF_TYPE_SRP:
        printf("IF_TYPE_SRP\n");
        break;
    case IF_TYPE_VOICEOVERATM:
        printf("IF_TYPE_VOICEOVERATM\n");
        break;
    case IF_TYPE_VOICEOVERFRAMERELAY:
        printf("IF_TYPE_VOICEOVERFRAMERELAY\n");
        break;
    case IF_TYPE_IDSL:
        printf("IF_TYPE_IDSL\n");
        break;
    case IF_TYPE_COMPOSITELINK:
        printf("IF_TYPE_COMPOSITELINK\n");
        break;
    case IF_TYPE_SS7_SIGLINK:
        printf("IF_TYPE_SS7_SIGLINK\n");
        break;
    case IF_TYPE_PROP_WIRELESS_P2P:
        printf("IF_TYPE_PROP_WIRELESS_P2P\n");
        break;
    case IF_TYPE_FR_FORWARD:
        printf("IF_TYPE_FR_FORWARD\n");
        break;
    case IF_TYPE_RFC1483:
        printf("IF_TYPE_RFC1483\n");
        break;
    case IF_TYPE_USB:
        printf("IF_TYPE_USB\n");
        break;
    case IF_TYPE_IEEE8023AD_LAG:
        printf("IF_TYPE_IEEE8023AD_LAG\n");
        break;
    case IF_TYPE_BGP_POLICY_ACCOUNTING:
        printf("IF_TYPE_BGP_POLICY_ACCOUNTING\n");
        break;
    case IF_TYPE_FRF16_MFR_BUNDLE:
        printf("IF_TYPE_FRF16_MFR_BUNDLE\n");
        break;
    case IF_TYPE_H323_GATEKEEPER:
        printf("IF_TYPE_H323_GATEKEEPER\n");
        break;
    case IF_TYPE_H323_PROXY:
        printf("IF_TYPE_H323_PROXY\n");
        break;
    case IF_TYPE_MPLS:
        printf("IF_TYPE_MPLS\n");
        break;
    case IF_TYPE_MF_SIGLINK:
        printf("IF_TYPE_MF_SIGLINK\n");
        break;
    case IF_TYPE_HDSL2:
        printf("IF_TYPE_HDSL2\n");
        break;
    case IF_TYPE_SHDSL:
        printf("IF_TYPE_SHDSL\n");
        break;
    case IF_TYPE_DS1_FDL:
        printf("IF_TYPE_DS1_FDL\n");
        break;
    case IF_TYPE_POS:
        printf("IF_TYPE_POS\n");
        break;
    case IF_TYPE_DVB_ASI_IN:
        printf("IF_TYPE_DVB_ASI_IN\n");
        break;
    case IF_TYPE_DVB_ASI_OUT:
        printf("IF_TYPE_DVB_ASI_OUT\n");
        break;
    case IF_TYPE_PLC:
        printf("IF_TYPE_PLC\n");
        break;
    case IF_TYPE_NFAS:
        printf("IF_TYPE_NFAS\n");
        break;
    case IF_TYPE_TR008:
        printf("IF_TYPE_TR008\n");
        break;
    case IF_TYPE_GR303_RDT:
        printf("IF_TYPE_GR303_RDT\n");
        break;
    case IF_TYPE_GR303_IDT:
        printf("IF_TYPE_GR303_IDT\n");
        break;
    case IF_TYPE_ISUP:
        printf("IF_TYPE_ISUP\n");
        break;
    case IF_TYPE_PROP_DOCS_WIRELESS_MACLAYER:
        printf("IF_TYPE_PROP_DOCS_WIRELESS_MACLAYER\n");
        break;
    case IF_TYPE_PROP_DOCS_WIRELESS_DOWNSTREAM:
        printf("IF_TYPE_PROP_DOCS_WIRELESS_DOWNSTREAM\n");
        break;
    case IF_TYPE_PROP_DOCS_WIRELESS_UPSTREAM:
        printf("IF_TYPE_PROP_DOCS_WIRELESS_UPSTREAM\n");
        break;
    case IF_TYPE_HIPERLAN2:
        printf("IF_TYPE_HIPERLAN2\n");
        break;
    case IF_TYPE_PROP_BWA_P2MP:
        printf("IF_TYPE_PROP_BWA_P2MP\n");
        break;
    case IF_TYPE_SONET_OVERHEAD_CHANNEL:
        printf("IF_TYPE_SONET_OVERHEAD_CHANNEL\n");
        break;
    case IF_TYPE_DIGITAL_WRAPPER_OVERHEAD_CHANNEL:
        printf("IF_TYPE_DIGITAL_WRAPPER_OVERHEAD_CHANNEL\n");
        break;
    case IF_TYPE_AAL2:
        printf("IF_TYPE_AAL2\n");
        break;
    case IF_TYPE_RADIO_MAC:
        printf("IF_TYPE_RADIO_MAC\n");
        break;
    case IF_TYPE_ATM_RADIO:
        printf("IF_TYPE_ATM_RADIO\n");
        break;
    case IF_TYPE_IMT:
        printf("IF_TYPE_IMT\n");
        break;
    case IF_TYPE_MVL:
        printf("IF_TYPE_MVL\n");
        break;
    case IF_TYPE_REACH_DSL:
        printf("IF_TYPE_REACH_DSL\n");
        break;
    case IF_TYPE_FR_DLCI_ENDPT:
        printf("IF_TYPE_FR_DLCI_ENDPT\n");
        break;
    case IF_TYPE_ATM_VCI_ENDPT:
        printf("IF_TYPE_ATM_VCI_ENDPT\n");
        break;
    case IF_TYPE_OPTICAL_CHANNEL:
        printf("IF_TYPE_OPTICAL_CHANNEL\n");
        break;
    case IF_TYPE_OPTICAL_TRANSPORT:
        printf("IF_TYPE_OPTICAL_TRANSPORT\n");
        break;
    case IF_TYPE_IEEE80216_WMAN:
        printf("IF_TYPE_IEEE80216_WMAN\n");
        break;
    case IF_TYPE_WWANPP:
        printf("WWAN devices based on GSM technology\n");
        break;
    case IF_TYPE_WWANPP2:
        printf("WWAN devices based on CDMA technology\n");
        break;
    case IF_TYPE_IEEE802154:
        printf("IEEE 802.15.4 WPAN interface\n");
        break;
    case IF_TYPE_XBOX_WIRELESS:
        printf("IF_TYPE_XBOX_WIRELESS\n");
        break;
    default:
        wprintf(L"UNKNOWN Type value: %u\n", Type);
        break;
    }
}


void PrintNeighborState(_In_ NL_NEIGHBOR_STATE State)
{
    switch (State) {
    case NlnsUnreachable:
        printf("NlnsUnreachable\n");
        break;
    case NlnsIncomplete:
        printf("NlnsIncomplete\n");
        break;
    case NlnsProbe:
        printf("NlnsProbe\n");
        break;
    case NlnsDelay:
        printf("NlnsDelay\n");
        break;
    case NlnsStale:
        printf("NlnsStale\n");
        break;
    case NlnsReachable:
        printf("NlnsReachable\n");
        break;
    case NlnsPermanent:
        printf("NlnsPermanent\n");
        break;
    default:
        printf("Unknown: %d\n", State);
        break;
    }
}


void PrintProtocol(IPPROTO protocol)
{
    switch (protocol) {
    case IPPROTO_HOPOPTS:
        wprintf(L"Unspecified\n"); // IPv6 Hop-by-Hop options
        break;
    case IPPROTO_ICMP:
        wprintf(L"ICMP\n");
        break;
    case IPPROTO_IGMP:
        wprintf(L"IGMP\n");
        break;
    case IPPROTO_GGP:
        wprintf(L"GGP\n");
        break;
    case IPPROTO_IPV4:
        wprintf(L"IPV4\n");
        break;
    case IPPROTO_ST:
        wprintf(L"ST\n");
        break;
    case IPPROTO_TCP:
        wprintf(L"IPPROTO_TCP (TCP)\n");
        break;
    case IPPROTO_CBT:
        wprintf(L"CBT\n");
        break;
    case IPPROTO_EGP:
        wprintf(L"EGP\n");
        break;
    case IPPROTO_IGP:
        wprintf(L"IGP\n");
        break;
    case IPPROTO_PUP:
        wprintf(L"PUP\n");
        break;
    case IPPROTO_UDP:
        wprintf(L"IPPROTO_UDP (UDP) \n");
        break;
    case IPPROTO_IDP:
        wprintf(L"IDP\n");
        break;
    case IPPROTO_RDP:
        wprintf(L"RDP\n");
        break;
    case IPPROTO_IPV6:
        wprintf(L"IPV6\n");
        break;
    case IPPROTO_ROUTING:
        wprintf(L"IPv6 Routing header\n");
        break;
    case IPPROTO_FRAGMENT:
        wprintf(L"IPv6 fragmentation header\n");
        break;
    case IPPROTO_ESP:
        wprintf(L"encapsulating security payload\n");
        break;
    case IPPROTO_ICMPV6:
        wprintf(L"ICMPv6\n");
        break;
    case IPPROTO_NONE:
        wprintf(L"IPv6 no next header\n");
        break;
    case IPPROTO_DSTOPTS:
        wprintf(L"IPv6 Destination options\n");
        break;
    case IPPROTO_ND:
        wprintf(L"ND\n");
        break;
    case IPPROTO_ICLFXBM:
        wprintf(L"ICLFXBM\n");
        break;
    case IPPROTO_PIM:
        wprintf(L"PIM\n");
        break;
    case IPPROTO_PGM:
        wprintf(L"PGM\n");
        break;
    case IPPROTO_L2TP:
        wprintf(L"L2TP\n");
        break;
    case IPPROTO_SCTP:
        wprintf(L"SCTP\n");
        break;
    case IPPROTO_RAW:
        wprintf(L"RAW\n");
        break;
    case IPPROTO_MAX:
        wprintf(L"MAX\n");
        break;
    case IPPROTO_RESERVED_RAW:
        wprintf(L"RESERVED_RAW\n");
        break;
    case IPPROTO_RESERVED_IPSEC:
        wprintf(L"RESERVED_IPSEC\n");
        break;
    case IPPROTO_RESERVED_IPSECOFFLOAD:
        wprintf(L"RESERVED_IPSECOFFLOAD\n");
        break;
    case IPPROTO_RESERVED_WNV:
        wprintf(L"RESERVED_WNV\n");
        break;
    case IPPROTO_RESERVED_MAX:
        wprintf(L"RESERVED_MAX\n");
        break;
    default:
        wprintf(L"Other %ld\n", protocol);
        break;
    }
}


void PrintSocketType(int SocketType)
{
    switch (SocketType) {
    case 0:
        printf("Unspecified\n");
        break;
    case SOCK_STREAM:
        printf("SOCK_STREAM (stream)\n");
        break;
    case SOCK_DGRAM:
        printf("SOCK_DGRAM (datagram) \n");
        break;
    case SOCK_RAW:
        printf("SOCK_RAW (raw) \n");
        break;
    case SOCK_RDM:
        printf("SOCK_RDM (reliable message datagram)\n");
        break;
    case SOCK_SEQPACKET:
        printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
        break;
    default:
        printf("Other %d\n", SocketType);
        break;
    }
}


void PrintAddressFamily(_In_ ADDRESS_FAMILY Family)
{
    _ASSERTE(Family < AF_MAX);

    switch (Family) {
    case AF_UNSPEC:
        printf("Unspecified\n");
        break;
    case AF_UNIX:
        printf("local to host (pipes, portals)\n");
        break;
    case AF_INET:
        printf("AF_INET (IPv4)\n");
        break;
    case AF_IMPLINK:
        printf("arpanet imp addresses\n");
        break;
    case AF_PUP:
        printf("pup protocols: e.g. BSP\n");
        break;
    case AF_CHAOS:
        printf("mit CHAOS protocols\n");
        break;
    case AF_NS:
        printf("XEROX NS protocols\n");
        break;
        // case AF_IPX:
        //    printf("IPX protocols: IPX, SPX, etc.\n");
        //    break;
    case AF_ISO:
        printf("ISO protocols\n");
        break;
        // case AF_OSI:
        //    printf("OSI is ISO\n");
        //    break;
    case AF_ECMA:
        printf("european computer manufacturers\n");
        break;
    case AF_DATAKIT:
        printf("datakit protocols\n");
        break;
    case AF_CCITT:
        printf("CCITT protocols, X.25 etc\n");
        break;
    case AF_SNA:
        printf("IBM SNA\n");
        break;
    case AF_DECnet:
        printf("DECnet\n");
        break;
    case AF_DLI:
        printf("Direct data link interface\n");
        break;
    case AF_LAT:
        printf("LAT\n");
        break;
    case AF_HYLINK:
        printf("NSC Hyperchannel\n");
        break;
    case AF_APPLETALK:
        printf("AppleTalk\n");
        break;
    case AF_NETBIOS:
        // printf("NetBios-style addresses\n");
        printf("AF_NETBIOS (NetBIOS)\n");
        break;
    case AF_VOICEVIEW:
        printf("VoiceView\n");
        break;
    case AF_FIREFOX:
        printf("Protocols from Firefox\n");
        break;
    case AF_UNKNOWN1:
        printf("Somebody is using this!\n");
        break;
    case AF_BAN:
        printf("Banyan\n");
        break;
    case AF_ATM:
        printf("Native ATM Services\n");
        break;
    case AF_INET6:
        printf("Internetwork Version 6\n");
        break;
    case AF_CLUSTER:
        printf("Microsoft Wolfpack\n");
        break;
    case AF_12844:
        printf("IEEE 1284.4 WG AF\n");
        break;
    case AF_IRDA:
        printf("IrDA\n");
        break;
    case AF_NETDES:
        printf("Network Designers OSI & gateway\n");
        break;
    case AF_TCNPROCESS:
        printf("TCNPROCESS\n");
        break;
    case AF_TCNMESSAGE:
        printf("TCNMESSAGE\n");
        break;
    case AF_ICLFXBM:
        printf("ICLFXBM\n");
        break;
    case AF_BTH:
        printf("Bluetooth RFCOMM/L2CAP protocols\n");
        break;
    case AF_LINK:
        printf("LINK\n");
        break;
    case AF_HYPERV:
        printf("HYPERV\n");
        break;
    default:
        printf("Other %lu\n", Family);
        break;
    }
}


void PrintProviderNamespace(DWORD NameSpace)
{
    switch (NameSpace) {
    case NS_ALL:
        wprintf(L"NS_ALL\n");
        break;
    case NS_SAP:
        wprintf(L"NS_SAP\n");
        break;
    case NS_NDS:
        wprintf(L"NS_NDS\n");
        break;
    case NS_PEER_BROWSE:
        wprintf(L"NS_PEER_BROWSE\n");
        break;
    case NS_SLP:
        wprintf(L"NS_SLP\n");
        break;
    case NS_DHCP:
        wprintf(L"NS_DHCP\n");
        break;
    case NS_TCPIP_LOCAL:
        wprintf(L"NS_TCPIP_LOCAL\n");
        break;
    case NS_TCPIP_HOSTS:
        wprintf(L"NS_TCPIP_HOSTS\n");
        break;
    case NS_DNS:
        wprintf(L"Domain Name System (NS_DNS)\n");
        break;
    case NS_NETBT:
        wprintf(L"NetBIOS (NS_NETBT)\n");
        break;
    case NS_WINS:
        wprintf(L"Windows Internet Naming Service (NS_WINS)\n");
        break;
    case NS_NLA:
        wprintf(L"Network Location Awareness (NS_NLA)\n");
        break;
    case NS_BTH:
        wprintf(L"Bluetooth (NS_BTH)\n");
        break;
    case NS_LOCALNAME:
        wprintf(L"NS_LOCALNAME\n");
        break;
    case NS_NBP:
        wprintf(L"NS_NBP\n");
        break;
    case NS_MS:
        wprintf(L"NS_MS\n");
        break;
    case NS_STDA:
        wprintf(L"NS_STDA\n");
        break;
    case NS_NTDS:
        wprintf(L"Windows NT Directory Services (NS_NTDS)\n");
        break;
    case NS_EMAIL:
        wprintf(L"Email (NS_EMAIL)\n");
        break;
    case NS_PNRPNAME:
        wprintf(L"Peer-to-peer (NS_PNRPNAME)\n");
        break;
    case NS_PNRPCLOUD:
        wprintf(L"Peer-to-peer collection (NS_PNRPCLOUD)\n");
        break;
    case NS_X500:
        wprintf(L"NS_X500\n");
        break;
    case NS_NIS:
        wprintf(L"NS_NIS\n");
        break;
    case NS_NISPLUS:
        wprintf(L"NS_NISPLUS\n");
        break;
    case NS_WRQ:
        wprintf(L"NS_WRQ\n");
        break;
    case NS_NETDES: /* Network Designers Limited */
        wprintf(L"NS_NETDES\n");
        break;
    default:
        wprintf(L"Other value (%u)\n", NameSpace);
        break;
    }
}


void PrintOffloadStateOfTcpConnection(_In_ TCP_CONNECTION_OFFLOAD_STATE OffloadState)
{
    _ASSERTE(OffloadState < TcpConnectionOffloadStateMax);

    switch (OffloadState) {
    case TcpConnectionOffloadStateInHost:
        printf("Owned by the network stack and not offloaded \n");
        break;
    case TcpConnectionOffloadStateOffloading:
        printf("In the process of being offloaded\n");
        break;
    case TcpConnectionOffloadStateOffloaded:
        printf("Offloaded to the network interface control\n");
        break;
    case TcpConnectionOffloadStateUploading:
        printf("In the process of being uploaded back to the network stack \n");
        break;
    default:
        printf("UNKNOWN Offload state value\n");
        break;
    }
}


void PrintArpType(_In_ MIB_IPNET_TYPE Type)
{
    switch (Type) {
    case MIB_IPNET_TYPE_OTHER:
        printf("Type: OTHER\n");
        break;
    case MIB_IPNET_TYPE_INVALID:
        printf("Type: INVALID\n");
        break;
    case MIB_IPNET_TYPE_DYNAMIC:
        printf("Type: DYNAMIC\n");
        break;
    case MIB_IPNET_TYPE_STATIC:
        printf("Type: STATIC\n");
        break;
    default:
        printf("Type: %d\n", Type);
        break;
    }
}


void PrintRouteType(_In_ DWORD dwForwardType)
{
    switch (dwForwardType) {
    case MIB_IPROUTE_TYPE_OTHER:
        printf("other\n");
        break;
    case MIB_IPROUTE_TYPE_INVALID:
        printf("invalid route\n");
        break;
    case MIB_IPROUTE_TYPE_DIRECT:
        printf("local route where next hop is final destination\n");
        break;
    case MIB_IPROUTE_TYPE_INDIRECT:
        printf("remote route where next hop is not final destination\n");
        break;
    default:
        printf("UNKNOWN Type value\n");
        break;
    }
}


void PrintRoutingProtocols(_In_ DWORD dwForwardProto)
{
    switch (dwForwardProto) {
    case MIB_IPPROTO_OTHER:
        printf("other\n");
        break;
    case MIB_IPPROTO_LOCAL:
        printf("local interface\n");
        break;
    case MIB_IPPROTO_NETMGMT:
        printf("static route set through network management \n");
        break;
    case MIB_IPPROTO_ICMP:
        printf("result of ICMP redirect\n");
        break;
    case MIB_IPPROTO_EGP:
        printf("Exterior Gateway Protocol (EGP)\n");
        break;
    case MIB_IPPROTO_GGP:
        printf("Gateway-to-Gateway Protocol (GGP)\n");
        break;
    case MIB_IPPROTO_HELLO:
        printf("Hello protocol\n");
        break;
    case MIB_IPPROTO_RIP:
        printf("Routing Information Protocol (RIP)\n");
        break;
    case MIB_IPPROTO_IS_IS:
        printf("Intermediate System-to-Intermediate System (IS-IS) protocol\n");
        break;
    case MIB_IPPROTO_ES_IS:
        printf("End System-to-Intermediate System (ES-IS) protocol\n");
        break;
    case MIB_IPPROTO_CISCO:
        printf("Cisco Interior Gateway Routing Protocol (IGRP)\n");
        break;
    case MIB_IPPROTO_BBN:
        printf("BBN Internet Gateway Protocol (IGP) using SPF\n");
        break;
    case MIB_IPPROTO_OSPF:
        printf("Open Shortest Path First (OSPF) protocol\n");
        break;
    case MIB_IPPROTO_BGP:
        printf("Border Gateway Protocol (BGP)\n");
        break;
    case RouteProtocolIdpr:
        printf("Idpr\n");
        break;
    case RouteProtocolEigrp:
        printf("Eigrp\n");
        break;
    case RouteProtocolDvmrp:
        printf("Dvmrp\n");
        break;
    case RouteProtocolRpl:
        printf("Rpl\n");
        break;
    case RouteProtocolDhcp:
        printf("Dhcp\n");
        break;
    case MIB_IPPROTO_NT_AUTOSTATIC:
        printf("special Windows auto static route\n");
        break;
    case MIB_IPPROTO_NT_STATIC:
        printf("special Windows static route\n");
        break;
    case MIB_IPPROTO_NT_STATIC_NON_DOD:
        printf("special Windows static route not based on Internet standards\n");
        break;
    default:
        printf("UNKNOWN Proto value\n");
        break;
    }
}


void PrintInterfaceOperationalStatus(_In_ INTERNAL_IF_OPER_STATUS dwOperStatus)
{
    switch (dwOperStatus) {
    case IF_OPER_STATUS_NON_OPERATIONAL:
        printf("Non Operational\n");
        break;
    case IF_OPER_STATUS_UNREACHABLE:
        printf("Unreachable\n");
        break;
    case IF_OPER_STATUS_DISCONNECTED:
        printf("Disconnected\n");
        break;
    case IF_OPER_STATUS_CONNECTING:
        printf("Connecting\n");
        break;
    case IF_OPER_STATUS_CONNECTED:
        printf("Connected\n");
        break;
    case IF_OPER_STATUS_OPERATIONAL:
        printf("Operational\n");
        break;
    default:
        printf("Unknown status %d\n", dwOperStatus);
        break;
    }
}


void PrintAdapterType(INT Type)
/*


注释：这个是精简版，全面的应该用PrintInterfaceType。
*/
{
    switch (Type) {
    case MIB_IF_TYPE_OTHER:
        printf("Other\n");
        break;
    case MIB_IF_TYPE_ETHERNET:
        printf("Ethernet\n");
        break;
    case MIB_IF_TYPE_TOKENRING:
        printf("Token Ring\n");
        break;
    case MIB_IF_TYPE_FDDI:
        printf("FDDI\n");
        break;
    case MIB_IF_TYPE_PPP:
        printf("PPP\n");
        break;
    case MIB_IF_TYPE_LOOPBACK:
        printf("Lookback\n");
        break;
    case MIB_IF_TYPE_SLIP:
        printf("Slip\n");
        break;
    case IF_TYPE_IEEE80211: // An IEEE 802.11 wireless network interface.
        /*
        Note  This adapter type is returned on Windows Vista and later.
        On Windows Server 2003 and Windows XP ,
        an IEEE 802.11 wireless network interface returns an adapter type of MIB_IF_TYPE_ETHERNET.

        https://docs.microsoft.com/en-us/windows/win32/api/iptypes/ns-iptypes-ip_adapter_info
        */
        printf("Radio spread spectrum\n");
        break;
    default:
        printf("Unknown type %d\n", Type);
        break;
    }
}


void PrintPrefixOrigin(NL_PREFIX_ORIGIN PrefixOrigin)
{
    switch (PrefixOrigin) {
    case IpPrefixOriginOther:
        wprintf(L"IpPrefixOriginOther\n");
        break;
    case IpPrefixOriginManual:
        wprintf(L"IpPrefixOriginManual\n");
        break;
    case IpPrefixOriginWellKnown:
        wprintf(L"IpPrefixOriginWellKnown\n");
        break;
    case IpPrefixOriginDhcp:
        wprintf(L"IpPrefixOriginDhcp\n");
        break;
    case IpPrefixOriginRouterAdvertisement:
        wprintf(L"IpPrefixOriginRouterAdvertisement\n");
        break;
    case IpPrefixOriginUnchanged:
        wprintf(L"IpPrefixOriginUnchanged\n");
        break;
    default:
        wprintf(L"Unknown: %d\n", PrefixOrigin);
        break;
    }
}


void PrintSuffixOrigin(NL_SUFFIX_ORIGIN SuffixOrigin)
{
    switch (SuffixOrigin) {
    case IpSuffixOriginOther:
        wprintf(L"IpSuffixOriginOther\n");
        break;
    case IpSuffixOriginManual:
        wprintf(L"IpSuffixOriginManual\n");
        break;
    case IpSuffixOriginWellKnown:
        wprintf(L"IpSuffixOriginWellKnown\n");
        break;
    case IpSuffixOriginDhcp:
        wprintf(L"IpSuffixOriginDhcp\n");
        break;
    case IpSuffixOriginLinkLayerAddress:
        wprintf(L"IpSuffixOriginLinkLayerAddress\n");
        break;
    case IpSuffixOriginRandom:
        wprintf(L"IpSuffixOriginRandom\n");
        break;
    case IpSuffixOriginUnchanged:
        wprintf(L"IpSuffixOriginUnchanged\n");
        break;
    default:
        wprintf(L"Unknown: %d\n", SuffixOrigin);
        break;
    }
}


void PrintDadState(NL_DAD_STATE DadState)
{
    switch (DadState) {
    case IpDadStateInvalid:
        wprintf(L"IpDadStateInvalid\n");
        break;
    case IpDadStateTentative:
        wprintf(L"IpDadStateTentative\n");
        break;
    case IpDadStateDuplicate:
        wprintf(L"IpDadStateDuplicate\n");
        break;
    case IpDadStateDeprecated:
        wprintf(L"IpDadStateDeprecated\n");
        break;
    case IpDadStatePreferred:
        wprintf(L"IpDadStatePreferred\n");
        break;
    default:
        wprintf(L"Unknown: %d\n", DadState);
        break;
    }
}


void PrintNodeType(UINT NodeType)
{
    switch (NodeType) {
    case BROADCAST_NODETYPE:
        printf("Broadcast node\n");
        break;
    case PEER_TO_PEER_NODETYPE:
        printf("Peer to Peer node\n");
        break;
    case MIXED_NODETYPE:
        printf("Mixed node\n");
        break;
    case HYBRID_NODETYPE:
        printf("Hybrid node\n");
        break;
    default:
        printf("Unknown node type %0lx\n", NodeType);
        break;
    }
}
