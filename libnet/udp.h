#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


//Õª×Ô£ºWindows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\iphdr.h
typedef struct udp_hdr {// Define the UDP header 
    unsigned short src_portno;       // Source port no.
    unsigned short dst_portno;       // Dest. port no.
    unsigned short udp_length;       // Udp packet length
    unsigned short udp_checksum;     // Udp checksum
} UDP_HDR, * PUDP_HDR;


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


DLLEXPORT
int WINAPI EnumUdpTable();

DLLEXPORT
int WINAPI EnumUdp6Table();

DLLEXPORT
int WINAPI EnumExtendedUdpTable4();

DLLEXPORT
int WINAPI EnumExtendedUdpTable6();


EXTERN_C_END
