#include "pch.h"
#include "Wlan.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


void DumpWlanInterfaceState(WLAN_INTERFACE_STATE isState)
{
    switch (isState) {
    case wlan_interface_state_not_ready:
        wprintf(L"Not ready\n");
        break;
    case wlan_interface_state_connected:
        wprintf(L"Connected\n");
        break;
    case wlan_interface_state_ad_hoc_network_formed:
        wprintf(L"First node in a ad hoc network\n");
        break;
    case wlan_interface_state_disconnecting:
        wprintf(L"Disconnecting\n");
        break;
    case wlan_interface_state_disconnected:
        wprintf(L"Not connected\n");
        break;
    case wlan_interface_state_associating:
        wprintf(L"Attempting to associate with a network\n");
        break;
    case wlan_interface_state_discovering:
        wprintf(L"Auto configuration is discovering settings for the network\n");
        break;
    case wlan_interface_state_authenticating:
        wprintf(L"In process of authenticating\n");
        break;
    default:
        wprintf(L"Unknown state %ld\n", isState);
        break;
    }
}


void Dumpdot11AuthAlgorithm(PWLAN_AVAILABLE_NETWORK pBssEntry)
{
    switch (pBssEntry->dot11DefaultAuthAlgorithm) {
    case DOT11_AUTH_ALGO_80211_OPEN:
        wprintf(L"802.11 Open (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
        wprintf(L"802.11 Shared (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    case DOT11_AUTH_ALGO_WPA:
        wprintf(L"WPA (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    case DOT11_AUTH_ALGO_WPA_PSK:
        wprintf(L"WPA-PSK (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    case DOT11_AUTH_ALGO_WPA_NONE:
        wprintf(L"WPA-None (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    case DOT11_AUTH_ALGO_RSNA:
        wprintf(L"RSNA (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    case DOT11_AUTH_ALGO_RSNA_PSK:
        wprintf(L"RSNA with PSK(%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    default:
        wprintf(L"Other (%d)\n", pBssEntry->dot11DefaultAuthAlgorithm);
        break;
    }
}


void Dumpdot11CipherAlgorithm(PWLAN_AVAILABLE_NETWORK pBssEntry)
{
    switch (pBssEntry->dot11DefaultCipherAlgorithm) {
    case DOT11_CIPHER_ALGO_NONE:
        wprintf(L"None (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    case DOT11_CIPHER_ALGO_WEP40:
        wprintf(L"WEP-40 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    case DOT11_CIPHER_ALGO_TKIP:
        wprintf(L"TKIP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    case DOT11_CIPHER_ALGO_CCMP:
        wprintf(L"CCMP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    case DOT11_CIPHER_ALGO_WEP104:
        wprintf(L"WEP-104 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    case DOT11_CIPHER_ALGO_WEP:
        wprintf(L"WEP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    default:
        wprintf(L"Other (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
        break;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


int EnumWlanInterfaces()
/*
The following example enumerates the wireless LAN interfaces on the local computer and
prints values from the retrieved WLAN_INTERFACE_INFO_LIST structure and 
the enumerated WLAN_INTERFACE_INFO structures.

Note  This example will fail to load on Windows Server 2008 and
Windows Server 2008 R2 if the Wireless LAN Service is not installed and started.

经测试：这个显示的只是已经连接的WIFI。

https://docs.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanenuminterfaces
*/
{
    // Declare and initialize variables.
    HANDLE hClient = nullptr;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = WlanOpenHandle(dwMaxClient, nullptr, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    }

    /* variables used for WlanEnumInterfaces  */
    PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
    dwResult = WlanEnumInterfaces(hClient, nullptr, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    } else {
        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
            PWLAN_INTERFACE_INFO pIfInfo = reinterpret_cast<WLAN_INTERFACE_INFO *>(&pIfList->InterfaceInfo[i]);
            wprintf(L"  Interface Index[%d]:\t %ld\n", i, i);

            WCHAR GuidString[40] = {0};
            int iRet = StringFromGUID2(pIfInfo->InterfaceGuid, reinterpret_cast<LPOLESTR>(&GuidString), 39);
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39); 
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString);
            }
            wprintf(L"  Interface Description[%d]: %ws", i, pIfInfo->strInterfaceDescription);
            wprintf(L"\n");
            wprintf(L"  Interface State[%d]:\t ", i);
            DumpWlanInterfaceState(pIfInfo->isState);
            wprintf(L"\n");
        }
    }

    if (pIfList != nullptr) {
        WlanFreeMemory(pIfList);
        pIfList = nullptr;
    }

    return 0;
}


int EnumWlanAvailableNetwork()
/*
Examples
The following example enumerates the wireless LAN interfaces on the local computer,
retrieves the list of available networks on each wireless LAN interface,
and prints values from the retrieved WLAN_AVAILABLE_NETWORK_LIST that contains the WLAN_AVAILABLE_NETWORK entries.

Note  This example will fail to load on Windows Server 2008 and Windows Server 2008 R2 if the Wireless LAN Service is not installed and started.

经测试：这是显示所有的发现的WIFI。

https://docs.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlangetavailablenetworklist
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlangetavailablenetworklist
*/
{
    // Declare and initialize variables.
    HANDLE hClient = nullptr;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;
    WCHAR GuidString[39] = {0};
    unsigned int i{}, j{}, k{};

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
    PWLAN_INTERFACE_INFO pIfInfo = nullptr;

    PWLAN_AVAILABLE_NETWORK_LIST pBssList = nullptr;
    PWLAN_AVAILABLE_NETWORK pBssEntry = nullptr;

    int iRSSI = 0;

    dwResult = WlanOpenHandle(dwMaxClient, nullptr, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }

    dwResult = WlanEnumInterfaces(hClient, nullptr, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    } else {
        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (i = 0; i < pIfList->dwNumberOfItems; i++) {
            pIfInfo = reinterpret_cast<WLAN_INTERFACE_INFO *>(&pIfList->InterfaceInfo[i]);
            wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, reinterpret_cast<LPOLESTR>(&GuidString),
                                   sizeof(GuidString) / sizeof(*GuidString));
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
            //     sizeof(GuidString)/sizeof(*GuidString)); 
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  InterfaceGUID[%u]: %ws\n", i, GuidString);
            }
            wprintf(L"  Interface Description[%u]: %ws", i, pIfInfo->strInterfaceDescription);
            wprintf(L"\n");
            wprintf(L"  Interface State[%u]:\t ", i);
            DumpWlanInterfaceState(pIfInfo->isState);
            wprintf(L"\n");

            dwResult = WlanGetAvailableNetworkList(hClient, &pIfInfo->InterfaceGuid, 0, nullptr, &pBssList);
            if (dwResult != ERROR_SUCCESS) {
                wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n", dwResult);
                dwRetVal = 1;
                // You can use FormatMessage to find out why the function failed
            } else {
                wprintf(L"WLAN_AVAILABLE_NETWORK_LIST for this interface\n");

                wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);
                for (j = 0; j < pBssList->dwNumberOfItems; j++) {
                    pBssEntry = reinterpret_cast<WLAN_AVAILABLE_NETWORK *>(&pBssList->Network[j]);

                    wprintf(L"  Profile Name[%u]:  %ws\n", j, pBssEntry->strProfileName);

                    wprintf(L"  SSID[%u]:\t\t ", j);
                    if (pBssEntry->dot11Ssid.uSSIDLength == 0)
                        wprintf(L"\n");
                    else {
                        //for (k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
                        //    wprintf(L"%c", pBssEntry->dot11Ssid.ucSSID[k]);//这个有乱码，估计是UTF8.
                        //}

                        _ASSERTE(pBssEntry->dot11Ssid.uSSIDLength < _ARRAYSIZE(pBssEntry->dot11Ssid.ucSSID));
                        LPWSTR ucSSID = UTF8ToWideChar((PCHAR)pBssEntry->dot11Ssid.ucSSID);
                        wprintf(L"%s", ucSSID);
                        HeapFree(GetProcessHeap(), 0, ucSSID);

                        wprintf(L"\n");
                    }

                    wprintf(L"  BSS Network type[%u]:\t ", j);
                    switch (pBssEntry->dot11BssType) {
                    case dot11_BSS_type_infrastructure:
                        wprintf(L"Infrastructure (%d)\n", pBssEntry->dot11BssType);
                        break;
                    case dot11_BSS_type_independent:
                        wprintf(L"Infrastructure (%d)\n", pBssEntry->dot11BssType);
                        break;
                    default:
                        wprintf(L"Other (%d)\n", pBssEntry->dot11BssType);
                        break;
                    }

                    wprintf(L"  Number of BSSIDs[%u]:\t %u\n", j, pBssEntry->uNumberOfBssids);

                    wprintf(L"  Connectable[%u]:\t ", j);
                    if (pBssEntry->bNetworkConnectable)
                        wprintf(L"Yes\n");
                    else {
                        wprintf(L"No\n");
                        wprintf(L"  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j,
                                pBssEntry->wlanNotConnectableReason);
                    }

                    wprintf(L"  Number of PHY types supported[%u]:\t %u\n", j, pBssEntry->uNumberOfPhyTypes);

                    if (pBssEntry->wlanSignalQuality == 0)
                        iRSSI = -100;
                    else if (pBssEntry->wlanSignalQuality == 100)
                        iRSSI = -50;
                    else
                        iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);

                    wprintf(L"  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j, 
                            pBssEntry->wlanSignalQuality, iRSSI);

                    wprintf(L"  Security Enabled[%u]:\t ", j);
                    if (pBssEntry->bSecurityEnabled)
                        wprintf(L"Yes\n");
                    else
                        wprintf(L"No\n");

                    wprintf(L"  Default AuthAlgorithm[%u]: ", j);
                    Dumpdot11AuthAlgorithm(pBssEntry);

                    wprintf(L"  Default CipherAlgorithm[%u]: ", j);
                    Dumpdot11CipherAlgorithm(pBssEntry);

                    wprintf(L"  Flags[%u]:\t 0x%x", j, pBssEntry->dwFlags);
                    if (pBssEntry->dwFlags) {
                        if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                            wprintf(L" - Currently connected");
                        if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
                            wprintf(L" - Has profile");
                    }

                    wprintf(L"\n");
                    wprintf(L"\n");
                }
            }
        }
    }

    if (pBssList != nullptr) {
        WlanFreeMemory(pBssList);
        pBssList = nullptr;
    }

    if (pIfList != nullptr) {
        WlanFreeMemory(pIfList);
        pIfList = nullptr;
    }

    return dwRetVal;
}


int _cdecl GetWlanProfile(LPCWSTR pProfileName) //int argc, WCHAR ** argv
/*
以下示例枚举本地计算机上的无线 LAN 接口，检索每个无线 LAN 接口上特定无线配置文件的信息，并打印检索到的值。 
还会打印查询配置文件的 XML 表示形式的字符串。

注意 如果未安装并启动无线 LAN 服务，本示例将无法在 Windows Server 2008 和 Windows Server 2008 R2 上加载。

https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlangetprofile
*/
{
    // Declare and initialize variables.
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;      //    
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;

    WCHAR GuidString[39] = {0};

    unsigned int i;

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    //LPCWSTR pProfileName = NULL;
    LPWSTR pProfileXml = NULL;
    DWORD dwFlags = 0;
    DWORD dwGrantedAccess = 0;

    // Validate the parameters
    //if (argc < 2) {
    //    wprintf(L"usage: %s <profile>\n", argv[0]);
    //    wprintf(L"   Gets a wireless profile\n");
    //    wprintf(L"   Example\n");
    //    wprintf(L"       %s \"Default Wireless\"\n", argv[0]);
    //    exit(1);
    //}
    //pProfileName = argv[1];

    wprintf(L"Information for profile: %ws\n\n", pProfileName);

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    } else {
        wprintf(L"WLAN_INTERFACE_INFO_LIST for this system\n");

        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n\n", pIfList->dwIndex);
        for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
            pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
            wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, 
                                   sizeof(GuidString) / sizeof(*GuidString));
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
            //     sizeof(GuidString)/sizeof(*GuidString)); 
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString);
            }
            wprintf(L"  Interface Description[%d]: %ws", i, pIfInfo->strInterfaceDescription);
            wprintf(L"\n");
            wprintf(L"  Interface State[%d]:\t ", i);
            DumpWlanInterfaceState(pIfInfo->isState);
            wprintf(L"\n\n");

            dwResult = WlanGetProfile(hClient,
                                      &pIfInfo->InterfaceGuid,
                                      pProfileName,
                                      NULL,
                                      &pProfileXml,
                                      &dwFlags,
                                      &dwGrantedAccess);
            if (dwResult != ERROR_SUCCESS) {
                wprintf(L"WlanGetProfile failed with error: %u\n", dwResult);
                // You can use FormatMessage to find out why the function failed
            } else {
                wprintf(L"  Profile Name:  %ws\n", pProfileName);

                wprintf(L"  Profile XML string:\n");
                wprintf(L"%ws\n\n", pProfileXml);//可解密出密码。

                wprintf(L"  dwFlags:\t    0x%x", dwFlags);
                //                    if (dwFlags & WLAN_PROFILE_GET_PLAINTEXT_KEY)
                //                        wprintf(L"   Get Plain Text Key");
                if (dwFlags & WLAN_PROFILE_GROUP_POLICY)
                    wprintf(L"  Group Policy");
                if (dwFlags & WLAN_PROFILE_USER)
                    wprintf(L"  Per User Profile");
                wprintf(L"\n");

                wprintf(L"  dwGrantedAccess:  0x%x", dwGrantedAccess);
                if (dwGrantedAccess & WLAN_READ_ACCESS)
                    wprintf(L"  Read access");
                if (dwGrantedAccess & WLAN_EXECUTE_ACCESS)
                    wprintf(L"  Execute access");
                if (dwGrantedAccess & WLAN_WRITE_ACCESS)
                    wprintf(L"  Write access");
                wprintf(L"\n");

                wprintf(L"\n");
            }
        }
    }

    if (pProfileXml != NULL) {
        WlanFreeMemory(pProfileXml);
        pProfileXml = NULL;
    }

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    return dwRetVal;
}


int EnumWlanProfile()
/*
以下示例枚举本地计算机上的无线 LAN 接口，检索每个无线 LAN 接口上的配置文件列表，
并从包含WLAN_PROFILE_INFO条目的检索WLAN_PROFILE_INFO_LIST打印值。

注意 如果未安装并启动无线 LAN 服务，此示例将无法在 Windows Server 2008 和 Windows Server 2008 R2 上加载。

https://learn.microsoft.com/zh-CN/windows/win32/api/wlanapi/nf-wlanapi-wlangetprofilelist
*/
{
    // Declare and initialize variables.
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2; 
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;

    WCHAR GuidString[39] = {0};

    unsigned int i, j;

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    PWLAN_PROFILE_INFO_LIST pProfileList = NULL;
    PWLAN_PROFILE_INFO pProfile = NULL;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    } else {
        wprintf(L"WLAN_INTERFACE_INFO_LIST for this system\n");

        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
            pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
            wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
                                   sizeof(GuidString) / sizeof(*GuidString));
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
            //     sizeof(GuidString)/sizeof(*GuidString)); 
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  Interface GUID[%d]: %ws\n", i, GuidString);
            }
            wprintf(L"  Interface Description[%d]: %ws", i, pIfInfo->strInterfaceDescription);
            wprintf(L"\n");
            wprintf(L"  Interface State[%d]:\t ", i);
            DumpWlanInterfaceState(pIfInfo->isState);
            wprintf(L"\n");

            dwResult = WlanGetProfileList(hClient, &pIfInfo->InterfaceGuid, NULL, &pProfileList);
            if (dwResult != ERROR_SUCCESS) {
                wprintf(L"WlanGetProfileList failed with error: %u\n", dwResult);
                dwRetVal = 1;
                // You can use FormatMessage to find out why the function failed
            } else {
                wprintf(L"WLAN_PROFILE_INFO_LIST for this interface\n");

                wprintf(L"  Num Entries: %lu\n\n", pProfileList->dwNumberOfItems);

                for (j = 0; j < pProfileList->dwNumberOfItems; j++) {
                    pProfile = (WLAN_PROFILE_INFO *)&pProfileList->ProfileInfo[j];

                    wprintf(L"  Profile Name[%u]:  %ws\n", j, pProfile->strProfileName);
                    GetWlanProfile(pProfile->strProfileName);

                    wprintf(L"  Flags[%u]:\t    0x%x", j, pProfile->dwFlags);
                    if (pProfile->dwFlags & WLAN_PROFILE_GROUP_POLICY)
                        wprintf(L"   Group Policy");
                    if (pProfile->dwFlags & WLAN_PROFILE_USER)
                        wprintf(L"   Per User Profile");
                    wprintf(L"\n");

                    wprintf(L"\n");
                }
            }
        }
    }

    if (pProfileList != NULL) {
        WlanFreeMemory(pProfileList);
        pProfileList = NULL;
    }

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    return dwRetVal;
}


int EnumWlanNetworkBss()
/*
WlanGetNetworkBssList 函数检索 (BSS) 给定无线 LAN 接口上无线网络或网络的条目的基本服务集的列表。

The WlanGetNetworkBssList function retrieves a list of the basic service set (BSS) entries of the wireless network or networks on a given wireless LAN interface.

WLAN_BSS_LIST 结构包含基本服务集的列表 (BSS) 条目。

https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlangetnetworkbsslist
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/ns-wlanapi-wlan_bss_list
https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/ns-wlanapi-wlan_bss_entry
*/
{
    // Declare and initialize variables.
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;

    WCHAR GuidString[39] = {0};

    unsigned int i, j;

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    } else {
        wprintf(L"WLAN_INTERFACE_INFO_LIST for this system\n");

        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
            pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
            wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
                                   sizeof(GuidString) / sizeof(*GuidString));
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
            //     sizeof(GuidString)/sizeof(*GuidString)); 
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  Interface GUID[%d]: %ws\n", i, GuidString);
            }
            wprintf(L"  Interface Description[%d]: %ws", i, pIfInfo->strInterfaceDescription);
            wprintf(L"\n");
            wprintf(L"  Interface State[%d]:\t ", i);
            DumpWlanInterfaceState(pIfInfo->isState);
            wprintf(L"\n");

            PWLAN_BSS_LIST WlanBssList = NULL;
            dwResult = WlanGetNetworkBssList(hClient, &pIfInfo->InterfaceGuid, NULL, (DOT11_BSS_TYPE)0, FALSE, NULL, &WlanBssList);
            if (dwResult != ERROR_SUCCESS) {
                wprintf(L"WlanGetNetworkBssList failed with error: %u\n", dwResult);
                dwRetVal = 1;
                // You can use FormatMessage to find out why the function failed
            } else {
                wprintf(L"  WLAN_BSS_LIST for this interface\n");
                wprintf(L"  Num Entries: %lu\n\n", WlanBssList->dwNumberOfItems);

                for (j = 0; j < WlanBssList->dwNumberOfItems; j++) {
                    wprintf(L"  PhyId: %lu\n\n", WlanBssList->wlanBssEntries[j].uPhyId);

                    //以后闲的时候补充打印输出。

                    wprintf(L"\n");
                }

                if (WlanBssList != NULL) {
                    WlanFreeMemory(WlanBssList);
                    WlanBssList = NULL;
                }
            }
        }
    }

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    return dwRetVal;
}


int EnumWlanFilter(_In_ WLAN_FILTER_LIST_TYPE wlanFilterListType)
/*
WlanGetFilterList 函数检索组策略或用户权限列表。

https://learn.microsoft.com/zh-cn/windows/win32/api/wlanapi/nf-wlanapi-wlangetfilterlist
*/
{
    // Declare and initialize variables.
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;

    WCHAR GuidString[39] = {0};

    unsigned int j;

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }

    PDOT11_NETWORK_LIST NetworkList = NULL;
    dwResult = WlanGetFilterList(hClient, wlanFilterListType, NULL, &NetworkList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanGetFilterList failed with error: %u\n", dwResult);
        dwRetVal = 1;
        // You can use FormatMessage to find out why the function failed
    } else {
        if (NetworkList) {//经测试这里始终是NULL。
            wprintf(L"  DOT11_NETWORK_LIST for this interface\n");
            wprintf(L"  Num Entries: %lu\n\n", NetworkList->dwNumberOfItems);

            for (j = 0; j < NetworkList->dwNumberOfItems; j++) {
                wprintf(L"  dot11BssType: %lu\n\n", NetworkList->Network[j].dot11BssType);

                _ASSERTE(NetworkList->Network[j].dot11Ssid.uSSIDLength < _ARRAYSIZE(NetworkList->Network[j].dot11Ssid.ucSSID));
                LPWSTR ucSSID = UTF8ToWideChar((PCHAR)NetworkList->Network[j].dot11Ssid.ucSSID);
                wprintf(L"  SSID:%s\n", ucSSID);
                HeapFree(GetProcessHeap(), 0, ucSSID);

                wprintf(L"\n");
            }

            WlanFreeMemory(NetworkList);
        }
    }

    return dwRetVal;
}


int EnumWlanFilter()
{
    EnumWlanFilter(wlan_filter_list_type_gp_permit);
    EnumWlanFilter(wlan_filter_list_type_gp_deny);
    EnumWlanFilter(wlan_filter_list_type_user_permit);
    EnumWlanFilter(wlan_filter_list_type_user_deny);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


//\reactos\base\applications\network\wlanconf\wlanconf.c


//////////////////////////////////////////////////////////////////////////////////////////////////


static int Usage(__in const wchar_t * name)
/*++
Routine Description:
    This routine prints the intended usage for this program.
Arguments:
    progName - NULL terminated string representing the name of the executable
--*/
{
    wprintf(L"查看连接的WIFI: %s EnumWlanInterfaces.\n", name);
    wprintf(L"查看发现的WIFI: %s EnumWlanAvailableNetwork.\n", name);
    wprintf(L"查看用过的WlanProfile: %s EnumWlanProfile.\n", name);
    wprintf(L"GetWlanNetworkBssList: %s EnumWlanNetworkBss.\n", name);
    wprintf(L"GetWlanFilterList: %s EnumWlanFilter.\n", name);

    return ERROR_SUCCESS;
}


int wlan(int argc, wchar_t * argv[])
{
    int ret = ERROR_SUCCESS;

    if (argc == 1) {
        return Usage(argv[0]);
    }

    if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWlanInterfaces")) == 0) {
        ret = EnumWlanInterfaces();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWlanAvailableNetwork")) == 0) {
        ret = EnumWlanAvailableNetwork();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWlanProfile")) == 0) {
        ret = EnumWlanProfile();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWlanNetworkBss")) == 0) {
        ret = EnumWlanNetworkBss();
    } else if (argc == 2 && lstrcmpi(argv[1], TEXT("EnumWlanFilter")) == 0) {
        ret = EnumWlanFilter();
    } else {
        ret = Usage(argv[0]);
    }

    return ret;
}
