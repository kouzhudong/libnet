#pragma once

#include "..\inc\libnet.h"
#include "pch.h"

class init
{

};

typedef
LONG
(NTAPI *
 RtlEthernetStringToAddressW_Fn)(
     _In_ PCWSTR S,
     _Out_ LPCWSTR * Terminator,
     _Out_ DL_EUI48 * Addr
     );

typedef
PWSTR
(NTAPI *
 RtlEthernetAddressToStringW_Fn)(
     _In_ const DL_EUI48 * Addr,
     _Out_writes_(18) PWSTR S
     );

extern RtlEthernetStringToAddressW_Fn EthernetStringToAddressW;
extern RtlEthernetAddressToStringW_Fn EthernetAddressToStringW;

void init();
