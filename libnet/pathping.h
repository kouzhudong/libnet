//
// Local Messages for ping
// Message range: 10000+
//

//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: PATHPING_BRKT_IP_ADDRESS
//
// MessageText:
//
// [%1!hs!] %0
//
#define PATHPING_BRKT_IP_ADDRESS         0x00002710L

//
// MessageId: PATHPING_TIME
//
// MessageText:
//
// %1!4lu! ms  %0
//
#define PATHPING_TIME                    0x00002711L

//
// MessageId: PATHPING_TIME_10MS
//
// MessageText:
//
//   <1 ms  %0
//
#define PATHPING_TIME_10MS               0x00002712L

//
// MessageId: PATHPING_USAGE
//
// MessageText:
//
// 
// Usage: pathping [-g host-list] [-h maximum_hops] [-i address] [-n] 
//                 [-p period] [-q num_queries] [-w timeout] 
//                 [-4] [-6] target_name
// 
// Options:
//     -g host-list     Loose source route along host-list.
//     -h maximum_hops  Maximum number of hops to search for target.
//     -i address       Use the specified source address. 
//     -n               Do not resolve addresses to hostnames.
//     -p period        Wait period milliseconds between pings.
//     -q num_queries   Number of queries per hop.
//     -w timeout       Wait timeout milliseconds for each reply.
//     -4               Force using IPv4.
//     -6               Force using IPv6.
//
#define PATHPING_USAGE                   0x00002713L

//
// MessageId: PATHPING_MESSAGE_1
//
// MessageText:
//
// Unable to resolve target system name %1!hs!.
//
#define PATHPING_MESSAGE_1               0x00002714L

//
// MessageId: PATHPING_MESSAGE_2
//
// MessageText:
//
// Unable to contact IP driver. Error code %1!d!.
//
#define PATHPING_MESSAGE_2               0x00002715L

//
// MessageId: PATHPING_HEADER1
//
// MessageText:
//
// 
// Tracing route to %1!hs! [%2!hs!]
// over a maximum of %3!u! hops:
//
#define PATHPING_HEADER1                 0x00002716L

//
// MessageId: PATHPING_MESSAGE_4
//
// MessageText:
//
// %1!3lu!  %0
//
#define PATHPING_MESSAGE_4               0x00002717L

//
// MessageId: PATHPING_MESSAGE_5
//
// MessageText:
//
// No information available.
//
#define PATHPING_MESSAGE_5               0x00002718L

//
// MessageId: PATHPING_MESSAGE_6
//
// MessageText:
//
//  reports: %0
//
#define PATHPING_MESSAGE_6               0x00002719L

//
// MessageId: PATHPING_MESSAGE_7
//
// MessageText:
//
// Transmit error: code %1!lu!.
//
#define PATHPING_MESSAGE_7               0x0000271AL

//
// MessageId: PATHPING_MESSAGE_8
//
// MessageText:
//
// 
// Trace complete.
//
#define PATHPING_MESSAGE_8               0x0000271BL

//
// MessageId: PATHPING_BUF_TOO_SMALL
//
// MessageText:
//
// General failure.
//
#define PATHPING_BUF_TOO_SMALL           0x0000271FL

//
// MessageId: PATHPING_DEST_NET_UNREACHABLE
//
// MessageText:
//
// Destination net unreachable.
//
#define PATHPING_DEST_NET_UNREACHABLE    0x00002720L

//
// MessageId: PATHPING_DEST_HOST_UNREACHABLE
//
// MessageText:
//
// Destination host unreachable.
//
#define PATHPING_DEST_HOST_UNREACHABLE   0x00002721L

//
// MessageId: PATHPING_DEST_PROT_UNREACHABLE
//
// MessageText:
//
// Destination protocol unreachable.
//
#define PATHPING_DEST_PROT_UNREACHABLE   0x00002722L

//
// MessageId: PATHPING_DEST_PORT_UNREACHABLE
//
// MessageText:
//
// Destination protocol unreachable.
//
#define PATHPING_DEST_PORT_UNREACHABLE   0x00002723L

//
// MessageId: PATHPING_NO_RESOURCES
//
// MessageText:
//
// No resources.
//
#define PATHPING_NO_RESOURCES            0x00002724L

//
// MessageId: PATHPING_BAD_OPTION
//
// MessageText:
//
// One of the IP options is invalid.
//
#define PATHPING_BAD_OPTION              0x00002725L

//
// MessageId: PATHPING_HW_ERROR
//
// MessageText:
//
// Hardware error.
//
#define PATHPING_HW_ERROR                0x00002726L

//
// MessageId: PATHPING_PACKET_TOO_BIG
//
// MessageText:
//
// Packet needs to be fragmented but DF flag is set.
//
#define PATHPING_PACKET_TOO_BIG          0x00002727L

//
// MessageId: PATHPING_REQ_TIMED_OUT
//
// MessageText:
//
// Request timed out.
//
#define PATHPING_REQ_TIMED_OUT           0x00002728L

//
// MessageId: PATHPING_BAD_REQ
//
// MessageText:
//
// General failure.
//
#define PATHPING_BAD_REQ                 0x00002729L

//
// MessageId: PATHPING_BAD_ROUTE
//
// MessageText:
//
// Invalid source route specified.
//
#define PATHPING_BAD_ROUTE               0x0000272AL

//
// MessageId: PATHPING_TTL_EXPIRED_TRANSIT
//
// MessageText:
//
// TTL expired in transit.
//
#define PATHPING_TTL_EXPIRED_TRANSIT     0x0000272BL

//
// MessageId: PATHPING_TTL_EXPIRED_REASSEM
//
// MessageText:
//
// TTL expired during reassembly.
//
#define PATHPING_TTL_EXPIRED_REASSEM     0x0000272CL

//
// MessageId: PATHPING_PARAM_PROBLEM
//
// MessageText:
//
// IP parameter problem.
//
#define PATHPING_PARAM_PROBLEM           0x0000272DL

//
// MessageId: PATHPING_SOURCE_QUENCH
//
// MessageText:
//
// Source quench received.
//
#define PATHPING_SOURCE_QUENCH           0x0000272EL

//
// MessageId: PATHPING_OPTION_TOO_BIG
//
// MessageText:
//
// The specified option is too large.
//
#define PATHPING_OPTION_TOO_BIG          0x0000272FL

//
// MessageId: PATHPING_BAD_DESTINATION
//
// MessageText:
//
// The destination specified is not valid.
//
#define PATHPING_BAD_DESTINATION         0x00002730L

//
// MessageId: PATHPING_GENERAL_FAILURE
//
// MessageText:
//
// General failure.
//
#define PATHPING_GENERAL_FAILURE         0x00002731L

//
// MessageId: PATHPING_WSASTARTUP_FAILED
//
// MessageText:
//
// Unable to initialize the Windows Sockets interface, error code %1!d!.
//
#define PATHPING_WSASTARTUP_FAILED       0x00002732L

//
// MessageId: PATHPING_HEADER2
//
// MessageText:
//
// 
// Tracing route to %1!hs! over a maximum of %2!u! hops
// 
//
#define PATHPING_HEADER2                 0x00002733L

//
// MessageId: PATHPING_NO_OPTION_VALUE
//
// MessageText:
//
// A value must be supplied for option %1!hs!.
//
#define PATHPING_NO_OPTION_VALUE         0x00002734L

//
// MessageId: PATHPING_BAD_OPTION_VALUE
//
// MessageText:
//
// Bad value for option %1!hs!.
//
#define PATHPING_BAD_OPTION_VALUE        0x00002735L

//
// MessageId: PATHPING_BAD_ROUTE_ADDRESS
//
// MessageText:
//
// %1!hs! is not a valid source route address.
//
#define PATHPING_BAD_ROUTE_ADDRESS       0x00002736L

//
// MessageId: PATHPING_TOO_MANY_OPTIONS
//
// MessageText:
//
// Too many options have been specified.
//
#define PATHPING_TOO_MANY_OPTIONS        0x00002737L

//
// MessageId: PATHPING_INVALID_SWITCH
//
// MessageText:
//
// %1!hs! is not a valid command option.
//
#define PATHPING_INVALID_SWITCH          0x00002738L

//
// MessageId: PATHPING_NO_ADDRESS
//
// MessageText:
//
// A target name or address must be specified.
//
#define PATHPING_NO_ADDRESS              0x00002739L

//
// MessageId: PATHPING_TARGET_NAME
//
// MessageText:
//
// %1!hs! %0
//
#define PATHPING_TARGET_NAME             0x0000273AL

//
// MessageId: PATHPING_NO_REPLY_TIME
//
// MessageText:
//
//    *     %0
//
#define PATHPING_NO_REPLY_TIME           0x0000273BL

//
// MessageId: PATHPING_CR
//
// MessageText:
//
// 
//
#define PATHPING_CR                      0x0000273CL

//
// MessageId: PATHPING_IP_ADDRESS
//
// MessageText:
//
// %1!hs! %0
//
#define PATHPING_IP_ADDRESS              0x0000273DL

//
// MessageId: PATHPING_STAT_HEADER
//
// MessageText:
//
//             Source to Here   This Node/Link
// Hop  RTT    Lost/Sent = Pct  Lost/Sent = Pct  Address
//   0                                           %0
//
#define PATHPING_STAT_HEADER             0x0000273EL

//
// MessageId: PATHPING_STAT_LINK
//
// MessageText:
//
//                              %1!4d!/%2!4d! =%3!3d!%%   |
//
#define PATHPING_STAT_LINK               0x0000273FL

//
// MessageId: PATHPING_STAT_LOSS
//
// MessageText:
//
// %1!4d!/%2!4d! =%3!3d!%%  %0
//
#define PATHPING_STAT_LOSS               0x00002740L

//
// MessageId: PATHPING_HOP_RTT
//
// MessageText:
//
// %1!3d! %2!4d!ms  %0
//
#define PATHPING_HOP_RTT                 0x00002741L

//
// MessageId: PATHPING_HOP_NO_RTT
//
// MessageText:
//
// %1!3d!  ---    %0
//
#define PATHPING_HOP_NO_RTT              0x00002742L

//
// MessageId: PATHPING_COMPUTING
//
// MessageText:
//
// 
// Computing statistics for %1!d! seconds...
//
#define PATHPING_COMPUTING               0x00002743L

//
// MessageId: PATHPING_NEGOTIATING_IPSEC
//
// MessageText:
//
// Negotiating IP Security.
//
#define PATHPING_NEGOTIATING_IPSEC       0x00002744L

//
// MessageId: PATHPING_WSACREATEEVENT_FAILED
//
// MessageText:
//
// Cound not create an Event object. Error code is %1!d!.
//
#define PATHPING_WSACREATEEVENT_FAILED   0x00002745L

//
// MessageId: PATHPING_WSASOCKET_FAILED
//
// MessageText:
//
// Cound not create a socket object. Error code is %1!d!.
//
#define PATHPING_WSASOCKET_FAILED        0x00002746L

//
// MessageId: PATHPING_WSAEVENTSELECT_FAILED
//
// MessageText:
//
// Cound not associate the event object with network events. Error code is %1!d!.
//
#define PATHPING_WSAEVENTSELECT_FAILED   0x00002747L

//
// MessageId: PATHPING_SENDTO_FAILED
//
// MessageText:
//
// Could not send the packet. Error code is %1!d!.
//
#define PATHPING_SENDTO_FAILED           0x00002748L

//
// MessageId: PATHPING_RECVFROM_FAILED
//
// MessageText:
//
// Could not receive a packet. Error code is %1!d!.
//
#define PATHPING_RECVFROM_FAILED         0x00002749L

//
// MessageId: PATHPING_FAMILY
//
// MessageText:
//
// 
// The option %1!s! is only supported for %2!hs!.
// 
//
#define PATHPING_FAMILY                  0x0000275CL

