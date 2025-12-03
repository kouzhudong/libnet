/*
本文只要收集关于网卡（适配器的代码）。

网卡（适配器的代码）是一个基本的概念，不可忽视的知识点，必须把握。
*/

/*
附上我很久以前写的汇编代码。
保留纪念吧！

;made by correy
;rc me.rc
;ml /coff test.asm /link /subsystem:windows me.res
;心得：不要高谈阔论，唯行之之难矣！
.386
.model flat,stdcall
option casemap:none
include windows.inc
include kernel32.inc
include user32.inc
include iphlpapi.inc
includelib user32.lib
includelib kernel32.lib
includelib iphlpapi.lib

.data?
buffer db 9*280h dup (?);最多显示9个，如想得到恰当的大小请先一错误运行一下GetAdaptersInfo。
buffer2 db 256 dup (?)

.data
correy db "made by correy",0
x dd 9*280h
formatmac db "%02X-%02X-%02X-%02X-%02X-%02X",13,10,0

x1 dd ?
x2 dd ?
x3 dd ?
x4 dd ?
x5 dd ?
x6 dd ?

.code

start:

invoke GetAdaptersInfo,addr buffer,addr x
;198h=408

lea esi,buffer;buffer的值不可以改变。

again:
mov eax,0
mov al,byte ptr [esi+404]
mov x1,eax

mov eax,0
mov al,byte ptr [esi+405]
mov x2,eax

mov eax,0
mov al,byte ptr [esi+406]
mov x3,eax

mov eax,0
mov al,byte ptr [esi+407]
mov x4,eax

mov eax,0
mov al,byte ptr [esi+408]
mov x5,eax

mov eax,0
mov al,byte ptr [esi+409]
mov x6,eax

mov eax,dword ptr [esi+8];mac的描述。
.if  eax!=0;必须用寄存器等，不能用变量名或内存。
;也可以辨别x1==0 &&  x2==0 && x3==0 && x4==0 && x5==0 && x6==0等。
;要显示别的也可以呀！如名字，ip，网关等。
invoke   wsprintf,addr buffer2,addr formatmac,x1,x2,x3,x4,x5,x6
invoke MessageBox,0,addr buffer2,addr correy,0;我显示的好像少一个。

add esi,280h

jmp again
.endif

invoke ExitProcess,NULL
end start
;made at 2011.01.31
*/

#pragma once
#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


DLLEXPORT
int WINAPI EnumAdaptersAddressesInfo(_In_ ADDRESS_FAMILY Family);

DLLEXPORT
int WINAPI EnumAdaptersInfo();

DLLEXPORT
int WINAPI EnumInterfaceInfo();

DLLEXPORT
int WINAPI GetGatewayByIPv4(const char * IPv4, char * Gateway);

DLLEXPORT
int WINAPI GetGatewayByIPv6(const char * IPv6, char * Gateway);

DLLEXPORT
int WINAPI GetGatewayMacByIPv6(const char * IPv6, PDL_EUI48 GatewayMac);


EXTERN_C_END
