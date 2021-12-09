#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
int WINAPI EnumTcpTable();

__declspec(dllexport)
int WINAPI EnumTcpTable2();

__declspec(dllexport)
int WINAPI EnumTcp6Table();

__declspec(dllexport)
int WINAPI EnumTcp6Table2();

__declspec(dllexport)
int WINAPI EnumTcpStatistics();

__declspec(dllexport)
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass);

__declspec(dllexport)
int WINAPI RunEstats();


EXTERN_C_END
