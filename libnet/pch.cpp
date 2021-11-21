#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


USHORT checksum(USHORT * buffer, int size)
//Õª×ÔWindows-classic-samples\Samples\Win7Samples\netds\winsock\iphdrinc\rawudp.c
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
