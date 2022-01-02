#include "pch.h"
#include "dns.h"


#pragma warning(disable:6387)
#pragma warning(disable:6011)
#pragma warning(disable:6255)
#pragma warning(disable:6263)


//////////////////////////////////////////////////////////////////////////////////////////////////


/************************************************************\
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

  Copyright ?2000  Microsoft Corporation.  All Rights Reserved.
/***************************************************************/


#define BUFFER_LEN 255


void Usage(char * progname)
//Usage of the program
{
    fprintf(stderr, "Usage\n%s -n [OwnerName] -t [Type]  -s [DnsServerIp]\n", progname);
    fprintf(stderr, "Where:\n\t\"OwnerName\" is name of the owner of the record set being queried\n");
    fprintf(stderr, "\t\"Type\" is the type of record set to be queried A or PTR\n");
    fprintf(stderr, "\t\"DnsServerIp\"is the IP address of DNS server (in dotted decimal notation)");
    fprintf(stderr, "to which the query should be sent\n");
    exit(1);
}


EXTERN_C
__declspec(dllexport)
void WINAPI DnsQueryTest(int argc, char * argv[])
/*Windows-classic-samples\Samples\Win7Samples\netds\dns\dnsquery\DNSQuery.Cpp
   FILE:          Dnsquery.cpp
   DESCRIPTION:   This sample illustrates the use of DnsQuery() function to send query to
                  a DNS server to resolve the host name to an IP address and vice-versa.
   PLATFORM:      Windows 2000
   WRITTEN BY:    Rashmi Anoop
   DATE:          3/22/2000
*/
/*
============
DESCRIPTION:
============
  This sample sends a query to a DNS server to query host (A) record or PTR record set.

  1.To resolve a host name to an Ip address run the program as follows:
  >Dnsquery -n host1 -t A -s 1.1.1.1( optional)

  2.To resolve an IP address for example 1.2.3.4 to a name run the program as follows:
  >Dnsquery -n 4.3.2.1.in-addr.arpa -t PTR  -s 1.1.1.1(optional)

====================
SUPPORTED PLATFORMS:
====================
  Windows 2000
*/
{
    DNS_STATUS status;               // return value of  DnsQuery_A() function.
    PDNS_RECORD pDnsRecord;          //pointer to DNS_RECORD structure
    PIP4_ARRAY pSrvList = NULL;      //pinter to IP4_ARRAY structure
    PCSTR pOwnerName = NULL;        //owner name to be queried 
    WORD wType = 0;                      //Type of the record to be queried
    char DnsServIp[BUFFER_LEN];      //DNS server ip address
    DNS_FREE_TYPE freetype;
    freetype = DnsFreeRecordListDeep;
    IN_ADDR ipaddr;

    if (argc > 4) {
        for (int i = 1; i < argc; i++) {
            if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
                switch (tolower(argv[i][1])) {
                case 'n':
                    pOwnerName = argv[++i];
                    break;
                case 't':
                    if (!_stricmp(argv[i + 1], "A"))
                        wType = DNS_TYPE_A; //Query host records to resolve a name
                    else if (!_stricmp(argv[i + 1], "PTR"))
                        wType = DNS_TYPE_PTR; //Query PTR records to resovle an IP address
                    else
                        Usage(argv[0]);

                    i++;
                    break;
                case 's':
                    // Allocate memory for IP4_ARRAY structure
                    pSrvList = (PIP4_ARRAY)LocalAlloc(LPTR, sizeof(IP4_ARRAY));
                    if (!pSrvList) {
                        printf("Memory allocation failed \n");
                        exit(1);
                    }

                    if (argv[++i]) {
                        strncpy_s(DnsServIp, _countof(DnsServIp), argv[i], _TRUNCATE);
                        DnsServIp[sizeof(DnsServIp) - 1] = '\0';
                        pSrvList->AddrCount = 1;
                        pSrvList->AddrArray[0] = inet_addr(DnsServIp); //DNS server IP address
                        if (pSrvList->AddrArray[0] == INADDR_NONE) {
                            printf("Invalid DNS server IP address \n");
                            Usage(argv[0]);
                        }
                        break;
                    }

                    break;//少了这个？
                default:
                    Usage(argv[0]);
                    break;
                }
            } else
                Usage(argv[0]);
        }
    } else
        Usage(argv[0]);

    // Calling function DnsQuery_A() to query Host or PTR records    
    status = DnsQuery_A(pOwnerName,                 //pointer to OwnerName 
                        wType,                      //Type of the record to be queried
                        DNS_QUERY_BYPASS_CACHE,     // Bypasses the resolver cache on the lookup. 
                        pSrvList,                   //contains DNS server IP address
                        &pDnsRecord,                //Resource record comprising the response
                        NULL);                     //reserved for future use
    if (status) {
        if (wType == DNS_TYPE_A)
            printf("Failed to query the host record for %s and the error is %d \n", pOwnerName, status);
        else
            printf("Failed to query the PTR record and the error is %d \n", status);
    } else {
        if (wType == DNS_TYPE_A) {
            //convert the Internet network address into a string in Internet standard dotted format.
            ipaddr.S_un.S_addr = (pDnsRecord->Data.A.IpAddress);
            printf("The IP address of the host %s is %s \n", pOwnerName, inet_ntoa(ipaddr));            
            DnsRecordListFree(pDnsRecord, freetype);// Free memory allocated for DNS records 
        } else {
            printf("The host name is %ws  \n", (pDnsRecord->Data.PTR.pNameHost));            
            DnsRecordListFree(pDnsRecord, freetype);// Free memory allocated for DNS records 
        }
    }

    LocalFree(pSrvList);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/************************************************************\
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

  Copyright ?2000  Microsoft Corporation.  All Rights Reserved.
/***************************************************************/


//Usage of the program
void Usage(wchar_t * progname)
{
    fprintf(stderr, "Usage\n%ws -n [OwnerName] - t [Type] -l [Ttl] -d [Data] -s [DnsServerIp]\n", progname);
    fprintf(stderr, "Where:\n\tOwnerName is the owner field to be added\n");
    fprintf(stderr, "\tType is the type of resource record to be added A or CNAME\n");
    fprintf(stderr, "\tData is the data corresponding to RR to be added\n");
    fprintf(stderr, "\tTtl is the time to live value in seconds \n");
    fprintf(stderr, "\tDnsServerIp is the ipaddress of DNS server (in dotted decimal notation)\n");
    exit(1);
}


EXTERN_C
__declspec(dllexport)
void WINAPI ModifyRecords(int argc, wchar_t * argv[])
/*
   FILE:          ModifyRecords.cpp
   DESCRIPTION:   This sample illustrates how to add Host Address( A) and CNAME resource records
                   to DNS server using DNSModifyRecordsInSet() API.

   PLATFORM:      Windows 2000
   WRITTEN BY:    Rashmi Anoop
   DATE:          3/22/2000
*/
/*
========
SUMMARY:
========
  The Modifyrecords sample demonstrates the use of DnsModifyRecordsInSet() function to
  add the resource records.

============
DESCRIPTION:
============
  This sample demonstrates how to add Host
  records (A) and CNAME resource records.

  The following is the example of a Host record
  host1.example.microsoft.com.     IN  A  127.0.0.1

  To add host record run the program as follows:
  >Modifyrecords -n host1.example.microsoft.com -t A -l 360 -d 127.0.0.1 -s 1.1.1.1( optional)

  The following is the example of a Canonical name (CNAME) resource record.
  aliasname.example.microsoft.com.   CNAME   1 truename.example.microsoft.com.

  To add CNAME record run the program as follows:
  >Modifyrecords -n aliasname.example.microsoft.com -t CNAME -l 360 -d truename.example.microsoft.com -s 1.1.1.1(optional)

====================
SUPPORTED PLATFORMS:
====================
  Windows 2000
*/
/*
本工程摘自：Windows-classic-samples\Samples\Win7Samples\netds\dns\modifyrecords\ModifyRecords.cpp

本工程原来是单字符版本，这里需要改为宽字符版本。
*/
{
    USES_CONVERSION;

    DNS_STATUS status;               // return value of  DnsModifyRecordsInSet() function.
    PDNS_RECORD pmyDnsRecord = NULL; //pointer to DNS_RECORD structure
    PIP4_ARRAY pSrvList = NULL;      //pinter to IP4_ARRAY structure
    LPWSTR pOwnerName = NULL, pNameData = NULL;  //owner name and the data for CNAME resource record
    wchar_t HostipAddress[BUFFER_LEN];         //Ip address required to add host record
    wchar_t DnsServIp[BUFFER_LEN];             //DNS server ip address

    memset(HostipAddress, 0, sizeof(HostipAddress));
    memset(DnsServIp, 0, sizeof(DnsServIp));

    //Allocate memory for DNS_RECORD structure.
    pmyDnsRecord = (PDNS_RECORD)LocalAlloc(LPTR, sizeof(DNS_RECORD));
    if (!pmyDnsRecord) {
        printf("Memory allocaltion failed\n");
        exit(1);
    }

    if (argc > 8) {
        for (int i = 1; i < argc; i++) {
            if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
                switch (tolower(argv[i][1])) {
                case 'n':
                    pOwnerName = argv[++i];
                    pmyDnsRecord->pName = pOwnerName; //copy the Owner name information 
                    break;
                case 't':
                    if (!_wcsicmp(argv[i + 1], L"A"))
                        pmyDnsRecord->wType = DNS_TYPE_A; //add host records
                    else if (!_wcsicmp(argv[i + 1], L"CNAME"))
                        pmyDnsRecord->wType = DNS_TYPE_CNAME; //add CNAME records
                    else
                        Usage(argv[0]);
                    i++;
                    break;
                case 'l':
                    pmyDnsRecord->dwTtl = _wtoi(argv[++i]); // time to live value in seconds
                    break;
                case 'd':
                    if (pmyDnsRecord->wType == DNS_TYPE_A) {
                        pmyDnsRecord->wDataLength = sizeof(DNS_A_DATA); //data structure for A records
                        wcsncpy_s(HostipAddress, _countof(HostipAddress), argv[++i], _TRUNCATE);//strncpy_s
                        HostipAddress[_ARRAYSIZE(HostipAddress) - 1] = '\0';
                        pmyDnsRecord->Data.A.IpAddress = inet_addr(W2A(HostipAddress)); //convert string to proper address
                        if (pmyDnsRecord->Data.A.IpAddress == INADDR_NONE) {
                            printf("Invalid IP address in A record data \n");
                            Usage(argv[0]);
                        }
                        break;
                    } else {
                        pmyDnsRecord->wDataLength = sizeof(DNS_PTR_DATA); //data structure for CNAME records
                        pNameData = argv[++i];
                        pmyDnsRecord->Data.Cname.pNameHost = (PWSTR)pNameData;
                        break;
                    }
                case 's':
                    // Allocate memory for IP4_ARRAY structure
                    pSrvList = (PIP4_ARRAY)LocalAlloc(LPTR, sizeof(IP4_ARRAY));
                    if (!pSrvList) {
                        printf("Memory allocation failed \n");
                        exit(1);
                    }
                    if (argv[++i]) {
                        wcsncpy_s(DnsServIp, _countof(DnsServIp), argv[i], _TRUNCATE);//strncpy_s
                        DnsServIp[_ARRAYSIZE(DnsServIp) - 1] = '\0';
                        pSrvList->AddrCount = 1;
                        pSrvList->AddrArray[0] = inet_addr(W2A(DnsServIp)); //DNS server IP address
                        if (pSrvList->AddrArray[0] == INADDR_NONE) {
                            printf("Invalid DNS server IP address\n");
                            Usage(argv[0]);
                        }
                        break;
                    }

                    break;//少了这个？
                default:
                    Usage(argv[0]);
                    break;
                }
            } else
                Usage(argv[0]);
        }
    } else {
        Usage(argv[0]);
    }

    // Calling function DNSModifyRecordsInSet_A to add Host or CNAME records
    status = DnsModifyRecordsInSet_A(pmyDnsRecord,                     //pointer to DNS_RECORD
                                     NULL,
                                     DNS_UPDATE_SECURITY_USE_DEFAULT,  //do not attempt secure dynamic updates
                                     NULL,                             //use default credentials
                                     pSrvList,                         //contains DNS server IP address
                                     NULL);                            //reserved for future use
    if (status) {
        if (pmyDnsRecord->wType == DNS_TYPE_A)
            printf("Failed to add the host record for %ws and the error is %d \n", pOwnerName, status);
        else
            printf("Failed to add the Cname record for %ws and the error is %d \n", pOwnerName, status);
    } else {
        if (pmyDnsRecord->wType == DNS_TYPE_A)
            printf("Successfully added the host record for %ws \n", pOwnerName);
        else
            printf("Successfully added the Cname record for %ws \n", pOwnerName);
    }

    LocalFree(pmyDnsRecord); // Free the memory allocated for DNS_RECORD structure
    LocalFree(pSrvList);     // Free the memory allocated for IP4_ARRAY structure
}


//////////////////////////////////////////////////////////////////////////////////////////////////
