#pragma once

#include "framework.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


//https://msdn.microsoft.com/en-us/library/windows/desktop/ms738518%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
//其实：IPV4的只需This buffer should be large enough to hold at least 16 characters.
#define MAX_ADDRESS_STRING_LENGTH   64


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3


//////////////////////////////////////////////////////////////////////////////////////////////////


USHORT checksum(USHORT * buffer, int size);
void GetTimeString(LARGE_INTEGER UtcTime, _Out_ LPWSTR TimeString);
void PrintTcpConnectionState(_In_ DWORD dwState);
void PrintInterfaceType(_In_ WORD Type);
void PrintNeighborState(_In_ NL_NEIGHBOR_STATE State);
