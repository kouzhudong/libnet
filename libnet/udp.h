#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
int WINAPI EnumUdpTable();

__declspec(dllexport)
int WINAPI EnumUdp6Table();

__declspec(dllexport)
int WINAPI EnumExtendedUdpTable4();

__declspec(dllexport)
int WINAPI EnumExtendedUdpTable6();


EXTERN_C_END
