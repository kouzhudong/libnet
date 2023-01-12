#include "pch.h"
#include "NetApi.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void UserEnum()
/*
//本功能可以列出本电脑上的所有的用户，相当于net user的功能，但还有其他的一些信息。
//可以修改其他的参数以便获得更多的信息。

//made at 2011.12.07
*/
{
    LPUSER_INFO_1 pBuf = NULL;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;

    NetUserEnum(0,
                1,
                FILTER_NORMAL_ACCOUNT,
                (LPBYTE *)&pBuf,
                (DWORD)-1,
                &dwEntriesRead,
                &dwTotalEntries,
                &dwResumeHandle);

    for (DWORD i = 0; i < dwEntriesRead; i++) {
        wprintf(L"%s:%s\n", pBuf->usri1_name, pBuf->usri1_comment);
        pBuf++;
    }
}


int UserEnum(int argc, wchar_t * argv[])
//https://docs.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netuserenum
{
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        return(1);
    }

    // The server is not the default local computer.
    if (argc == 2)
        pszServerName = (LPTSTR)argv[1];

    wprintf(L"\nUser account on %s: \n", pszServerName);

    // Call the NetUserEnum function, specifying level 0; 
    //   enumerate global user account types only.
    do // begin do
    {
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
                              dwLevel,
                              FILTER_NORMAL_ACCOUNT, // global users
                              (LPBYTE *)&pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {// If the call succeeds,
            if ((pTmpBuf = pBuf) != NULL) {
                // Loop through the entries.
                for (i = 0; (i < dwEntriesRead); i++) {
                    assert(pTmpBuf != NULL);
                    if (pTmpBuf == NULL) {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    wprintf(L"\t-- %s\n", pTmpBuf->usri0_name);//  Print the name of the user account.

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else {// Otherwise, print the system error.
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
        }

        if (pBuf != NULL) {// Free the allocated buffer.
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    // Continue to call NetUserEnum while there are more entries. 
    while (nStatus == ERROR_MORE_DATA); // end do

    // Check again for allocated memory.
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    // Print the final count of users enumerated.
    fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

    return 0;
}


void EnumLocalGroup()
/*
//本程序的功能是显示本计算机上的所有的组，及其内的成员的一些信息。
//本程序可以实现net localgroup的功能。

//made at 2011.12.07
*/
{
    DWORD read;
    DWORD total;
    DWORD_PTR resume = 0;
    LPVOID buff;
    NetLocalGroupEnum(0, 1, (unsigned char **)&buff, (DWORD)-1, &read, &total, &resume);
    PLOCALGROUP_INFO_1 info = (PLOCALGROUP_INFO_1)buff;

    for (DWORD i = 0; i < read; i++) {
        printf("GROUP: %S\n", info[i].lgrpi1_name);
        char comment[255];
        WideCharToMultiByte(CP_ACP, 0, info[i].lgrpi1_comment, -1, comment, 255, 0, 0);
        printf("COMMENT: %s\n", comment);

        NetLocalGroupGetMembers(0, info[i].lgrpi1_name, 2, (unsigned char **)&buff, 1024, &read, &total, &resume);
        PLOCALGROUP_MEMBERS_INFO_2 info2 = (PLOCALGROUP_MEMBERS_INFO_2)buff;
        for (unsigned j = 0; j < read; j++) {
            printf("\t域\\名:%S\n", info2[j].lgrmi2_domainandname);
            //printf("\tSID:%d\n", info[i].lgrmi2_sid);
            if (info2[j].lgrmi2_sidusage == SidTypeUser) {
                printf("\tSIDUSAGE:The account is a user account\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeGroup) {
                printf("\tSIDUSAGE:The account is a global group account\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeWellKnownGroup) {
                printf("\tSIDUSAGE:The account is a well-known group account (such as Everyone). \n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeDeletedAccount) {
                printf("\tSIDUSAGE:The account has been deleted\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeDomain) {
                printf("\tSIDUSAGE:SidTypeDomain\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeAlias) {
                printf("\tSIDUSAGE:SidTypeAlias\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeInvalid) {
                printf("\tSIDUSAGE:SidTypeInvalid\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeComputer) {
                printf("\tSIDUSAGE:SidTypeComputer\n");
            } else if (info2[j].lgrmi2_sidusage == SidTypeLabel) {
                printf("\tSIDUSAGE:SidTypeLabel\n");
            } else {
                printf("\tSIDUSAGE:未知\n");
            }
            printf("\n");
        }

        NetApiBufferFree(buff);
        printf("////////////////////////////////////////////////////////////////////////////\n");
    }
}


void EnumShare()
/*
//本功能可以列出本电脑上的所有的共享资源，相当于net share的功能，当然还有其他的一些信息没有列举出来。
//made at 2011.12.08
*/
{
    PSHARE_INFO_502 p, p1;
    DWORD er = 0, tr = 0, resume = 0;

    printf("共享名:            资源:                          注释               \n");
    printf("---------------------------------------------------------------------\n");

    (void)NetShareEnum((LPWSTR)L".", 502, (LPBYTE *)&p, (DWORD)-1, &er, &tr, &resume);
    p1 = p;

    for (DWORD i = 1; i <= er; i++) {
        char comment[255];
        WideCharToMultiByte(0, 0, p->shi502_remark, -1, comment, 255, 0, 0);

        printf("%-20S%-30S%s\n", p->shi502_netname, p->shi502_path, comment);

        p++;
    }

    NetApiBufferFree(p1);
    printf("命令成功完成。\n\n");
}


void EnumShare(int argc, TCHAR * lpszArgv[])
/*
https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netshareenum

调用示例：EnumShare(argc, argv);
输入参数是一个点即可，如：network.exe  .
*/
{
    PSHARE_INFO_502 BufPtr, p;
    NET_API_STATUS res;
    LPTSTR   lpszServer = NULL;
    DWORD er = 0, tr = 0, resume = 0, i;

    switch (argc) {
    case 2:
        lpszServer = lpszArgv[1];
        break;
    default:
        printf("Usage: NetShareEnum <servername>\n");
        return;
    }

    // Print a report header.
    printf("Share:              Local Path:                   Uses:   Descriptor:\n");
    printf("---------------------------------------------------------------------\n");

    // Call the NetShareEnum function; specify level 502.
    do // begin do
    {
        res = NetShareEnum(lpszServer, 502, (LPBYTE *)&BufPtr, MAX_PREFERRED_LENGTH, &er, &tr, &resume);
        if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA) {// If the call succeeds,
            p = BufPtr;

            // Loop through the entries;
            //  print retrieved data.
            for (i = 1; i <= er; i++) {
                printf("%-20S%-30S%-8u", p->shi502_netname, p->shi502_path, p->shi502_current_uses);

                // Validate the value of the shi502_security_descriptor member.
                if (IsValidSecurityDescriptor(p->shi502_security_descriptor))
                    printf("Yes\n");
                else
                    printf("No\n");

                p++;
            }

            NetApiBufferFree(BufPtr);// Free the allocated buffer.
        } else {
            printf("Error: %ld\n", res);
        }
    }
    // Continue to call NetShareEnum while there are more entries. 
    while (res == ERROR_MORE_DATA); // end do
}


void EnumWkstaUser()
/*
//本功能可以列出已经登录到本电脑上的所有的用户。第一个好像不是，是啥具体不太清楚。
//made at 2011.12.08
*/
{
    LPWKSTA_USER_INFO_0 pBuf, pTmpBuf;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;

    NetWkstaUserEnum((LPWSTR)L".", 0, (LPBYTE *)&pBuf, (DWORD)-1, &dwEntriesRead, &dwTotalEntries, &dwResumeHandle);
    pTmpBuf = pBuf;

    for (DWORD i = 0; (i < dwEntriesRead); i++) {
        wprintf(L"%s\n", pTmpBuf->wkui0_username);
        pTmpBuf++;
    }

    NetApiBufferFree(pBuf);
}


int EnumWkstaUser(int argc, wchar_t * argv[])
//https://docs.microsoft.com/en-us/windows/win32/api/lmwksta/nf-lmwksta-netwkstauserenum
{
    LPWKSTA_USER_INFO_0 pBuf = NULL;
    LPWKSTA_USER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        return(1);
    }

    // The server is not the default local computer.
    if (argc == 2)
        pszServerName = argv[1];
    fwprintf(stderr, L"\nUsers currently logged on %s:\n", pszServerName);

    // Call the NetWkstaUserEnum function, specifying level 0.
    do // begin do
    {
        nStatus = NetWkstaUserEnum(pszServerName,
                                   dwLevel,
                                   (LPBYTE *)&pBuf,
                                   dwPrefMaxLen,
                                   &dwEntriesRead,
                                   &dwTotalEntries,
                                   &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {// If the call succeeds,
            if ((pTmpBuf = pBuf) != NULL) {
                for (i = 0; (i < dwEntriesRead); i++) {// Loop through the entries.
                    assert(pTmpBuf != NULL);
                    if (pTmpBuf == NULL) {
                        // Only members of the Administrators local group
                        //  can successfully execute NetWkstaUserEnum
                        //  locally and on a remote server.
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    wprintf(L"\t-- %s\n", pTmpBuf->wkui0_username);// Print the user logged on to the workstation. 

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else {// Otherwise, indicate a system error.
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
        }

        if (pBuf != NULL) {// Free the allocated memory.
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    // Continue to call NetWkstaUserEnum while there are more entries. 
    while (nStatus == ERROR_MORE_DATA); // end do

    // Check again for allocated memory.
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    // Print the final count of workstation users.
    fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

    return 0;
}


int EnumSession(int argc, wchar_t * argv[])
//NetSessionEnum 枚举就不写了，相当于 net session。
//https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netsessionenum
{
    LPSESSION_INFO_10 pBuf = NULL;
    LPSESSION_INFO_10 pTmpBuf;
    DWORD dwLevel = 10;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    LPTSTR pszServerName = NULL;
    LPTSTR pszClientName = NULL;
    LPTSTR pszUserName = NULL;
    NET_API_STATUS nStatus;

    // Check command line arguments.
    if (argc > 4) {
        wprintf(L"Usage: %s [\\\\ServerName] [\\\\ClientName] [UserName]\n", argv[0]);
        return(1);
    }

    if (argc >= 2)
        pszServerName = argv[1];

    if (argc >= 3)
        pszClientName = argv[2];

    if (argc == 4)
        pszUserName = argv[3];

    // Call the NetSessionEnum function, specifying level 10.
    do // begin do
    {
        nStatus = NetSessionEnum(pszServerName,
                                 pszClientName,
                                 pszUserName,
                                 dwLevel,
                                 (LPBYTE *)&pBuf,
                                 dwPrefMaxLen,
                                 &dwEntriesRead,
                                 &dwTotalEntries,
                                 &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {// If the call succeeds,
            if ((pTmpBuf = pBuf) != NULL) {
                for (i = 0; (i < dwEntriesRead); i++) {// Loop through the entries.
                    assert(pTmpBuf != NULL);
                    if (pTmpBuf == NULL) {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    // Print the retrieved data. 
                    wprintf(L"\n\tClient: %s\n", pTmpBuf->sesi10_cname);
                    wprintf(L"\tUser:   %s\n", pTmpBuf->sesi10_username);
                    printf("\tActive: %d\n", pTmpBuf->sesi10_time);
                    printf("\tIdle:   %d\n", pTmpBuf->sesi10_idle_time);

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else {// Otherwise, indicate a system error.
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
        }

        if (pBuf != NULL) {// Free the allocated memory.
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    // Continue to call NetSessionEnum while there are more entries. 
    while (nStatus == ERROR_MORE_DATA); // end do

    // Check again for an allocated buffer.
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    // Print the final count of sessions enumerated.
    fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

    return 0;
}


void EnumConnection(int argc, wchar_t * argv[])
//https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netconnectionenum
{
    DWORD res, i, er = 0, tr = 0, resume = 0;
    PCONNECTION_INFO_1 p, b;
    LPTSTR lpszServer = NULL, lpszShare = NULL;

    if (argc < 2)
        wprintf(L"Syntax: %s [ServerName] ShareName | \\\\ComputerName\n", argv[0]);
    else {
        // The server is not the default local computer.
        if (argc > 2)
            lpszServer = argv[1];

        lpszShare = argv[argc - 1];// ShareName is always the last argument.

        // Call the NetConnectionEnum function, specifying information level 1.
        res = NetConnectionEnum(lpszServer, lpszShare, 1, (LPBYTE *)&p, MAX_PREFERRED_LENGTH, &er, &tr, &resume);
        if (res == 0) {// If no error occurred,
            if (er > 0) {// If there were any results,
                b = p;

                // Loop through the entries; print user name and network name.
                for (i = 0; i < er; i++) {
                    printf("%S\t%S\n", b->coni1_username, b->coni1_netname);
                    b++;
                }

                NetApiBufferFree(p);// Free the allocated buffer.
            }
            // Otherwise, print a message depending on whether 
            //  the qualifier parameter was a computer (\\ComputerName) or a share (ShareName).
            else {
                if (lpszShare[0] == '\\')
                    printf("No connection to %S from %S\n",
                           (lpszServer == NULL) ? TEXT("LocalMachine") : lpszServer, lpszShare);
                else
                    printf("No one connected to %S\\%S\n",
                           (lpszServer == NULL) ? TEXT("\\\\LocalMachine") : lpszServer, lpszShare);
            }
        } else {// Otherwise, print the error.
            printf("Error: %d\n", res);
        }
    }
}


int EnumServer(int argc, wchar_t * argv[])
//NetServerEnum 就不写了，可以使用 WNetEnumResource。
//https://docs.microsoft.com/en-us/windows/win32/api/lmserver/nf-lmserver-netserverenum
{
    LPSERVER_INFO_101 pBuf = NULL;
    LPSERVER_INFO_101 pTmpBuf;
    DWORD dwLevel = 101;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwTotalCount = 0;
    DWORD dwServerType = SV_TYPE_SERVER;        // all servers
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;
    LPWSTR pszDomainName = NULL;
    DWORD i;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [DomainName]\n", argv[0]);
        return(1);
    }

    // The request is not for the primary domain.
    if (argc == 2)
        pszDomainName = argv[1];

    // Call the NetServerEnum function to retrieve information
    //  for all servers, specifying information level 101.
    nStatus = NetServerEnum(pszServerName,
                            dwLevel,
                            (LPBYTE *)&pBuf,
                            dwPrefMaxLen,
                            &dwEntriesRead,
                            &dwTotalEntries,
                            dwServerType,
                            pszDomainName,
                            &dwResumeHandle);
    if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {// If the call succeeds,
        if ((pTmpBuf = pBuf) != NULL) {

            // Loop through the entries and print the data for all server types.
            for (i = 0; i < dwEntriesRead; i++) {
                assert(pTmpBuf != NULL);
                if (pTmpBuf == NULL) {
                    fprintf(stderr, "An access violation has occurred\n");
                    break;
                }

                printf("\tPlatform: %d\n", pTmpBuf->sv101_platform_id);
                wprintf(L"\tName:     %s\n", pTmpBuf->sv101_name);
                printf("\tVersion:  %d.%d\n", pTmpBuf->sv101_version_major, pTmpBuf->sv101_version_minor);
                printf("\tType:     %d", pTmpBuf->sv101_type);

                // Check to see if the server is a domain controller;
                //  if so, identify it as a PDC or a BDC.
                if (pTmpBuf->sv101_type & SV_TYPE_DOMAIN_CTRL)
                    wprintf(L" (PDC)");
                else if (pTmpBuf->sv101_type & SV_TYPE_DOMAIN_BAKCTRL)
                    wprintf(L" (BDC)");

                printf("\n");

                // Also print the comment associated with the server.
                wprintf(L"\tComment:  %s\n\n", pTmpBuf->sv101_comment);

                pTmpBuf++;
                dwTotalCount++;
            }

            // Display a warning if all available entries were
            //  not enumerated, print the number actually enumerated, and the total number available.
            if (nStatus == ERROR_MORE_DATA) {
                fprintf(stderr, "\nMore entries available!!!\n");
                fprintf(stderr, "Total entries: %d", dwTotalEntries);
            }

            printf("\nEntries enumerated: %d\n", dwTotalCount);
        } else {
            printf("No servers were found\n");
            printf("The buffer (bufptr) returned was NULL\n");
            printf("  entriesread: %d\n", dwEntriesRead);
            printf("  totalentries: %d\n", dwEntriesRead);
        }
    } else
        fprintf(stderr, "NetServerEnum failed with error: %d\n", nStatus);

    if (pBuf != NULL)// Free the allocated buffer.
        NetApiBufferFree(pBuf);

    return 0;
}


int EnumServerDisk(int argc, wchar_t * argv[])
//https://docs.microsoft.com/en-us/windows/win32/api/lmserver/nf-lmserver-netserverdiskenum
{
    const int ENTRY_SIZE = 3; // Drive letter, colon, NULL
    LPTSTR pBuf = NULL;
    DWORD dwLevel = 0; // level must be zero
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        return(1);
    }

    // The server is not the default local computer.
    if (argc == 2)
        pszServerName = (LPTSTR)argv[1];

    // Call the NetServerDiskEnum function.
    nStatus = NetServerDiskEnum(pszServerName,
                                dwLevel,
                                (LPBYTE *)&pBuf,
                                dwPrefMaxLen,
                                &dwEntriesRead,
                                &dwTotalEntries,
                                NULL);
    if (nStatus == NERR_Success) {// If the call succeeds,
        LPTSTR pTmpBuf;

        if ((pTmpBuf = pBuf) != NULL) {
            DWORD i;
            DWORD dwTotalCount = 0;

            // Loop through the entries.
            for (i = 0; i < dwEntriesRead; i++) {
                assert(pTmpBuf != NULL);
                if (pTmpBuf == NULL) {
                    // On a remote computer, only members of the
                    //  Administrators or the Server Operators local group can execute NetServerDiskEnum.
                    fprintf(stderr, "An access violation has occurred\n");
                    break;
                }

                // Print drive letter, colon, NULL for each drive;
                //   the number of entries actually enumerated; and the total number of entries available.
                fwprintf(stdout, L"\tDisk: %lS\n", pTmpBuf);

                pTmpBuf += ENTRY_SIZE;
                dwTotalCount++;
            }

            fprintf(stderr, "\nEntries enumerated: %d\n", dwTotalCount);
        }
    } else
        fprintf(stderr, "A system error has occurred: %d\n", nStatus);

    if (pBuf != NULL)// Free the allocated buffer.
        NetApiBufferFree(pBuf);

    return 0;
}


void DisplayStruct(int i, LPNETRESOURCE lpnrLocal)
{
    printf("NETRESOURCE[%d] Scope: ", i);
    switch (lpnrLocal->dwScope) {
    case (RESOURCE_CONNECTED):
        printf("connected\n");
        break;
    case (RESOURCE_GLOBALNET):
        printf("all resources\n");
        break;
    case (RESOURCE_REMEMBERED):
        printf("remembered\n");
        break;
    default:
        printf("unknown scope %d\n", lpnrLocal->dwScope);
        break;
    }

    printf("NETRESOURCE[%d] Type: ", i);
    switch (lpnrLocal->dwType) {
    case (RESOURCETYPE_ANY):
        printf("any\n");
        break;
    case (RESOURCETYPE_DISK):
        printf("disk\n");
        break;
    case (RESOURCETYPE_PRINT):
        printf("print\n");
        break;
    default:
        printf("unknown type %d\n", lpnrLocal->dwType);
        break;
    }

    printf("NETRESOURCE[%d] DisplayType: ", i);
    switch (lpnrLocal->dwDisplayType) {
    case (RESOURCEDISPLAYTYPE_GENERIC):
        printf("generic\n");
        break;
    case (RESOURCEDISPLAYTYPE_DOMAIN):
        printf("domain\n");
        break;
    case (RESOURCEDISPLAYTYPE_SERVER):
        printf("server\n");
        break;
    case (RESOURCEDISPLAYTYPE_SHARE):
        printf("share\n");
        break;
    case (RESOURCEDISPLAYTYPE_FILE):
        printf("file\n");
        break;
    case (RESOURCEDISPLAYTYPE_GROUP):
        printf("group\n");
        break;
    case (RESOURCEDISPLAYTYPE_NETWORK):
        printf("network\n");
        break;
    default:
        printf("unknown display type %d\n", lpnrLocal->dwDisplayType);
        break;
    }

    printf("NETRESOURCE[%d] Usage: 0x%x = ", i, lpnrLocal->dwUsage);
    if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONNECTABLE)
        printf("connectable ");
    if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONTAINER)
        printf("container ");
    printf("\n");

    printf("NETRESOURCE[%d] Localname: %S\n", i, lpnrLocal->lpLocalName);
    printf("NETRESOURCE[%d] Remotename: %S\n", i, lpnrLocal->lpRemoteName);
    printf("NETRESOURCE[%d] Comment: %S\n", i, lpnrLocal->lpComment);
    printf("NETRESOURCE[%d] Provider: %S\n", i, lpnrLocal->lpProvider);
    printf("\n");
}


BOOL WINAPI EnumResource(LPNETRESOURCE lpnr)
/*

调用代码示例：
    LPNETRESOURCE lpnr = NULL;
    if (EnumResource(lpnr) == FALSE) {
        printf("Call to EnumerateFunc failed\n");
        return 1;
    } else
        return 0;

https://docs.microsoft.com/en-us/windows/win32/wnet/enumerating-network-resources
*/
{
    DWORD dwResult, dwResultEnum;
    HANDLE hEnum;
    DWORD cbBuffer = 16384;     // 16K is a good size
    DWORD cEntries = (DWORD)-1;        // enumerate all possible entries
    LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures
    DWORD i;

    // Call the WNetOpenEnum function to begin the enumeration.
    dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
                            RESOURCETYPE_ANY,   // all resources
                            0,  // enumerate all resources
                            lpnr,       // NULL first time the function is called
                            &hEnum);    // handle to the resource
    if (dwResult != NO_ERROR) {
        printf("WnetOpenEnum failed with error %d\n", dwResult);
        return FALSE;
    }

    // Call the GlobalAlloc function to allocate resources.
    lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
    if (lpnrLocal == NULL) {
        printf("WnetOpenEnum failed with error %d\n", dwResult);
        //      NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetOpenEnum");
        return FALSE;
    }

    do {
        ZeroMemory(lpnrLocal, cbBuffer);// Initialize the buffer.

        // Call the WNetEnumResource function to continue the enumeration.
        dwResultEnum = WNetEnumResource(hEnum,  // resource handle
                                        &cEntries,      // defined locally as -1
                                        lpnrLocal,      // LPNETRESOURCE
                                        &cbBuffer);     // buffer size        
        if (dwResultEnum == NO_ERROR) {// If the call succeeds, loop through the structures.
            for (i = 0; i < cEntries; i++) {
                // Call an application-defined function to
                //  display the contents of the NETRESOURCE structures.
                DisplayStruct(i, &lpnrLocal[i]);

                // If the NETRESOURCE structure represents a container resource, 
                //  call the EnumerateFunc function recursively.
                if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage & RESOURCEUSAGE_CONTAINER))
                    //          if(!EnumerateFunc(hwnd, hdc, &lpnrLocal[i]))
                    if (!EnumResource(&lpnrLocal[i]))
                        printf("EnumerateFunc returned FALSE\n");
                //            TextOut(hdc, 10, 10, "EnumerateFunc returned FALSE.", 29);
            }
        } else if (dwResultEnum != ERROR_NO_MORE_ITEMS) {// Process errors.
            printf("WNetEnumResource failed with error %d\n", dwResultEnum);
            //      NetErrorHandler(hwnd, dwResultEnum, (LPSTR)"WNetEnumResource");
            break;
        }
    } while (dwResultEnum != ERROR_NO_MORE_ITEMS);// End do.

    GlobalFree((HGLOBAL)lpnrLocal);// Call the GlobalFree function to free the memory.

    // Call WNetCloseEnum to end the enumeration.
    dwResult = WNetCloseEnum(hEnum);
    if (dwResult != NO_ERROR) {
        // Process errors.
        printf("WNetCloseEnum failed with error %d\n", dwResult);
        //    NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetCloseEnum");
        return FALSE;
    }

    return TRUE;
}


//NetScheduleJobEnum与此类似，更多的就不写了。
//NetGroupEnum
//NetAccessEnum 
//NetWkstaTransportEnum 
//NetFileEnum 
