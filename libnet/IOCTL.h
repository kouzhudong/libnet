/*
本文主要的目的是演示：
1.getsockopt and setsockopt functions。WSPSetSockOpt, WSPGetSockOpt.
2.WSAIoctl or WSPIoctl function, 还有ioctlsocket。
3.NtDeviceIoControlFile or DeviceIoControl function。
的用法和功能。
*/

/*
Socket Options
2018/05/31
2 分钟可看完

This section describes Winsock Socket Options for various editions of Windows operating systems. 
Use the getsockopt and setsockopt functions for more getting and setting socket options. 
To enumerate protocols and discover supported properties for each installed protocol, use the WSAEnumProtocols function.

Some socket options require more explanation than these tables can convey; 
such options contain links to additional pages.

IPPROTO_IP
Socket options applicable at the IPv4 level.
For more information, see the IPPROTO_IP Socket Options.

IPPROTO_IPV6
Socket options applicable at the IPv6 level. 
For more information, see the IPPROTO_IPV6 Socket Options.

IPPROTO_RM
Socket options applicable at the reliable multicast level. 
For more information, see the IPPROTO_RM Socket Options.

IPPROTO_TCP
Socket options applicable at the TCP level. 
For more information, see the IPPROTO_TCP Socket Options.

IPPROTO_UDP
Socket options applicable at the UDP level. 
For more information, see the IPPROTO_UDP Socket Options.

NSPROTO_IPX
Socket options applicable at the IPX level. 
For more information, see the NSPROTO_IPX Socket Options.

SOL_APPLETALK
Socket options applicable at the AppleTalk level. 
For more information, see the SOL_APPLETALK Socket Options.

SOL_IRLMP
Socket options applicable at the InfraRed Link Management Protocol level. 
For more information, see the SOL_IRLMP Socket Options.

SOL_SOCKET
Socket options applicable at the socket level. 
For more information, see the SOL_SOCKET Socket Options.

Remarks
All SO_* socket options apply equally to IPv4 and IPv6 (except SO_BROADCAST, since broadcast is not implemented in IPv6).

https://docs.microsoft.com/zh-cn/windows/win32/winsock/socket-options?redirectedfrom=MSDN
*/


/*
Winsock IOCTLs
05/31/2018

This section describes Winsock Socket input/output controls (IOCTLs) for various editions of Windows operating systems. 
Use the WSAIoctl or WSPIoctl function to issue a Winsock IOCTL to control the mode of a socket,
the transport protocol, or the communications subsystem.

https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-ioctls
*/

#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


#define FILE_OPEN_IF                    0x00000003
#define FILE_SYNCHRONOUS_IO_NONALERT    0x00000020
#define OBJ_CASE_INSENSITIVE            0x00000040L

typedef NTSTATUS(NTAPI * P_NT_CREATE_FILE)(
    OUT PHANDLE              FileHandle,
    IN  ACCESS_MASK          DesiredAccess,
    IN  POBJECT_ATTRIBUTES   ObjectAttributes,
    OUT PIO_STATUS_BLOCK     IoStatusBlock,
    IN  PLARGE_INTEGER       AllocationSize OPTIONAL,
    IN  ULONG                FileAttributes,
    IN  ULONG                ShareAccess,
    IN  ULONG                CreateDisposition,
    IN  ULONG                CreateOptions,
    IN  PVOID                EaBuffer OPTIONAL,
    IN  ULONG                EaLength);


//////////////////////////////////////////////////////////////////////////////////////////////////


//C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\km\tdistat.h
#define TDI_SUCCESS             0           // Success


//////////////////////////////////////////////////////////////////////////////////////////////////


extern P_NT_CREATE_FILE pNtCreateFile;


EXTERN_C_START


__declspec(dllexport)
int WINAPI EnumEntityArray();


EXTERN_C_END
