#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
void WINAPI DnsQueryTest(int argc, char * argv[]);

__declspec(dllexport)
void WINAPI ModifyRecords(int argc, wchar_t * argv[]);

__declspec(dllexport)
VOID WINAPI PrintDnsRecordList(PDNS_RECORD DnsRecord);


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
