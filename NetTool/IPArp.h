//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2008-2009
//  File:       iparp.h

#pragma once

#include "pch.h"

#ifndef WIN_SUCCESS
#define WIN_SUCCESS(x) ((x) == NO_ERROR)
#endif //WIN_SUCCESS

#define IPADDR_BUF_SIZE 128
#define IPTYPE_BUF_SIZE 128
#define PHYSADDR_BUF_SIZE 256

// Forward declarations
void DoGetIpNetTable();
void DoSetIpNetEntry(char* pszDottedInetAddr, char* pszPhysAddr, char* pszInterface = NULL);
void DoDeleteIpNetEntry(char* pszDottedInetAddr, char* pszInterface = NULL);
DWORD MyGetIpNetTable(PMIB_IPNETTABLE& pIpNetTable, bool fOrder = TRUE);
void PrintIpNetTable(PMIB_IPNETTABLE pIpNetTable);
int StringToPhysAddr(char* szInEther,  char* szOutEther);
DWORD MyGetIpAddrTable(PMIB_IPADDRTABLE& pIpAddrTable, bool fOrder = TRUE);

void _cdecl IpArp(int argc, char ** argv);
