#include "pch.h"
#include "NetBIOS.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
Getting the MAC Address for an Ethernet Adapter

http ://msdn.microsoft.com/zh-cn/bb870897

[Netbios is not supported on Windows Vista, Windows Server 2008, and subsequent versions of the operating system]

You can use the Netbios function to get the Media Access Control(MAC) address for an ethernet adapter if your card is bound to NetBIOS.
The following example uses the NCBASTAT command, providing an asterisk(*) as the name in the ncb_callname member of the NCB structure.
*/


typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff[30];
}ASTAT, * PASTAT;

ASTAT Adapter;

void ncbastat(void)
{
    NCB ncb{};
    UCHAR uRetCode{};

    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = 0;

    uRetCode = Netbios(&ncb);
    printf("The NCBRESET return code is: 0x%x \n", uRetCode);

    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = 0;

    memcpy(&ncb.ncb_callname, "*               ", 16);
    ncb.ncb_buffer = reinterpret_cast<UCHAR *>(&Adapter);
    ncb.ncb_length = sizeof(Adapter);

    uRetCode = Netbios(&ncb);
    printf("The NCBASTAT return code is: 0x%x \n", uRetCode);
    if (uRetCode == 0) {
        printf("The Ethernet Number is: %02x%02x%02x%02x%02x%02x\n",
               Adapter.adapt.adapter_address[0],
               Adapter.adapt.adapter_address[1],
               Adapter.adapt.adapter_address[2],
               Adapter.adapt.adapter_address[3],
               Adapter.adapt.adapter_address[4],
               Adapter.adapt.adapter_address[5]);
    }
}

//Windows Me / 98 / 95:  The preceding example does not work reliably on the Windows Me / 98 / 95 versions of Windows.


//////////////////////////////////////////////////////////////////////////////////////////////////

/*
Listing all NetBIOS Names on a LANA

[Netbios is not supported on Windows Vista, Windows Server 2008, and subsequent versions of the operating system]
You can use the Netbios function to list all the NetBIOS names on a LANA.
The following example uses a unique name as the name in the ncb_callname member of the NCB structure.
This causes the adapter status to be treated as a remote call, which enables you to retrieve names added by other processes.
*/

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


void NBListNames()
{
    if (!NBReset(LANANUM, 20, 30)) return;

    if (!NBAddName(LANANUM, LOCALNAME)) return;

    if (!NBListNames(LANANUM, LOCALNAME)) return;

    printf("Succeeded.\n");
}


BOOL NBReset(UCHAR nLana, UCHAR nSessions, UCHAR nNames)
{
    NCB ncb;

    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lsn = 0;                // Allocate new lana_num resources 
    ncb.ncb_lana_num = nLana;
    ncb.ncb_callname[0] = nSessions;  // maximum sessions 
    ncb.ncb_callname[2] = nNames;   // maximum names 

    Netbios(&ncb);
    NBCheck(ncb);

    return (NRC_GOODRET == ncb.ncb_retcode);
}


BOOL NBAddName(UCHAR nLana, LPCSTR szName)
{
    NCB ncb;

    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBADDNAME;
    ncb.ncb_lana_num = nLana;

    MakeNetbiosName((char *)ncb.ncb_name, szName);

    Netbios(&ncb);
    NBCheck(ncb);

    return (NRC_GOODRET == ncb.ncb_retcode);
}


void MakeNetbiosName(char * achDest, LPCSTR szSrc)
// Build a name of length NCBNAMSZ, padding with spaces.
{
    int cchSrc;

    cchSrc = lstrlenA(szSrc);
    if (cchSrc > NCBNAMSZ)
        cchSrc = NCBNAMSZ;

    memset(achDest, ' ', NCBNAMSZ);
    memcpy(achDest, szSrc, cchSrc);
}


BOOL NBListNames(int nLana, LPCSTR szName)
{
    int cbBuffer{};
    ADAPTER_STATUS * pStatus{};
    NAME_BUFFER * pNames{};
    int i{};
    HANDLE hHeap{};

    hHeap = GetProcessHeap();

    // Allocate the largest buffer that might be needed. 
    cbBuffer = sizeof(ADAPTER_STATUS) + 255 * sizeof(NAME_BUFFER);
    pStatus = reinterpret_cast<ADAPTER_STATUS *>(HeapAlloc(hHeap, 0, cbBuffer));
    if (nullptr == pStatus)
        return FALSE;

    if (!NBAdapterStatus(nLana, reinterpret_cast<PVOID>(pStatus), cbBuffer, szName)) {
        HeapFree(hHeap, 0, pStatus);
        return FALSE;
    }

    // The list of names follows the adapter status structure.
    pNames = reinterpret_cast<NAME_BUFFER *>(pStatus + 1);

    for (i = 0; i < pStatus->name_count; i++)
        printf("\t%.*s\n", NCBNAMSZ, pNames[i].name);

    HeapFree(hHeap, 0, pStatus);

    return TRUE;
}


BOOL NBAdapterStatus(int nLana, PVOID pBuffer, int cbBuffer, LPCSTR szName)
{
    NCB ncb;

    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = static_cast<UCHAR>(nLana);

    ncb.ncb_buffer = static_cast<PUCHAR>(pBuffer);
    ncb.ncb_length = static_cast<WORD>(cbBuffer);

    MakeNetbiosName(reinterpret_cast<char *>(ncb.ncb_callname), szName);

    Netbios(&ncb);
    NBCheck(ncb);

    return (NRC_GOODRET == ncb.ncb_retcode);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
