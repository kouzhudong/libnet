/*
此文主要实现iphelapi.h和netioapi.h里的一些信息获取和枚举的函数的功能例子。

此文的内容太多，已经分离出notify.h，Adapter.h，tcp.h，udp.h内容。
*/

#pragma once

#include "pch.h"
#include <netiodef.h>


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


DLLEXPORT
BOOL WINAPI GetMacByGatewayIPv6(const char * ipv6, PDL_EUI48 mac);


EXTERN_C_END
