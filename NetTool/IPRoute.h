#pragma once

#include "pch.h"

void DoGetIpForwardTable();
void DoSetIpForwardEntry(char* pszDest, char* pszNetMask, char* pszGateway, char* pszInterface, DWORD dwMetric = 1);
void DoDeleteIpForwardEntry(char* pszDest);
static bool InterfaceIdxToInterfaceIp(PMIB_IPADDRTABLE pIpAddrTable, DWORD dwIndex, char str[]);
bool InterfaceIpToIdxAndMask(PMIB_IPADDRTABLE pIpAddrTable, char str[], DWORD& dwIndex, DWORD& dwMask);
DWORD MyGetIpAddrTable(PMIB_IPADDRTABLE& pIpAddrTable, BOOL fOrder = FALSE);
DWORD MyGetIpForwardTable(PMIB_IPFORWARDTABLE& pIpRouteTab, BOOL fOrder = FALSE);
void PrintIpForwardTable(PMIB_IPFORWARDTABLE pIpRouteTable);

void _cdecl IpRoute(int argc, char ** argv);
