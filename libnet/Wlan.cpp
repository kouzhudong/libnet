#include "pch.h"
#include "Wlan.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


int WlanEnum()
/*
The following example enumerates the wireless LAN interfaces on the local computer and
prints values from the retrieved WLAN_INTERFACE_INFO_LIST structure and the enumerated WLAN_INTERFACE_INFO structures.

Note  This example will fail to load on Windows Server 2008 and
Windows Server 2008 R2 if the Wireless LAN Service is not installed and started.

https://docs.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanenuminterfaces
*/
{
    // Declare and initialize variables.
    HANDLE hClient = nullptr;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    int iRet = 0;
    WCHAR GuidString[40] = {0};

    /* variables used for WlanEnumInterfaces  */
    PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
    PWLAN_INTERFACE_INFO pIfInfo = nullptr;

    dwResult = WlanOpenHandle(dwMaxClient, nullptr, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    }

    dwResult = WlanEnumInterfaces(hClient, nullptr, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    } else {
        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
            pIfInfo = reinterpret_cast<WLAN_INTERFACE_INFO *>(&pIfList->InterfaceInfo[i]);
            wprintf(L"  Interface Index[%d]:\t %ld\n", i, i);
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, reinterpret_cast<LPOLESTR>(&GuidString), 39);
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
            switch (pIfInfo->isState) {
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
                wprintf(L"Unknown state %ld\n", pIfInfo->isState);
                break;
            }
            wprintf(L"\n");
        }
    }

    if (pIfList != nullptr) {
        WlanFreeMemory(pIfList);
        pIfList = nullptr;
    }
    return 0;
}


int WlanEnumEx()
/*
Examples
The following example enumerates the wireless LAN interfaces on the local computer,
retrieves the list of available networks on each wireless LAN interface,
and prints values from the retrieved WLAN_AVAILABLE_NETWORK_LIST that contains the WLAN_AVAILABLE_NETWORK entries.

Note  This example will fail to load on Windows Server 2008 and Windows Server 2008 R2 if the Wireless LAN Service is not installed and started.

https://docs.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlangetavailablenetworklist
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
            switch (pIfInfo->isState) {
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
                wprintf(L"Unknown state %ld\n", pIfInfo->isState);
                break;
            }
            wprintf(L"\n");

            dwResult = WlanGetAvailableNetworkList(hClient,
                                                   &pIfInfo->InterfaceGuid,
                                                   0,
                                                   nullptr,
                                                   &pBssList);
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
                        for (k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
                            wprintf(L"%c", pBssEntry->dot11Ssid.ucSSID[k]);
                        }
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

                    wprintf(L"  Default CipherAlgorithm[%u]: ", j);
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


//////////////////////////////////////////////////////////////////////////////////////////////////
