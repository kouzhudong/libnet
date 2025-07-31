#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


int _cdecl special_ip();


EXTERN_C_START


DLLEXPORT
void WINAPI EnumIPv4ByMask(const char * ipv4, const char * mask);

DLLEXPORT
void WINAPI EnumIPv4ByMasks(const char * ipv4, BYTE mask);

DLLEXPORT
int WINAPI AddIPv4Address(int argc, char ** argv);

DLLEXPORT
int WINAPI SetStaticIPv4(PCWSTR Ipv4, PCWSTR SubnetMask);


EXTERN_C_END
