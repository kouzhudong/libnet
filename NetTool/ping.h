// From Network Programming for Microsoft Windows, Second Edition by 
// Anthony Jones and James Ohlund.  
// Copyright 2002.   Reproduced by permission of Microsoft Press.  
// All rights reserved.
//
//
// Sample: IPv4 and IPv6 Ping Sample
//
// Files:
//    iphdr.h       - IPv4 and IPv6 packet header definitions
//    ping.cpp      - this file
//    resolve.cpp   - Common name resolution routine
//    resolve.h     - Header file for common name resolution routines
//
// Description:
//    This sample illustrates how to use raw sockets to send ICMP
//    echo requests and receive their response. This sample performs
//    both IPv4 and IPv6 ICMP echo requests. When using raw sockets,
//    the protocol value supplied to the socket API is used as the
//    protocol field (or next header field) of the IP packet. Then
//    as a part of the data submitted to sendto, we include both
//    the ICMP request and data.
//
//    For IPv4 the IP record route option is supported via the 
//    IP_OPTIONS socket option.
//
// Compile:
//      cl -o ping.exe ping.cpp resolve.cpp ws2_32.lib
//
// Command Line Options/Parameters:
//     ping.exe [-a 4|6] [-i ttl] [-l datasize] [-r] [host]
//     
//     -a       Address family (IPv4 or IPv6)
//     -i ttl   TTL value to set on socket
//     -l size  Amount of data to send as part of the ICMP request
//     -r       Use IPv4 record route
//     host     Hostname or literal address

#pragma once
 
#include "pch.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _IA64_
#pragma warning (disable: 4267)
#endif

#define DEFAULT_DATA_SIZE      32       // default data size
#define DEFAULT_SEND_COUNT     4        // number of ICMP requests to send
#define DEFAULT_RECV_TIMEOUT   6000     // six second
#define DEFAULT_TTL            128
#define MAX_RECV_BUF_LEN       0xFFFF   // Max incoming packet size.

//////////////////////////////////////////////////////////////////////////////////////////////////
 
int ping(int argc, char ** argv);