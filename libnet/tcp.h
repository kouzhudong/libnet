#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


DLLEXPORT
int WINAPI EnumTcpTable();

DLLEXPORT
int WINAPI EnumTcpTable2();

DLLEXPORT
int WINAPI EnumTcp6Table();

DLLEXPORT
int WINAPI EnumTcp6Table2();

DLLEXPORT
int WINAPI EnumTcpStatistics();

DLLEXPORT
int WINAPI EnumExtendedTcpTable(_In_ ULONG ulAf, _In_ TCP_TABLE_CLASS TableClass);

DLLEXPORT
int WINAPI RunEstats();


EXTERN_C_END
