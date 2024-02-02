#pragma once

#include "pch.h"


typedef struct _ASTAT_ {
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff[30];
}ASTAT, * PASTAT;


// Set LANANUM and LOCALNAME as appropriate for your system
#define LANANUM     0
#define LOCALNAME   "MAKEUNIQUE"
#define NBCheck(x)  if (NRC_GOODRET != x.ncb_retcode) { \
                        printf("Line %d: Got 0x%x from NetBios()\n", \
                               __LINE__, x.ncb_retcode); \
                    }


void MakeNetbiosName(char *, LPCSTR);
BOOL NBAddName(UCHAR nLana, LPCSTR szName);
BOOL NBReset(UCHAR nLana, UCHAR nSessions, UCHAR nNames);
BOOL NBListNames(int, LPCSTR);
BOOL NBAdapterStatus(int nLana, PVOID pBuffer, int cbBuffer, LPCSTR szName);


void ncbastat(void);
void NBListNames();
