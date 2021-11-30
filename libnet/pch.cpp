#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


USHORT checksum(USHORT * buffer, int size)
//摘自Windows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c
// Description:
//    This function calculates the 16-bit one's complement sum for the supplied buffer.
{
    unsigned long cksum = 0;

    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }

    // If the buffer was not a multiple of 16-bits, add the last byte
    if (size) {
        cksum += *(UCHAR *)buffer;
    }

    // Add the low order 16-bits to the high order 16-bits
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    // Take the 1's complement
    return (USHORT)(~cksum);
}


void GetTimeString(LARGE_INTEGER UtcTime, _Out_ LPWSTR TimeString)
/*
把UTC时间转换为本地时间的字符串形式。
*/
{
    FILETIME ft;
    ft.dwHighDateTime = UtcTime.HighPart;
    ft.dwLowDateTime = UtcTime.LowPart;

    FILETIME lft;
    BOOL B = FileTimeToLocalFileTime(&ft, &lft);
    _ASSERTE(B);

    SYSTEMTIME st;
    //GetLocalTime(&st);
    B = FileTimeToSystemTime(&lft, &st);
    _ASSERTE(B);

    //SystemTimeToTzSpecificLocalTime

    //格式：2016-07-11 17:35:54 
    wsprintfW(TimeString, L"%04d-%02d-%02d %02d:%02d:%02d.\r\n",
              st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}


void PrintTcpConnectionState(_In_ DWORD dwState)
//Print The state of the TCP connection
{
    switch (dwState) {
    case MIB_TCP_STATE_CLOSED:
        printf("CLOSED\n");
        break;
    case MIB_TCP_STATE_LISTEN:
        printf("LISTEN\n");
        break;
    case MIB_TCP_STATE_SYN_SENT:
        printf("SYN-SENT\n");
        break;
    case MIB_TCP_STATE_SYN_RCVD:
        printf("SYN-RECEIVED\n");
        break;
    case MIB_TCP_STATE_ESTAB:
        printf("ESTABLISHED\n");
        break;
    case MIB_TCP_STATE_FIN_WAIT1:
        printf("FIN-WAIT-1\n");
        break;
    case MIB_TCP_STATE_FIN_WAIT2:
        printf("FIN-WAIT-2 \n");
        break;
    case MIB_TCP_STATE_CLOSE_WAIT:
        printf("CLOSE-WAIT\n");
        break;
    case MIB_TCP_STATE_CLOSING:
        printf("CLOSING\n");
        break;
    case MIB_TCP_STATE_LAST_ACK:
        printf("LAST-ACK\n");
        break;
    case MIB_TCP_STATE_TIME_WAIT:
        printf("TIME-WAIT\n");
        break;
    case MIB_TCP_STATE_DELETE_TCB:
        printf("DELETE-TCB\n");
        break;
    default:
        wprintf(L"UNKNOWN dwState value: %d\n", dwState);
        break;
    }
}
