#pragma once

#include "pch.h"

void DoGetIpForwardTable();
void DoSetIpForwardEntry(char* pszDest, char* pszNetMask, char* pszGateway, char* pszInterface, DWORD dwMetric = 1);
void DoDeleteIpForwardEntry(char* pszDest);
DWORD MyGetIpForwardTable(PMIB_IPFORWARDTABLE& pIpRouteTab, BOOL fOrder = FALSE);
void PrintIpForwardTable(PMIB_IPFORWARDTABLE pIpRouteTable);

void _cdecl IpRoute(int argc, char ** argv);
