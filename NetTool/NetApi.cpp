#include "pch.h"
#include "NetApi.h"


//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////


void GetUserFlag(DWORD flags, wstring & FlagsString)
{
    if (flags & UF_SCRIPT) {
        FlagsString += L"登录脚本；";
    }

    if (flags & UF_ACCOUNTDISABLE) {
        FlagsString += L"用户的帐户已禁用；";
    }

    if (flags & UF_HOMEDIR_REQUIRED) {
        FlagsString += L"主目录；";
    }

    if (flags & UF_LOCKOUT) {
        FlagsString += L"帐户当前已锁定；";
    }

    if (flags & UF_PASSWD_NOTREQD) {
        FlagsString += L"不需要密码；";
    }

    if (flags & UF_PASSWD_CANT_CHANGE) {
        FlagsString += L"用户无法更改密码；";
    }

    if (flags & UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED) {
        FlagsString += L"用户的密码存储在 Active Directory 中的可逆加密下；";
    }

    if (flags & UF_DONT_EXPIRE_PASSWD) {
        FlagsString += L"密码在帐户上永不过期；";
    }

    if (flags & UF_MNS_LOGON_ACCOUNT) {
        FlagsString += L"UF_MNS_LOGON_ACCOUNT；";
    }

    if (flags & UF_SMARTCARD_REQUIRED) {
        FlagsString += L"要求用户使用智能卡登录到用户帐户；";
    }

    if (flags & UF_TRUSTED_FOR_DELEGATION) {
        FlagsString += L"帐户已启用委派；";
    }

    if (flags & UF_NOT_DELEGATED) {
        FlagsString += L"将帐户标记为“敏感”;其他用户不能充当此用户帐户的代理；";
    }

    if (flags & UF_USE_DES_KEY_ONLY) {
        FlagsString += L"将此主体限制为仅对密钥使用数据加密标准 (DES) 加密类型；";
    }

    if (flags & UF_DONT_REQUIRE_PREAUTH) {
        FlagsString += L"此帐户不需要 Kerberos 预身份验证即可登录；";
    }

    if (flags & UF_PASSWORD_EXPIRED) {
        FlagsString += L"用户的密码已过期；";
    }

    if (flags & UF_TRUSTED_TO_AUTHENTICATE_FOR_DELEGATION) {
        FlagsString += L"该帐户受信任；";
    }

    if (flags & UF_NO_AUTH_DATA_REQUIRED) {
        FlagsString += L"UF_NO_AUTH_DATA_REQUIRED；";
    }

    if (flags & UF_PARTIAL_SECRETS_ACCOUNT) {
        FlagsString += L"UF_PARTIAL_SECRETS_ACCOUNT；";
    }

    if (flags & UF_USE_AES_KEYS) {
        FlagsString += L"UF_USE_AES_KEYS；";
    }

    //以下值描述帐户类型。 只能设置一个值。

    if (flags & UF_TEMP_DUPLICATE_ACCOUNT) {
        FlagsString += L"这是主帐户位于另一个域中的用户的帐户；";
    }

    if (flags & UF_NORMAL_ACCOUNT) {
        FlagsString += L"典型用户的默认帐户类型；";
    }

    if (flags & UF_INTERDOMAIN_TRUST_ACCOUNT) {
        FlagsString += L"这是信任其他域的域的帐户的允许；";
    }

    if (flags & UF_WORKSTATION_TRUST_ACCOUNT) {
        FlagsString += L"这是属于此域的计算机的计算机帐户；";
    }

    if (flags & UF_SERVER_TRUST_ACCOUNT) {
        FlagsString += L"这是属于此域的备份域控制器的计算机帐户；";
    }
}


int UserEnum()
/*
本功能可以列出本电脑上的所有的用户，相当于net user的功能，但还有其他的一些信息。
可以修改其他的参数以便获得更多的信息。

made at 2011.12.07
*/
{
    LPUSER_INFO_1 pBuf = nullptr;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;

    NetUserEnum(0,
                1,
                FILTER_NORMAL_ACCOUNT,
                reinterpret_cast<LPBYTE *>(&pBuf),
                static_cast<DWORD>(-1),
                &dwEntriesRead,
                &dwTotalEntries,
                &dwResumeHandle);

    for (DWORD i = 0; i < dwEntriesRead; i++) {
        wstring FlagsString;
        GetUserFlag(pBuf->usri1_flags, FlagsString);
        wprintf(L"name:%s\n", pBuf->usri1_name);
        wprintf(L"comment:%s\n", pBuf->usri1_comment);
        wprintf(L"Flag:%ls\n", FlagsString.c_str());
        wprintf(L"\n");

        pBuf++;
    }

    return 0;
}


int UserEnum(int argc, wchar_t * argv[])
/*
NetUserEnum 函数检索有关服务器上所有用户帐户的信息。

NetUserEnum 函数检索有关指定远程服务器或本地计算机上的所有用户帐户的信息。

NetQueryDisplayInformation 函数可用于快速枚举用户、计算机或全局组帐户信息，以便在用户界面 中显示。

如果要对 Active Directory 进行编程，则可以调用某些 Active Directory 服务接口 (ADSI) 方法，
以实现通过调用网络管理用户函数实现的相同功能。
有关详细信息，请参阅 IADsUser 和 IADsComputer。

NetUserEnum 函数不会返回所有系统用户。
它仅返回通过调用 NetUserAdd 函数添加的用户。
无法保证将按排序顺序返回用户列表。

用户帐户名称限制为 20 个字符，组名限制为 256 个字符。

下面的代码示例演示如何通过调用 NetUserEnum 函数检索有关服务器上的用户帐户的信息。
此示例调用 NetUserEnum，指定信息级别 0 (USER_INFO_0) 仅枚举全局用户帐户。
如果调用成功，代码将循环访问条目并输出每个用户帐户的名称。
最后，代码示例释放为信息缓冲区分配的内存，并打印枚举的用户总数。

https://docs.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netuserenum
https://learn.microsoft.com/zh-cn/windows/win32/api/lmaccess/nf-lmaccess-netuserenum
*/
{
    LPUSER_INFO_0 pBuf = nullptr;
    LPUSER_INFO_0 pTmpBuf{};
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i{};
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus{};
    LPCWSTR pszServerName = nullptr;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        return (1);
    }

    // The server is not the default local computer.
    if (argc == 2)
        pszServerName = argv[1];

    wprintf(L"\nUser account on %s: \n", pszServerName);

    // Call the NetUserEnum function, specifying level 0;
    //   enumerate global user account types only.
    do { // begin do
        nStatus = NetUserEnum(pszServerName,
                              dwLevel,
                              FILTER_NORMAL_ACCOUNT, // global users
                              reinterpret_cast<LPBYTE *>(&pBuf),
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) { // If the call succeeds,
            if ((pTmpBuf = pBuf) != nullptr) {
                for (i = 0; (i < dwEntriesRead); i++) { // Loop through the entries.
                    assert(pTmpBuf != nullptr);
                    if (pTmpBuf == nullptr) {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    wprintf(L"\t-- %s\n", pTmpBuf->usri0_name); //  Print the name of the user account.

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else { // Otherwise, print the system error.
            fprintf(stderr, "A system error has occurred: %u\n", nStatus);
        }

        if (pBuf != nullptr) { // Free the allocated buffer.
            NetApiBufferFree(pBuf);
            pBuf = nullptr;
        }
    }
    // Continue to call NetUserEnum while there are more entries.
    while (nStatus == ERROR_MORE_DATA); // end do

    // Check again for allocated memory.
    if (pBuf != nullptr)
        NetApiBufferFree(pBuf);

    // Print the final count of users enumerated.
    fprintf(stderr, "\nTotal of %u entries enumerated\n", dwTotalCount);

    return 0;
}


void NetQueryDisplayInfo(int argc, char * argv[])
/*
NetQueryDisplayInformation 函数返回用户帐户、计算机或组帐户信息。
调用此函数可快速枚举帐户信息，以便在用户界面中显示。

NetQueryDisplayInformation 函数仅返回调用方具有读取访问权限的信息。
调用方必须具有对 Domain 对象的 List Contents 访问权限，并且对位于系统容器中的 SAM 服务器对象枚举整个 SAM 域访问权限。

NetQueryDisplayInformation 和 NetGetDisplayInformationIndex 函数提供了一种用于枚举用户和组帐户的有效机制。
如果可能，请使用这些函数，而不是 NetUserEnum 函数或 NetGroupEnum 函数。

若要枚举信任域或成员计算机帐户，请调用 NetUserEnum，指定相应的筛选器值以获取所需的帐户信息。
若要枚举受信任的域，请调用 LsaEnumerateTrustedDomains 或 LsaEnumerateTrustedDomainsEx 函数。

此函数返回的条目数取决于根域对象上的安全描述符。
API 将返回域中的前 100 个条目或整个条目集，具体取决于用户的访问权限。
用于控制此行为的 ACE 是“SAM-Enumerate-Entire-Domain”，默认情况下授予经过身份验证的用户。
管理员可以修改此设置，以允许用户枚举整个域。

每次调用 NetQueryDisplayInformation 最多返回 100 个对象。
调用 NetQueryDisplayInformation 函数来枚举域帐户信息可能会降低性能成本。
如果要对 Active Directory 进行编程，则可以使用 IDirectorySearch 接口上的方法针对域进行分页查询。
有关详细信息，请参阅 IDirectorySearch：：SetSearchPreference 和 IDirectorySearch：：ExecuteSearch。
若要枚举受信任的域，请调用 LsaEnumerateTrustedDomainsEx 函数。

经测试：
1.Level == 3，ReturnedEntryCount == 1.
2.Level == 2，ReturnedEntryCount == 0.
3.

https://learn.microsoft.com/zh-cn/windows/win32/api/lmaccess/nf-lmaccess-netquerydisplayinformation
*/
{
    PNET_DISPLAY_GROUP pBuff, p;
    DWORD res, dwRec, i = 0;
    TCHAR szServer[255] = TEXT(""); // You can pass a NULL or empty string to retrieve the local information.

    if (argc > 1)
        // Check to see if a server name was passed;
        //  if so, convert it to Unicode.
        MultiByteToWideChar(CP_ACP, 0, argv[1], -1, szServer, 255);

    do { // begin do
        // Call the NetQueryDisplayInformation function;
        //   specify information level 3 (group account information).
        res = NetQueryDisplayInformation(szServer, 3, i, 1000, MAX_PREFERRED_LENGTH, &dwRec, (PVOID *)&pBuff);
        if ((res == ERROR_SUCCESS) || (res == ERROR_MORE_DATA)) { // If the call succeeds,
            p = pBuff;
            for (; dwRec > 0; dwRec--) {
                // Print the retrieved group information.
                printf("Name:      %S\n"
                       "Comment:   %S\n"
                       "Group ID:  %u\n"
                       "Attributes: %u\n"
                       "--------------------------------\n",
                       p->grpi3_name,
                       p->grpi3_comment,
                       p->grpi3_group_id,
                       p->grpi3_attributes);

                // If there is more data, set the index.
                i = p->grpi3_next_index;
                p++;
            }

            NetApiBufferFree(pBuff); // Free the allocated memory.
        } else {
            printf("Error: %u\n", res);
        }

        // Continue while there is more data.
    } while (res == ERROR_MORE_DATA); // end do

    PNET_DISPLAY_MACHINE pBuff2, p2;
    do { // begin do
         // Call the NetQueryDisplayInformation function;
         //   specify information level 3 (group account information).
        res = NetQueryDisplayInformation(szServer, 2, i, 1000, MAX_PREFERRED_LENGTH, &dwRec, (PVOID *)&pBuff2);
        if ((res == ERROR_SUCCESS) || (res == ERROR_MORE_DATA)) { // If the call succeeds,
            p2 = pBuff2;
            for (; dwRec > 0; dwRec--) {
                // Print the retrieved group information.
                printf("Name:      %S\n"
                       "Comment:   %S\n"
                       "flags:     %u\n"
                       "user_id:   %u\n"
                       "--------------------------------\n",
                       p2->usri2_name,
                       p2->usri2_comment,
                       p2->usri2_flags,
                       p2->usri2_user_id);

                // If there is more data, set the index.
                i = p2->usri2_next_index;
                p2++;
            }

            NetApiBufferFree(pBuff2); // Free the allocated memory.
        } else {
            printf("Error: %u\n", res);
        }

        // Continue while there is more data.
    } while (res == ERROR_MORE_DATA); // end do

    //本想这个遍历的多，谁知竟然比NetUserEnum还少，当前活动的都没遍历出。
    PNET_DISPLAY_USER pBuff3, p3;
    do { // begin do
         // Call the NetQueryDisplayInformation function;
         //   specify information level 3 (group account information).
        res = NetQueryDisplayInformation(szServer, 1, i, 1000, MAX_PREFERRED_LENGTH, &dwRec, (PVOID *)&pBuff3);
        if ((res == ERROR_SUCCESS) || (res == ERROR_MORE_DATA)) { // If the call succeeds,
            p3 = pBuff3;
            for (; dwRec > 0; dwRec--) {
                // Print the retrieved group information.
                printf("Name:      %ls\n"
                       "Comment:   %ls\n"
                       "flags:     %u\n"
                       "full_name: %ls\n"
                       "user_id:   %u\n"
                       "--------------------------------\n",
                       p3->usri1_name,
                       p3->usri1_comment,
                       p3->usri1_flags,
                       p3->usri1_full_name,
                       p3->usri1_user_id);

                // If there is more data, set the index.
                i = p3->usri1_next_index;
                p3++;
            }

            NetApiBufferFree(pBuff3); // Free the allocated memory.
        } else {
            printf("Error: %u\n", res);
        }

        // Continue while there is more data.
    } while (res == ERROR_MORE_DATA); // end do
}


int EnumLocalGroup()
/*
本程序的功能是显示本计算机上的所有的组，及其内的成员的一些信息。
本程序可以实现net localgroup的功能。

made at 2011.12.07
*/
{
    DWORD read{};
    DWORD total{};
    DWORD_PTR resume = 0;
    LPVOID buff{};
    DWORD ret = NetLocalGroupEnum(0, 1, reinterpret_cast<unsigned char **>(&buff), static_cast<DWORD>(-1), &read, &total, &resume);
    PLOCALGROUP_INFO_1 info = reinterpret_cast<PLOCALGROUP_INFO_1>(buff);

    for (DWORD i = 0; i < read; i++) {
        printf("GROUP: %S\n", info[i].lgrpi1_name);
        char comment[255];
        WideCharToMultiByte(CP_ACP, 0, info[i].lgrpi1_comment, -1, comment, 255, nullptr, nullptr);
        printf("COMMENT: %s\n", comment);

        DWORD entriesread{};
        ret = NetLocalGroupGetMembers(nullptr,
                                      info[i].lgrpi1_name,
                                      2,
                                      reinterpret_cast<unsigned char **>(&buff),
                                      1024,
                                      &entriesread,
                                      &total,
                                      &resume);
        PLOCALGROUP_MEMBERS_INFO_2 info2 = reinterpret_cast<PLOCALGROUP_MEMBERS_INFO_2>(buff);
        for (unsigned j = 0; j < entriesread; j++) {
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
        printf("\n");
    }

    return 0;
}


int EnumShare()
/*
本功能可以列出本电脑上的所有的共享资源，相当于net share的功能，当然还有其他的一些信息没有列举出来。
made at 2011.12.08
*/
{
    PSHARE_INFO_502 p{}, p1{};
    DWORD er = 0, tr = 0, resume = 0;

    printf("共享名:            资源:                          注释               \n");
    printf("---------------------------------------------------------------------\n");

    (void)NetShareEnum((LPWSTR)L".", 502, reinterpret_cast<LPBYTE *>(&p), static_cast<DWORD>(-1), &er, &tr, &resume);
    p1 = p;

    for (DWORD i = 1; i <= er; i++) {
        char comment[255];
        WideCharToMultiByte(0, 0, p->shi502_remark, -1, comment, 255, 0, 0);

        printf("%-20S%-30S%s\n", p->shi502_netname, p->shi502_path, comment);

        p++;
    }

    NetApiBufferFree(p1);
    printf("命令成功完成。\n\n");
    return 0;
}


int EnumShare(int argc, TCHAR * lpszArgv[])
/*
https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netshareenum

调用示例：EnumShare(argc, argv);
输入参数是一个点即可，如：network.exe  .
*/
{
    PSHARE_INFO_502 BufPtr{}, p{};
    NET_API_STATUS res{};
    LPTSTR lpszServer = nullptr;
    DWORD er = 0, tr = 0, resume = 0, i{};

    switch (argc) {
    case 1:
        lpszServer = nullptr;
        break;
    case 2:
        lpszServer = lpszArgv[1];
        break;
    default:
        printf("Usage: NetShareEnum <servername>\n");
        return 0;
    }

    // Print a report header.
    printf("Share:              Local Path:                   Uses:   Descriptor:\n");
    printf("---------------------------------------------------------------------\n");

    // Call the NetShareEnum function; specify level 502.
    do // begin do
    {
        res = NetShareEnum(lpszServer, 502, reinterpret_cast<LPBYTE *>(&BufPtr), MAX_PREFERRED_LENGTH, &er, &tr, &resume);
        if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA) { // If the call succeeds,
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

            NetApiBufferFree(BufPtr); // Free the allocated buffer.
        } else {
            printf("Error: %lu\n", res);
        }
    }
    // Continue to call NetShareEnum while there are more entries.
    while (res == ERROR_MORE_DATA); // end do

    return 0;
}


void EnumWkstaUser()
/*
本功能可以列出已经登录到本电脑上的所有的用户。第一个好像不是，是啥具体不太清楚。
made at 2011.12.08
*/
{
    LPWKSTA_USER_INFO_0 pBuf{}, pTmpBuf{};
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;

    NetWkstaUserEnum((LPWSTR)L".",
                     0,
                     reinterpret_cast<LPBYTE *>(&pBuf),
                     static_cast<DWORD>(-1),
                     &dwEntriesRead,
                     &dwTotalEntries,
                     &dwResumeHandle);
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
    LPWKSTA_USER_INFO_0 pBuf = nullptr;
    LPWKSTA_USER_INFO_0 pTmpBuf{};
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i{};
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus{};
    LPWSTR pszServerName = nullptr;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        return (1);
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
                                   reinterpret_cast<LPBYTE *>(&pBuf),
                                   dwPrefMaxLen,
                                   &dwEntriesRead,
                                   &dwTotalEntries,
                                   &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) { // If the call succeeds,
            if ((pTmpBuf = pBuf) != nullptr) {
                for (i = 0; (i < dwEntriesRead); i++) { // Loop through the entries.
                    assert(pTmpBuf != nullptr);
                    if (pTmpBuf == nullptr) {
                        // Only members of the Administrators local group
                        //  can successfully execute NetWkstaUserEnum
                        //  locally and on a remote server.
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    wprintf(L"\t-- %s\n", pTmpBuf->wkui0_username); // Print the user logged on to the workstation.

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else { // Otherwise, indicate a system error.
            fprintf(stderr, "A system error has occurred: %u\n", nStatus);
        }

        if (pBuf != nullptr) { // Free the allocated memory.
            NetApiBufferFree(pBuf);
            pBuf = nullptr;
        }
    }
    // Continue to call NetWkstaUserEnum while there are more entries.
    while (nStatus == ERROR_MORE_DATA); // end do

    // Check again for allocated memory.
    if (pBuf != nullptr)
        NetApiBufferFree(pBuf);

    // Print the final count of workstation users.
    fprintf(stderr, "\nTotal of %u entries enumerated\n", dwTotalCount);

    return 0;
}


int EnumSession(int argc, wchar_t * argv[])
//NetSessionEnum 枚举就不写了，相当于 net session。
//https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netsessionenum
{
    LPSESSION_INFO_10 pBuf = nullptr;
    LPSESSION_INFO_10 pTmpBuf{};
    DWORD dwLevel = 10;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i{};
    DWORD dwTotalCount = 0;
    LPTSTR pszServerName = nullptr;
    LPTSTR pszClientName = nullptr;
    LPTSTR pszUserName = nullptr;
    NET_API_STATUS nStatus{};

    // Check command line arguments.
    if (argc > 4) {
        wprintf(L"Usage: %s [\\\\ServerName] [\\\\ClientName] [UserName]\n", argv[0]);
        return (1);
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
                                 reinterpret_cast<LPBYTE *>(&pBuf),
                                 dwPrefMaxLen,
                                 &dwEntriesRead,
                                 &dwTotalEntries,
                                 &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) { // If the call succeeds,
            if ((pTmpBuf = pBuf) != nullptr) {
                for (i = 0; (i < dwEntriesRead); i++) { // Loop through the entries.
                    assert(pTmpBuf != nullptr);
                    if (pTmpBuf == nullptr) {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    // Print the retrieved data.
                    wprintf(L"\n\tClient: %s\n", pTmpBuf->sesi10_cname);
                    wprintf(L"\tUser:   %s\n", pTmpBuf->sesi10_username);
                    printf("\tActive: %u\n", pTmpBuf->sesi10_time);
                    printf("\tIdle:   %u\n", pTmpBuf->sesi10_idle_time);

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else { // Otherwise, indicate a system error.
            fprintf(stderr, "A system error has occurred: %u\n", nStatus);
        }

        if (pBuf != nullptr) { // Free the allocated memory.
            NetApiBufferFree(pBuf);
            pBuf = nullptr;
        }
    }
    // Continue to call NetSessionEnum while there are more entries.
    while (nStatus == ERROR_MORE_DATA); // end do

    // Check again for an allocated buffer.
    if (pBuf != nullptr)
        NetApiBufferFree(pBuf);

    // Print the final count of sessions enumerated.
    fprintf(stderr, "\nTotal of %u entries enumerated\n", dwTotalCount);

    return 0;
}


int EnumConnection(int argc, wchar_t * argv[])
//wprintf(L"Syntax: %s [ServerName] ShareName | \\\\ComputerName\n", argv[0]);
//https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netconnectionenum
{
    DWORD res{}, i{}, er = 0, tr = 0, resume = 0;
    PCONNECTION_INFO_1 p{}, b{};
    LPTSTR lpszServer = nullptr, lpszShare = nullptr;

    if (argc > 2) {
        return 0;
    }

    switch (argc) {
    case 1:
        lpszServer = nullptr;
        break;
    case 2:
        lpszServer = argv[1]; // The server is not the default local computer.
        break;
    default:
        lpszServer = argv[1]; // The server is not the default local computer.
        break;
    }

    lpszShare = argv[argc - 1]; // ShareName is always the last argument.

    // Call the NetConnectionEnum function, specifying information level 1.
    res = NetConnectionEnum(lpszServer,
                            lpszShare,
                            1,
                            reinterpret_cast<LPBYTE *>(&p),
                            MAX_PREFERRED_LENGTH,
                            &er,
                            &tr,
                            &resume);
    if (res == 0) {   // If no error occurred,
        if (er > 0) { // If there were any results,
            b = p;

            // Loop through the entries; print user name and network name.
            for (i = 0; i < er; i++) {
                printf("%S\t%S\n", b->coni1_username, b->coni1_netname);
                b++;
            }

            NetApiBufferFree(p); // Free the allocated buffer.
        }
        // Otherwise, print a message depending on whether
        //  the qualifier parameter was a computer (\\ComputerName) or a share (ShareName).
        else {
            if (lpszShare[0] == '\\')
                printf("No connection to %S from %S\n",
                       (lpszServer == nullptr) ? TEXT("LocalMachine") : lpszServer,
                       lpszShare);
            else
                printf("No one connected to %S\\%S\n",
                       (lpszServer == nullptr) ? TEXT("\\\\LocalMachine") : lpszServer,
                       lpszShare);
        }
    } else { // Otherwise, print the error.
        printf("Error: %u\n", res);
    }

    return 0;
}


int EnumServer(int argc, wchar_t * argv[])
//NetServerEnum 就不写了，可以使用 WNetEnumResource。
//https://docs.microsoft.com/en-us/windows/win32/api/lmserver/nf-lmserver-netserverenum
{
    LPSERVER_INFO_101 pBuf = nullptr;
    LPSERVER_INFO_101 pTmpBuf{};
    DWORD dwLevel = 101;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwTotalCount = 0;
    DWORD dwServerType = SV_TYPE_SERVER; // all servers
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus{};
    LPWSTR pszServerName = nullptr;
    LPWSTR pszDomainName = nullptr;
    DWORD i{};

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [DomainName]\n", argv[0]);
        return (1);
    }

    // The request is not for the primary domain.
    if (argc == 2)
        pszDomainName = argv[1];

    // Call the NetServerEnum function to retrieve information
    //  for all servers, specifying information level 101.
    nStatus = NetServerEnum(pszServerName,
                            dwLevel,
                            reinterpret_cast<LPBYTE *>(&pBuf),
                            dwPrefMaxLen,
                            &dwEntriesRead,
                            &dwTotalEntries,
                            dwServerType,
                            pszDomainName,
                            &dwResumeHandle);
    if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) { // If the call succeeds,
        if ((pTmpBuf = pBuf) != nullptr) {

            // Loop through the entries and print the data for all server types.
            for (i = 0; i < dwEntriesRead; i++) {
                assert(pTmpBuf != nullptr);
                if (pTmpBuf == nullptr) {
                    fprintf(stderr, "An access violation has occurred\n");
                    break;
                }

                printf("\tPlatform: %u\n", pTmpBuf->sv101_platform_id);
                wprintf(L"\tName:     %s\n", pTmpBuf->sv101_name);
                printf("\tVersion:  %u.%u\n", pTmpBuf->sv101_version_major, pTmpBuf->sv101_version_minor);
                printf("\tType:     %u", pTmpBuf->sv101_type);

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
                fprintf(stderr, "Total entries: %u", dwTotalEntries);
            }

            printf("\nEntries enumerated: %u\n", dwTotalCount);
        } else {
            printf("No servers were found\n");
            printf("The buffer (bufptr) returned was NULL\n");
            printf("  entriesread: %u\n", dwEntriesRead);
            printf("  totalentries: %u\n", dwEntriesRead);
        }
    } else
        fprintf(stderr, "NetServerEnum failed with error: %u\n", nStatus);

    if (pBuf != nullptr) // Free the allocated buffer.
        NetApiBufferFree(pBuf);

    return 0;
}


int EnumServerDisk(int argc, wchar_t * argv[])
//https://docs.microsoft.com/en-us/windows/win32/api/lmserver/nf-lmserver-netserverdiskenum
{
    const int ENTRY_SIZE = 3; // Drive letter, colon, NULL
    LPTSTR pBuf = nullptr;
    DWORD dwLevel = 0; // level must be zero
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus{};
    LPWSTR pszServerName = nullptr;

    if (argc > 2) {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        return (1);
    }

    // The server is not the default local computer.
    if (argc == 2)
        pszServerName = argv[1];

    // Call the NetServerDiskEnum function.
    nStatus = NetServerDiskEnum(pszServerName,
                                dwLevel,
                                reinterpret_cast<LPBYTE *>(&pBuf),
                                dwPrefMaxLen,
                                &dwEntriesRead,
                                &dwTotalEntries,
                                nullptr);
    if (nStatus == NERR_Success) { // If the call succeeds,
        LPTSTR pTmpBuf;

        if ((pTmpBuf = pBuf) != nullptr) {
            DWORD i{};
            DWORD dwTotalCount = 0;

            // Loop through the entries.
            for (i = 0; i < dwEntriesRead; i++) {
                assert(pTmpBuf != nullptr);
                if (pTmpBuf == nullptr) {
                    // On a remote computer, only members of the
                    //  Administrators or the Server Operators local group can execute NetServerDiskEnum.
                    fprintf(stderr, "An access violation has occurred\n");
                    break;
                }

                // Print drive letter, colon, nullptr for each drive;
                //   the number of entries actually enumerated; and the total number of entries available.
                fwprintf(stdout, L"\tDisk: %lS\n", pTmpBuf);

                pTmpBuf += ENTRY_SIZE;
                dwTotalCount++;
            }

            fprintf(stderr, "\nEntries enumerated: %u\n", dwTotalCount);
        }
    } else
        fprintf(stderr, "A system error has occurred: %u\n", nStatus);

    if (pBuf != nullptr) // Free the allocated buffer.
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
        printf("unknown scope %u\n", lpnrLocal->dwScope);
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
        printf("unknown type %u\n", lpnrLocal->dwType);
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
        printf("unknown display type %u\n", lpnrLocal->dwDisplayType);
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


BOOL EnumResource(LPNETRESOURCE lpnr)
/*

调用代码示例：
    LPNETRESOURCE lpnr = nullptr;
    if (EnumResource(lpnr) == FALSE) {
        printf("Call to EnumerateFunc failed\n");
        return 1;
    } else
        return 0;

https://docs.microsoft.com/en-us/windows/win32/wnet/enumerating-network-resources
*/
{
    DWORD dwResult{}, dwResultEnum{};
    HANDLE hEnum{};
    DWORD cbBuffer = 16384;                  // 16K is a good size
    DWORD cEntries = static_cast<DWORD>(-1); // enumerate all possible entries
    LPNETRESOURCE lpnrLocal{};               // pointer to enumerated structures
    DWORD i{};

    // Call the WNetOpenEnum function to begin the enumeration.
    dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
                            RESOURCETYPE_ANY,   // all resources
                            0,                  // enumerate all resources
                            lpnr,               // NULL first time the function is called
                            &hEnum);            // handle to the resource
    if (dwResult != NO_ERROR) {
        printf("WnetOpenEnum failed with error %u\n", dwResult);
        return FALSE;
    }

    // Call the GlobalAlloc function to allocate resources.
    lpnrLocal = static_cast<LPNETRESOURCE>(GlobalAlloc(GPTR, cbBuffer));
    if (lpnrLocal == nullptr) {
        printf("WnetOpenEnum failed with error %u\n", dwResult);
        //      NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetOpenEnum");
        return FALSE;
    }

    do {
        ZeroMemory(lpnrLocal, cbBuffer); // Initialize the buffer.

        // Call the WNetEnumResource function to continue the enumeration.
        dwResultEnum = WNetEnumResource(hEnum,      // resource handle
                                        &cEntries,  // defined locally as -1
                                        lpnrLocal,  // LPNETRESOURCE
                                        &cbBuffer); // buffer size
        if (dwResultEnum == NO_ERROR) {             // If the call succeeds, loop through the structures.
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
        } else if (dwResultEnum != ERROR_NO_MORE_ITEMS) { // Process errors.
            printf("WNetEnumResource failed with error %u\n", dwResultEnum);
            //      NetErrorHandler(hwnd, dwResultEnum, (LPSTR)"WNetEnumResource");
            break;
        }
    } while (dwResultEnum != ERROR_NO_MORE_ITEMS); // End do.

    GlobalFree(static_cast<HGLOBAL>(lpnrLocal)); // Call the GlobalFree function to free the memory.

    // Call WNetCloseEnum to end the enumeration.
    dwResult = WNetCloseEnum(hEnum);
    if (dwResult != NO_ERROR) {
        // Process errors.
        printf("WNetCloseEnum failed with error %u\n", dwResult);
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
//NetUserChangePassword 在另一个工程里，用于弱密码检测。


//////////////////////////////////////////////////////////////////////////////////////////////////


static int Usage(__in const wchar_t * name)
/*++
Routine Description:
    This routine prints the intended usage for this program.
Arguments:
    progName - NULL terminated string representing the name of the executable
--*/
{
    wprintf(L"%s user.\n", name);
    wprintf(L"%s localgroup.\n", name);
    wprintf(L"%s share.\n", name);
    wprintf(L"%s session.\n", name);
    wprintf(L"%s loggedUser.\n", name);
    wprintf(L"%s Connection.\n", name);
    wprintf(L"%s Server.\n", name);
    wprintf(L"%s ServerDisk.\n", name);
    wprintf(L"%s Resource.\n", name);

    return ERROR_SUCCESS;
}


int net(int argc, wchar_t * argv[])
{
    int ret = ERROR_SUCCESS;

    if (argc == 1) {
        return Usage(argv[0]);
    }

    if (argc == 2 && lstrcmpi(argv[1], TEXT("user")) == 0) {
        ret = UserEnum();
        NetQueryDisplayInfo(0, NULL);
        ret = UserEnum(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("localgroup")) == 0) {
        ret = EnumLocalGroup();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("share")) == 0) {
        ret = EnumShare();
        ret = EnumShare(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("session")) == 0) {
        ret = EnumSession(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("loggedUser")) == 0) {
        EnumWkstaUser();
        ret = EnumWkstaUser(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Connection")) == 0) {
        //ret = EnumConnection(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Server")) == 0) {
        ret = EnumServer(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("ServerDisk")) == 0) {
        ret = EnumServerDisk(--argc, ++argv);
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("Resource")) == 0) {
        LPNETRESOURCE lpnr = nullptr;
        ret = EnumResource(lpnr);
    } else {
        ret = Usage(argv[0]);
    }

    return ret;
}
