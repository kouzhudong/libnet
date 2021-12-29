#include "pch.h"
#include "Firewall.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


HRESULT WFCOMInitialize(INetFwPolicy2 ** ppNetFwPolicy2)
// Instantiate INetFwPolicy2
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(__uuidof(NetFwPolicy2),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwPolicy2),
                          (void **)ppNetFwPolicy2);
    if (FAILED(hr)) {
        wprintf(L"CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:
    return hr;
}


void WFCOMCleanup(INetFwPolicy2 * pNetFwPolicy2)
// Release INetFwPolicy2
{
    // Release the INetFwPolicy2 object (Vista+)
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a firewall rule with
 EdgeTraversalOptions (one of the EdgeTraversalOptions values).

********************************************************************/


#define STRING_BUFFER_SIZE  500     


// Forward declarations
HRESULT    AddFirewallRuleWithEdgeTraversal(__in INetFwPolicy2 * pNetFwPolicy2);


int __cdecl DynamicEdgeTraversal()
/*
Adding an Application Rule to Allow Dynamic Edge Traversal

05/31/2018

This example adds an application rule to allow dynamic edge traversal using the Windows Firewall with Advanced Security APIs.

In order for Windows Firewall to dynamically allow edge traversal traffic, the following two items must be done:

The application must use the IPV6_PROTECTION_LEVEL socket option on the listening socket and set it to PROTECTION_LEVEL_UNRESTRICTED when the application wants to allow edge traffic.
The Windows Firewall rule added for the application must set edge traversal option to NET_FW_EDGE_TRAVERSAL_TYPE_DEFER_TO_APP.
This sample only illustrates one of the edge traversal option values.
For more information on these values, see NET_FW_EDGE_TRAVERSAL_TYPE.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-an-application-rule-edge-traversal
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            wprintf(L"CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Add firewall rule with EdgeTraversalOption=DeferApp (Windows7+) if available 
    //   else add with Edge=True (Vista and Server 2008).
    AddFirewallRuleWithEdgeTraversal(pNetFwPolicy2);

Cleanup:

    // Release INetFwPolicy2
    WFCOMCleanup(pNetFwPolicy2);

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


HRESULT    AddFirewallRuleWithEdgeTraversal(__in INetFwPolicy2 * pNetFwPolicy2)
// Add firewall rule with EdgeTraversalOption=DeferApp (Windows7+) if available 
//   else add with Edge=True (Vista and Server 2008).
{
    HRESULT hr = S_OK;
    INetFwRules * pNetFwRules = NULL;
    INetFwRule * pNetFwRule = NULL;
    INetFwRule2 * pNetFwRule2 = NULL;
    WCHAR pwszTemp[STRING_BUFFER_SIZE] = L"";
    BSTR RuleName = NULL;
    BSTR RuleGroupName = NULL;
    BSTR RuleDescription = NULL;
    BSTR RuleAppPath = NULL;

    //  For localization purposes, the rule name, description, and group can be 
    //    provided as indirect strings. These indirect strings can be defined in an rc file.
    //  Examples of the indirect string definitions in the rc file -
    //    127                     "EdgeTraversalOptions Sample Application"
    //    128                     "Allow inbound TCP traffic to application EdgeTraversalOptions.exe"
    //    129                     "Allow EdgeTraversalOptions.exe to receive inbound traffic for TCP protocol 
    //                          from remote machines located within your network as well as from 
    //                          the Internet (i.e from outside of your Edge device like Firewall or NAT"

    //    Examples of using indirect strings -
    //    hr = StringCchPrintfW(pwszTemp, STRING_BUFFER_SIZE, L"@EdgeTraversalOptions.exe,-128");
    hr = StringCchPrintfW(pwszTemp,
                          STRING_BUFFER_SIZE,
                          L"Allow inbound TCP traffic to application EdgeTraversalOptions.exe");
    if (FAILED(hr)) {
        wprintf(L"Failed to compose a resource identifier string: 0x%08lx\n", hr);
        goto Cleanup;
    }
    RuleName = SysAllocString(pwszTemp);
    if (NULL == RuleName) {
        wprintf(L"\nERROR: Insufficient memory\n");
        goto Cleanup;
    }
    //    Examples of using indirect strings -
    //    hr = StringCchPrintfW(pwszTemp, STRING_BUFFER_SIZE, L"@EdgeTraversalOptions.exe,-127");
    hr = StringCchPrintfW(pwszTemp, STRING_BUFFER_SIZE, L"EdgeTraversalOptions Sample Application");
    if (FAILED(hr)) {
        wprintf(L"Failed to compose a resource identifier string: 0x%08lx\n", hr);
        goto Cleanup;
    }
    RuleGroupName = SysAllocString(pwszTemp);  // Used for grouping together multiple rules
    if (NULL == RuleGroupName) {
        wprintf(L"\nERROR: Insufficient memory\n");
        goto Cleanup;
    }
    //    Examples of using indirect strings -
    //    hr = StringCchPrintfW(pwszTemp, STRING_BUFFER_SIZE, L"@EdgeTraversalOptions.exe,-129");
    hr = StringCchPrintfW(pwszTemp, STRING_BUFFER_SIZE, L"Allow EdgeTraversalOptions.exe to receive \
       inbound traffic for TCP protocol from remote machines located within your network as well as \
       from the Internet (i.e from outside of your Edge device like Firewall or NAT)");
    if (FAILED(hr)) {
        wprintf(L"Failed to compose a resource identifier string: 0x%08lx\n", hr);
        goto Cleanup;
    }
    RuleDescription = SysAllocString(pwszTemp);
    if (NULL == RuleDescription) {
        wprintf(L"\nERROR: Insufficient memory\n");
        goto Cleanup;
    }

    RuleAppPath = SysAllocString(L"%ProgramFiles%\\EdgeTraversalOptions\\EdgeTraversalOptions.exe");
    if (NULL == RuleAppPath) {
        wprintf(L"\nERROR: Insufficient memory\n");
        goto Cleanup;
    }

    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr)) {
        wprintf(L"Failed to retrieve firewall rules collection : 0x%08lx\n", hr);
        goto Cleanup;
    }

    hr = CoCreateInstance(
        __uuidof(NetFwRule),    //CLSID of the class whose object is to be created
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwRule),   // Identifier of the Interface used for communicating with the object
        (void **)&pNetFwRule);
    if (FAILED(hr)) {
        wprintf(L"CoCreateInstance for INetFwRule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Name(RuleName);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Name failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Grouping(RuleGroupName);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Grouping failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Description(RuleDescription);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Description failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Direction(NET_FW_RULE_DIR_IN);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Direction failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Action(NET_FW_ACTION_ALLOW);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Action failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_ApplicationName(RuleAppPath);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_ApplicationName failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Protocol(6);  // TCP
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Protocol failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Profiles(NET_FW_PROFILE2_DOMAIN | NET_FW_PROFILE2_PRIVATE);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Profiles failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    hr = pNetFwRule->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr)) {
        wprintf(L"Failed INetFwRule::put_Enabled failed with error: 0x %x.\n", hr);
        goto Cleanup;
    }

    // Check if INetFwRule2 interface is available (i.e Windows7+)
    // If supported, then use EdgeTraversalOptions
    // Else use the EdgeTraversal boolean flag.

    if (SUCCEEDED(pNetFwRule->QueryInterface(__uuidof(INetFwRule2), (void **)&pNetFwRule2))) {
        hr = pNetFwRule2->put_EdgeTraversalOptions(NET_FW_EDGE_TRAVERSAL_TYPE_DEFER_TO_APP);
        if (FAILED(hr)) {
            wprintf(L"Failed INetFwRule::put_EdgeTraversalOptions failed with error: 0x %x.\n", hr);
            goto Cleanup;
        }
    } else {
        hr = pNetFwRule->put_EdgeTraversal(VARIANT_TRUE);
        if (FAILED(hr)) {
            wprintf(L"Failed INetFwRule::put_EdgeTraversal failed with error: 0x %x.\n", hr);
            goto Cleanup;
        }
    }

    hr = pNetFwRules->Add(pNetFwRule);
    if (FAILED(hr)) {
        wprintf(L"Failed to add firewall rule to the firewall rules collection : 0x%08lx\n", hr);
        goto Cleanup;
    }

    wprintf(L"Successfully added firewall rule !\n");

Cleanup:

    SysFreeString(RuleName);
    SysFreeString(RuleGroupName);
    SysFreeString(RuleDescription);
    SysFreeString(RuleAppPath);

    if (pNetFwRule2 != NULL) {
        pNetFwRule2->Release();
    }

    if (pNetFwRule != NULL) {
        pNetFwRule->Release();
    }

    if (pNetFwRules != NULL) {
        pNetFwRules->Release();
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code for enumerating Windows Firewall
    rules using the Microsoft Windows Firewall APIs.

********************************************************************/


void DumpFWRulesInCollection(INetFwRule * FwRule)
// Output properties of a Firewall rule 
{
    variant_t InterfaceArray;
    variant_t InterfaceString;
    VARIANT_BOOL bEnabled;
    BSTR bstrVal;
    long lVal = 0;
    long lProfileBitmask = 0;
    NET_FW_RULE_DIRECTION fwDirection;
    NET_FW_ACTION fwAction;

    struct ProfileMapElement
    {
        NET_FW_PROFILE_TYPE2 Id;
        LPCWSTR Name;
    };

    ProfileMapElement ProfileMap[3];
    ProfileMap[0].Id = NET_FW_PROFILE2_DOMAIN;
    ProfileMap[0].Name = L"Domain";
    ProfileMap[1].Id = NET_FW_PROFILE2_PRIVATE;
    ProfileMap[1].Name = L"Private";
    ProfileMap[2].Id = NET_FW_PROFILE2_PUBLIC;
    ProfileMap[2].Name = L"Public";

    wprintf(L"---------------------------------------------\n");

    if (SUCCEEDED(FwRule->get_Name(&bstrVal))) {
        wprintf(L"Name:             %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_Description(&bstrVal))) {
        wprintf(L"Description:      %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_ApplicationName(&bstrVal))) {
        wprintf(L"Application Name: %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_ServiceName(&bstrVal))) {
        wprintf(L"Service Name:     %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_Protocol(&lVal))) {
        switch (lVal) {
        case NET_FW_IP_PROTOCOL_TCP:
            wprintf(L"IP Protocol:      %s\n", NET_FW_IP_PROTOCOL_TCP_NAME);
            break;
        case NET_FW_IP_PROTOCOL_UDP:
            wprintf(L"IP Protocol:      %s\n", NET_FW_IP_PROTOCOL_UDP_NAME);
            break;
        default:
            break;
        }

        if (lVal != NET_FW_IP_VERSION_V4 && lVal != NET_FW_IP_VERSION_V6) {
            if (SUCCEEDED(FwRule->get_LocalPorts(&bstrVal))) {
                wprintf(L"Local Ports:      %s\n", bstrVal);
            }

            if (SUCCEEDED(FwRule->get_RemotePorts(&bstrVal))) {
                wprintf(L"Remote Ports:      %s\n", bstrVal);
            }
        } else {
            if (SUCCEEDED(FwRule->get_IcmpTypesAndCodes(&bstrVal))) {
                wprintf(L"ICMP TypeCode:      %s\n", bstrVal);
            }
        }
    }

    if (SUCCEEDED(FwRule->get_LocalAddresses(&bstrVal))) {
        wprintf(L"LocalAddresses:   %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_RemoteAddresses(&bstrVal))) {
        wprintf(L"RemoteAddresses:  %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_Profiles(&lProfileBitmask))) {
        // The returned bitmask can have more than 1 bit set if multiple profiles 
        //   are active or current at the same time

        for (int i = 0; i < 3; i++) {
            if (lProfileBitmask & ProfileMap[i].Id) {
                wprintf(L"Profile:  %s\n", ProfileMap[i].Name);
            }
        }
    }

    if (SUCCEEDED(FwRule->get_Direction(&fwDirection))) {
        switch (fwDirection) {
        case NET_FW_RULE_DIR_IN:
            wprintf(L"Direction:        %s\n", NET_FW_RULE_DIR_IN_NAME);
            break;
        case NET_FW_RULE_DIR_OUT:
            wprintf(L"Direction:        %s\n", NET_FW_RULE_DIR_OUT_NAME);
            break;
        default:
            break;
        }
    }

    if (SUCCEEDED(FwRule->get_Action(&fwAction))) {
        switch (fwAction) {
        case NET_FW_ACTION_BLOCK:
            wprintf(L"Action:           %s\n", NET_FW_RULE_ACTION_BLOCK_NAME);
            break;
        case NET_FW_ACTION_ALLOW:
            wprintf(L"Action:           %s\n", NET_FW_RULE_ACTION_ALLOW_NAME);
            break;
        default:
            break;
        }
    }

    if (SUCCEEDED(FwRule->get_Interfaces(&InterfaceArray))) {
        if (InterfaceArray.vt != VT_EMPTY) {
            SAFEARRAY * pSa = NULL;

            pSa = InterfaceArray.parray;

            for (long index = pSa->rgsabound->lLbound; index < (long)pSa->rgsabound->cElements; index++) {
                SafeArrayGetElement(pSa, &index, &InterfaceString);
                wprintf(L"Interfaces:       %s\n", (BSTR)InterfaceString.bstrVal);
            }
        }
    }

    if (SUCCEEDED(FwRule->get_InterfaceTypes(&bstrVal))) {
        wprintf(L"Interface Types:  %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_Enabled(&bEnabled))) {
        if (bEnabled) {
            wprintf(L"Enabled:          %s\n", NET_FW_RULE_ENABLE_IN_NAME);
        } else {
            wprintf(L"Enabled:          %s\n", NET_FW_RULE_DISABLE_IN_NAME);
        }
    }

    if (SUCCEEDED(FwRule->get_Grouping(&bstrVal))) {
        wprintf(L"Grouping:         %s\n", bstrVal);
    }

    if (SUCCEEDED(FwRule->get_EdgeTraversal(&bEnabled))) {
        if (bEnabled) {
            wprintf(L"Edge Traversal:   %s\n", NET_FW_RULE_ENABLE_IN_NAME);
        } else {
            wprintf(L"Edge Traversal:   %s\n", NET_FW_RULE_DISABLE_IN_NAME);
        }
    }
}


int __cdecl EnumeratingFirewallRules()
/*
Enumerating Firewall Rules

This example enumerates firewall rules using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-enumerating-firewall-rules
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    ULONG cFetched = 0;
    CComVariant var;
    IUnknown * pEnumerator;
    IEnumVARIANT * pVariant = NULL;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    long fwRuleCount;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            wprintf(L"CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        wprintf(L"get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Obtain the number of Firewall rules
    hr = pFwRules->get_Count(&fwRuleCount);
    if (FAILED(hr)) {
        wprintf(L"get_Count failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    wprintf(L"The number of rules in the Windows Firewall are %d\n", fwRuleCount);

    // Iterate through all of the rules in pFwRules
    pFwRules->get__NewEnum(&pEnumerator);
    if (pEnumerator) {
        hr = pEnumerator->QueryInterface(__uuidof(IEnumVARIANT), (void **)&pVariant);
    }

    while (SUCCEEDED(hr) && hr != S_FALSE) {
        var.Clear();
        hr = pVariant->Next(1, &var, &cFetched);
        if (S_FALSE != hr) {
            if (SUCCEEDED(hr)) {
                hr = var.ChangeType(VT_DISPATCH);
            }
            if (SUCCEEDED(hr)) {
                hr = (V_DISPATCH(&var))->QueryInterface(__uuidof(INetFwRule), reinterpret_cast<void **>(&pFwRule));
            }

            if (SUCCEEDED(hr)) {
                // Output the properties of this rule
                DumpFWRulesInCollection(pFwRule);
            }
        }
    }

Cleanup:

    // Release pFwRule
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code for disabling Windows Firewall per
    Interface.

--********************************************************************/


int __cdecl DisablingFirewallPerInterface()
/*
Disabling a Firewall Per Interface

05/31/2018

This example disables a firewall per interface using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-disabling-a-firewall-per-interface
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    variant_t      vtInterfaceName("Local Area Connection"), vtInterface;
    long           index = 0;
    SAFEARRAY * pSa = NULL;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve Local Interface
    pSa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    if (!pSa)
        _com_issue_error(E_OUTOFMEMORY);
    else {
        hr = SafeArrayPutElement(pSa, &index, &vtInterfaceName);
        if FAILED(hr)
            _com_issue_error(hr);
        vtInterface.vt = VT_ARRAY | VT_VARIANT;
        vtInterface.parray = pSa;
    }

    // Disable Windows Firewall for the local interface (Public profile)
    hr = pNetFwPolicy2->put_ExcludedInterfaces(NET_FW_PROFILE2_PRIVATE, vtInterface);
    if (FAILED(hr)) {
        printf("put_ExcludedInterfaces failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code for disabling Windows Firewall
    per profile using the Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl DisablingFirewall()
/*
Disabling Windows Firewall

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-disabling-windows-firewall

经测试OK。

This example disables Windows Firewall using the Windows Firewall with Advanced Security APIs.
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Disable Windows Firewall for the Domain profile
    hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, FALSE);
    if (FAILED(hr)) {
        printf("put_FirewallEnabled failed for Domain: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Disable Windows Firewall for the Private profile
    hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, FALSE);
    if (FAILED(hr)) {
        printf("put_FirewallEnabled failed for Private: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Disable Windows Firewall for the Public profile
    hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, FALSE);
    if (FAILED(hr)) {
        printf("put_FirewallEnabled failed for Public: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code for enabling Windows Firewall
    exception groups using the Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl EnablingGroup()
/*
Enabling a Group

05/31/2018

This example enables a group using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-enabling-a-group
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;

    // Rule group to use
    BSTR bstrRuleGroup = SysAllocString(L"File and Printer Sharing");
    VARIANT_BOOL bIsEnabled = FALSE;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Check if the rule group is enabled for the Private profile
    hr = pNetFwPolicy2->IsRuleGroupEnabled(NET_FW_PROFILE2_PRIVATE, bstrRuleGroup, &bIsEnabled);
    if (FAILED(hr)) {
        printf("IsRuleGroupEnabled failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    if (!bIsEnabled) {
        // Group is not enabled for the Private profile - need to enable it
        printf("Rule Group is NOT enabled for the Private profile. Calling EnableRuleGroup...");

        hr = pNetFwPolicy2->EnableRuleGroup(NET_FW_PROFILE2_PRIVATE, bstrRuleGroup, TRUE);
        if (FAILED(hr)) {
            printf("EnableRuleGroup failed: 0x%08lx\n", hr);
            goto Cleanup;
        }
    } else {
        printf("Rule Group is enabled for the Private profile");
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleGroup);

    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


// Forward declarations
void        Get_FirewallSettings_PerProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed,
                                                INetFwPolicy2 * pNetFwPolicy2);


int __cdecl GettingFirewallSettings()
/*
Getting Firewall Settings

This example gets firewall settings using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-getting-firewall-settings

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code for reading Windows Firewall
    Settings per profile using the Microsoft Windows Firewall APIs.
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    printf("Settings for the firewall domain profile:\n");
    Get_FirewallSettings_PerProfileType(NET_FW_PROFILE2_DOMAIN, pNetFwPolicy2);

    printf("Settings for the firewall private profile:\n");
    Get_FirewallSettings_PerProfileType(NET_FW_PROFILE2_PRIVATE, pNetFwPolicy2);

    printf("Settings for the firewall public profile:\n");
    Get_FirewallSettings_PerProfileType(NET_FW_PROFILE2_PUBLIC, pNetFwPolicy2);

Cleanup:

    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


void Get_FirewallSettings_PerProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed,
                                         INetFwPolicy2 * pNetFwPolicy2)
{
    VARIANT_BOOL bIsEnabled = FALSE;
    NET_FW_ACTION action;

    printf("******************************************\n");

    if (SUCCEEDED(pNetFwPolicy2->get_FirewallEnabled(ProfileTypePassed, &bIsEnabled))) {
        printf("Firewall is %s\n", bIsEnabled ? "enabled" : "disabled");
    }

    if (SUCCEEDED(pNetFwPolicy2->get_BlockAllInboundTraffic(ProfileTypePassed, &bIsEnabled))) {
        printf("Block all inbound traffic is %s\n", bIsEnabled ? "enabled" : "disabled");
    }

    if (SUCCEEDED(pNetFwPolicy2->get_NotificationsDisabled(ProfileTypePassed, &bIsEnabled))) {
        printf("Notifications are %s\n", bIsEnabled ? "disabled" : "enabled");
    }

    if (SUCCEEDED(pNetFwPolicy2->get_UnicastResponsesToMulticastBroadcastDisabled(ProfileTypePassed, &bIsEnabled))) {
        printf("UnicastResponsesToMulticastBroadcast is %s\n", bIsEnabled ? "disabled" : "enabled");
    }

    if (SUCCEEDED(pNetFwPolicy2->get_DefaultInboundAction(ProfileTypePassed, &action))) {
        printf("Default inbound action is %s\n", action != NET_FW_ACTION_BLOCK ? "Allow" : "Block");
    }

    if (SUCCEEDED(pNetFwPolicy2->get_DefaultOutboundAction(ProfileTypePassed, &action))) {
        printf("Default outbound action is %s\n", action != NET_FW_ACTION_BLOCK ? "Allow" : "Block");
    }

    printf("\n");
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that restricts a service using
    the Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl RestrictingService()
/*
Restricting Service

This example restricts service using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-restricting-service
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    VARIANT_BOOL isServiceRestricted = FALSE;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwServiceRestriction * pFwServiceRestriction = NULL;

    // The Service and App name to use
    BSTR bstrServiceName = SysAllocString(L"SampleService");   // provide a valid service short name here.
    BSTR bstrAppName = SysAllocString(L"%systemDrive%\\WINDOWS\\system32\\svchost.exe");
    // The rule name, description should be provided as indirect strings '@appfullpath,-resource index' for
    // localization purposes. 
    // Using the strings directly for illustration here.
    BSTR bstrRuleName = SysAllocString(L"Allow TCP 12345 to sampleservice");
    BSTR bstrRuleDescription = SysAllocString(L"Allow only TCP 12345 traffic to sampleservice service, block everything else");
    BSTR bstrRuleLPorts = SysAllocString(L"12345");

    // Error checking for BSTR allocations
    if (NULL == bstrServiceName) { printf("Failed to allocate bstrServiceName\n"); goto Cleanup; }
    if (NULL == bstrAppName) { printf("Failed to allocate bstrAppName\n"); goto Cleanup; }
    if (NULL == bstrRuleName) { printf("Failed to allocate bstrRuleName\n"); goto Cleanup; }
    if (NULL == bstrRuleDescription) { printf("Failed to allocate bstrRuleDescription\n"); goto Cleanup; }
    if (NULL == bstrRuleLPorts) { printf("Failed to allocate bstrRuleLPorts\n"); goto Cleanup; }

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwServiceRestriction
    hr = pNetFwPolicy2->get_ServiceRestriction(&pFwServiceRestriction);
    if (FAILED(hr)) {
        printf("get_ServiceRestriction failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Restrict the sampleservice Service.
    // This will add two WSH rules -
    //    - a default block all inbound traffic to the service
    //    - a default block all outbound traffic from the service
    hr = pFwServiceRestriction->RestrictService(bstrServiceName, bstrAppName, TRUE, FALSE);
    if (FAILED(hr)) {
        printf("RestrictService failed: 0x%08lx\nMake sure you specified a valid service shortname.\n", hr);
        goto Cleanup;
    }

    // If the service does not send/receive any network traffic then you are done. You can skip adding the allow WSH rules below.

    // If the service requires sending/receiving certain traffic, then add 'allow' WSH rules as follows

    // Get the collections of Windows Service Hardening networking rules first
    hr = pFwServiceRestriction->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        wprintf(L"get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Add inbound WSH allow rule for allowing TCP 12345 to the service
    // Create a new Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Rule Name
    hr = pFwRule->put_Name(bstrRuleName);
    if (FAILED(hr)) {
        printf("put_Name failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Rule Description
    hr = pFwRule->put_Description(bstrRuleDescription);
    if (FAILED(hr)) {
        printf("put_Description failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Application Name
    hr = pFwRule->put_ApplicationName(bstrAppName);
    if (FAILED(hr)) {
        printf("put_ApplicationName failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Service Name
    hr = pFwRule->put_ServiceName(bstrServiceName);
    if (FAILED(hr)) {
        printf("put_ServiceName failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Protocol
    hr = pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
    if (FAILED(hr)) {
        printf("put_Protocol failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Local Ports
    hr = pFwRule->put_LocalPorts(bstrRuleLPorts);
    if (FAILED(hr)) {
        printf("put_LocalPorts failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the rule Action
    hr = pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    if (FAILED(hr)) {
        printf("put_Action failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the rule Enabled setting
    hr = pFwRule->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr)) {
        printf("put_Enabled failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Add the Rule to the collection of Windows Service Hardening(WSH) rules
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    Sleep(3000);

    // Check to see if the Service is Restricted
    hr = pFwServiceRestriction->ServiceRestricted(bstrServiceName, bstrAppName, &isServiceRestricted);
    if (FAILED(hr)) {
        printf("ServiceRestricted failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    if (isServiceRestricted) {
        printf("Service was successfully restricted in WSH.\nExcept for TCP 12345 inbound traffic and its responses, all other inbound and outbound connections to and from the service will be blocked.\n");
    } else {
        printf("The Service could not be properly restricted.\n");
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrServiceName);
    SysFreeString(bstrAppName);
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleLPorts);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a GRE rule for the
    currently active profiles using the Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl AddGreRule()
/*
Adding a GRE Rule

This example adds a GRE rule using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-a-gre-rule
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    long CurrentProfilesBitMask = 0;
    BSTR bstrRuleName = SysAllocString(L"GRE_RULE");
    BSTR bstrRuleDescription = SysAllocString(L"Allow GRE Traffic");
    BSTR bstrRuleGroup = SysAllocString(L"Sample Rule Group");

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve Current Profiles bitmask
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr)) {
        printf("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile, we remove it from the bitmask
    if ((CurrentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (CurrentProfilesBitMask != NET_FW_PROFILE2_PUBLIC)) {
        CurrentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_Protocol(47);
    pFwRule->put_Grouping(bstrRuleGroup);
    pFwRule->put_Profiles(CurrentProfilesBitMask);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a ICMP rule
    for the Private profile using the Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl AddIcmpRule()
/*
Adding an ICMP Rule

05/31/2018

This example adds an ICMP rule using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-an-icmp-rule
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    BSTR bstrRuleName = SysAllocString(L"ICMP_RULE");
    BSTR bstrRuleDescription = SysAllocString(L"Allow ICMP network traffic");
    BSTR bstrRuleGroup = SysAllocString(L"Sample Rule Group");
    // ICMP Echo Request
    BSTR bstrICMPTypeCode = SysAllocString(L"8:*");

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_Protocol(1);
    pFwRule->put_IcmpTypesAndCodes(bstrICMPTypeCode);
    pFwRule->put_Grouping(bstrRuleGroup);
    pFwRule->put_Profiles(NET_FW_PROFILE2_PRIVATE);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);
    SysFreeString(bstrICMPTypeCode);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a LAN rule to the
    currently active profiles using the Microsoft Windows Firewall
    APIs.

--********************************************************************/


int __cdecl AddLanRule()
/*
Adding a LAN Rule

05/31/2018

This example adds a LAN rule using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-a-lan-rule
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    long CurrentProfilesBitMask = 0;
    BSTR bstrRuleName = SysAllocString(L"PER_INTERFACETYPE_RULE");
    BSTR bstrRuleDescription = SysAllocString(L"Allow incoming network traffic over port 2400 coming from LAN interface type");
    BSTR bstrRuleGroup = SysAllocString(L"Sample Rule Group");
    BSTR bstrRuleLPorts = SysAllocString(L"2400-2450");
    BSTR bstrRuleInterfaceType = SysAllocString(L"LAN");

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve Current Profiles bitmask
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr)) {
        printf("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile, we remove it from the bitmask
    if ((CurrentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (CurrentProfilesBitMask != NET_FW_PROFILE2_PUBLIC)) {
        CurrentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
    pFwRule->put_LocalPorts(bstrRuleLPorts);
    pFwRule->put_Grouping(bstrRuleGroup);
    pFwRule->put_InterfaceTypes(bstrRuleInterfaceType);
    pFwRule->put_Profiles(CurrentProfilesBitMask);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);
    SysFreeString(bstrRuleLPorts);
    SysFreeString(bstrRuleInterfaceType);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a outbound rule for
    the currently active profiles to allow a TCP connection using the
    Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl AddOutboundRule()
/*
Adding an Outbound Rule

This example adds an outbound rule using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-an-outbound-rule
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    long CurrentProfilesBitMask = 0;
    BSTR bstrRuleName = SysAllocString(L"OUTBOUND_RULE");
    BSTR bstrRuleDescription = SysAllocString(L"Allow outbound network traffic from my Application over TCP port 4000");
    BSTR bstrRuleGroup = SysAllocString(L"Sample Rule Group");
    BSTR bstrRuleApplication = SysAllocString(L"%programfiles%\\MyApplication.exe");
    BSTR bstrRuleLPorts = SysAllocString(L"4000");

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve Current Profiles bitmask
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr)) {
        printf("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile, we remove it from the bitmask
    if ((CurrentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (CurrentProfilesBitMask != NET_FW_PROFILE2_PUBLIC)) {
        CurrentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_ApplicationName(bstrRuleApplication);
    pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
    pFwRule->put_LocalPorts(bstrRuleLPorts);
    pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
    pFwRule->put_Grouping(bstrRuleGroup);
    pFwRule->put_Profiles(CurrentProfilesBitMask);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);
    SysFreeString(bstrRuleApplication);
    SysFreeString(bstrRuleLPorts);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a rule per interface
    for the currently active profiles using the Microsoft Windows
    Firewall APIs.

--********************************************************************/


int __cdecl AddPerInterfaceRule()
/*
Adding a Per Interface Rule

05/31/2018

This example adds a per interface rule using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-a-per-interface-rule
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    variant_t      vtInterfaceName("Local Area Connection"), vtInterface;
    long           index = 0;
    SAFEARRAY * pSa = NULL;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    long CurrentProfilesBitMask = 0;
    BSTR bstrRuleName = SysAllocString(L"PER_INTERFACE_RULE");
    BSTR bstrRuleDescription = SysAllocString(L"Add a PER_INTERFACE rule");
    BSTR bstrRuleGroup = SysAllocString(L"Sample Rule Group");
    BSTR bstrRuleLPorts = SysAllocString(L"2300");

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve Current Profiles bitmask
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr)) {
        printf("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile, we remove it from the bitmask
    if ((CurrentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (CurrentProfilesBitMask != NET_FW_PROFILE2_PUBLIC)) {
        CurrentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }

    // Retrieve Local Interface
    pSa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    if (!pSa)
        _com_issue_error(E_OUTOFMEMORY);
    else {
        hr = SafeArrayPutElement(pSa, &index, &vtInterfaceName);
        if FAILED(hr)
            _com_issue_error(hr);
        vtInterface.vt = VT_ARRAY | VT_VARIANT;
        vtInterface.parray = pSa;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_Enabled(VARIANT_TRUE);
    pFwRule->put_Profiles(CurrentProfilesBitMask);
    pFwRule->put_Grouping(bstrRuleGroup);
    pFwRule->put_Interfaces(vtInterface);
    pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
    pFwRule->put_LocalPorts(bstrRuleLPorts);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);
    SysFreeString(bstrRuleLPorts);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
    This C++ file includes sample code that adds a Service rule for the
    currently active profiles using the Microsoft Windows Firewall APIs.

--********************************************************************/


int __cdecl AddServiceRule()
/*
Adding a Service Rule

05/31/2018

This example adds a service rule using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-adding-a-service-rule
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;
    INetFwRules * pFwRules = NULL;
    INetFwRule * pFwRule = NULL;
    long CurrentProfilesBitMask = 0;
    BSTR bstrRuleName = SysAllocString(L"SERVICE_RULE");
    BSTR bstrRuleDescription = SysAllocString(L"Allow incoming network traffic to myservice");
    BSTR bstrRuleGroup = SysAllocString(L"Sample Rule Group");
    BSTR bstrRuleApplication = SysAllocString(L"%systemroot%\\system32\\myservice.exe");
    BSTR bstrRuleService = SysAllocString(L"myservicename");
    BSTR bstrRuleLPorts = SysAllocString(L"135");

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve Current Profiles bitmask
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr)) {
        printf("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile, we remove it from the bitmask
    if ((CurrentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (CurrentProfilesBitMask != NET_FW_PROFILE2_PUBLIC)) {
        CurrentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(__uuidof(NetFwRule),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwRule),
                          (void **)&pFwRule);
    if (FAILED(hr)) {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_ApplicationName(bstrRuleApplication);
    pFwRule->put_ServiceName(bstrRuleService);
    pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
    pFwRule->put_LocalPorts(bstrRuleLPorts);
    pFwRule->put_Grouping(bstrRuleGroup);
    pFwRule->put_Profiles(CurrentProfilesBitMask);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);
    SysFreeString(bstrRuleApplication);
    SysFreeString(bstrRuleService);
    SysFreeString(bstrRuleLPorts);

    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
  This C++ file includes sample code that takes ownership
    of the NET_FW_RULE_CATEGORY_FIREWALL using the Microsoft
    Windows Firewall APIs.

    The API to register for NET_FW_RULE_CATEGORY_FIREWALL
    needs the binary that is making this call to be linked
    with /integritycheck option to ensure code integrity. Failure
    to do so can lead to error SEC_E_CANNOT_INSTALL at runtime.

    For more details on code integrity read
    http://msdn2.microsoft.com/library/ms680339.aspx

--********************************************************************/


#define BAIL_ON_ALLOC_FAILURE(ptr, fnName) \
   if ((ptr) == NULL) \
   { \
      result = ERROR_NOT_ENOUGH_MEMORY; \
      printf(#fnName " = ERROR_NOT_ENOUGH_MEMORY\n"); \
      goto CLEANUP; \
   }


// Forward declarations
DWORD ArrayOfLongsToVariant(
    __in unsigned long numItems,
    __in_ecount(numItems) const long * items,
    __out VARIANT * dst);


void __cdecl RWFTOFPM()
/*
Registering with Windows Firewall And Taking Ownership of Firewall Policy Management

05/31/2018

This example registers a product with Windows Firewall and takes ownership of Firewall policy management using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-registering-with-windows-firewall-ownership
*/
{
    DWORD result = NO_ERROR;
    HRESULT hr = S_OK;
    INetFwProduct * product = NULL;
    INetFwProducts * products = NULL;
    IUnknown * registration = NULL;
    long * categories = NULL;
    BSTR displayName = NULL;
    VARIANT varCategories = {VT_EMPTY};
    int numberOfCategories = 1;
    long count = 0;
    BOOL comInit = FALSE;

    //Allocate Memory
    categories = (long *)calloc(numberOfCategories, sizeof(long));
    BAIL_ON_ALLOC_FAILURE(categories, calloc);

    //Take Firewall Category Ownership
    categories[0] = NET_FW_RULE_CATEGORY_FIREWALL;
    result = ArrayOfLongsToVariant(numberOfCategories, categories, &varCategories);

    //  For localization purposes, the display name can be 
    //    provided as an indirect string. The indirect strings can be defined in an rc file.
    //  Examples of the indirect string definition in the rc file -
    //    127                     "A Test Firewall Product"
    //    displayName = SysAllocString(L"@RegisterFirewallSample.exe,-127");

    displayName = SysAllocString(L"A Test Firewall Product");
    BAIL_ON_ALLOC_FAILURE(displayName, SysAllocString);

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        //COM initialize failed
        wprintf(L"CoInitialize failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }
    comInit = TRUE;


    hr = CoCreateInstance(__uuidof(NetFwProduct),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwProduct),
                          (void **)&product);
    if (FAILED(hr)) {
        //CoCreateInstance Failed
        wprintf(L"CoCreateInstance for INetFwProduct failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = product->put_DisplayName(displayName);
    if (FAILED(hr)) {
        //Put_displayName failed
        wprintf(L"put_DisplayName for INetFwProduct failed Error: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = product->put_RuleCategories(varCategories);
    if (FAILED(hr)) {
        //Put_rulecategories failed
        wprintf(L"put_RuleCategories failed for INetFwProduct Error: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = CoCreateInstance(__uuidof(NetFwProducts),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwProducts),
                          (void **)&products);
    if (FAILED(hr)) {
        //CoCreateInstance Failed
        wprintf(L"CoCreateInstance for INetFwProducts failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = products->Register(product, &registration);
    if (FAILED(hr)) {
        //Failed to Register Products
        wprintf(L"Register failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = products->get_Count(&count);
    if (FAILED(hr)) {
        //Failed to get Count of Products
        wprintf(L"Get count failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }
    wprintf(L"INetFwProducts_Count returned %ld.\n", count);

    wprintf(L"Hit any key to unregister.\n");

    //_getch();

CLEANUP:
    if (registration != NULL) {
        registration->Release();
    }
    if (products != NULL) {
        products->Release();
    }
    if (product != NULL) {
        product->Release();
    }
    if (comInit) {
        CoUninitialize();
    }
    free(categories);
    SysFreeString(displayName);
    VariantClear(&varCategories);
    return;
}


DWORD ArrayOfLongsToVariant(
    __in unsigned long numItems,
    __in_ecount(numItems) const long * items,
    __out VARIANT * dst
)
//This Function Converts and Array of Longs to Variant
{
    DWORD result = NO_ERROR;
    SAFEARRAYBOUND bound[1];
    SAFEARRAY * sa = NULL;
    VARIANT * data;
    unsigned long i;

    VariantInit(dst);

    // If there are no items, just return VT_EMPTY.
    if (numItems == 0) {
        goto CLEANUP;
    }

    bound[0].lLbound = 0;
    bound[0].cElements = numItems;

    sa = SafeArrayCreate(VT_VARIANT, ARRAYSIZE(bound), bound);
    BAIL_ON_ALLOC_FAILURE(sa, SafeArrayCreate);

    data = (VARIANT *)(sa->pvData);

    for (i = 0; i < numItems; ++i) {
        V_VT(data + i) = VT_I4;
        V_I4(data + i) = items[i];
    }

    V_VT(dst) = VT_ARRAY | VT_VARIANT;
    V_ARRAY(dst) = sa;

CLEANUP:
    return result;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************++
Copyright (C) Microsoft. All Rights Reserved.

Abstract:
  This C++ file includes sample code that registers itself with the
    Windows Firewall using the Microsoft Windows Firewall APIs but does
    not take ownership of any NET_FW_RULE_CATEGORY.

    The API to register for NET_FW_RULE_CATEGORY_FIREWALL
    needs the binary that is making this call to be linked
    with /integritycheck option to ensure code integrity. Failure
    to do so can lead to error SEC_E_CANNOT_INSTALL at runtime.

    For more details on code integrity read
    http://msdn2.microsoft.com/library/ms680339.aspx

--********************************************************************/


#define BAIL_ON_ALLOC_FAILURE_2(ptr, fnName) \
   if ((ptr) == NULL) \
   { \
      printf(#fnName " = ERROR_NOT_ENOUGH_MEMORY\n"); \
      goto CLEANUP; \
   }


void __cdecl RWFWTOFPM()
/*
Registering with Windows Firewall Without Taking Ownership of Firewall Policy Management

05/31/2018

This example registers a product with Windows Firewall without taking ownership of Firewall policy management using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/c-registering-with-windows-firewall-no-ownership
*/
{
    HRESULT hr = S_OK;
    INetFwProduct * product = NULL;
    INetFwProducts * products = NULL;
    IUnknown * registration = NULL;
    BSTR displayName = NULL;
    VARIANT varCategories = {VT_EMPTY};
    long count = 0;
    BOOL comInit = FALSE;

    //  For localization purposes, the display name can be 
    //    provided as an indirect string. The indirect strings can be defined in an rc file.
    //  Examples of the indirect string definition in the rc file -
    //    127                     "A Test Firewall Product"
    //    displayName = SysAllocString(L"@RegisterWithoutCategoryOwnership.exe,-127");

    displayName = SysAllocString(L"A Test Firewall Product");
    BAIL_ON_ALLOC_FAILURE_2(displayName, SysAllocString);

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        //COM initialize failed
        wprintf(L"CoInitialize failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }
    comInit = TRUE;

    hr = CoCreateInstance(__uuidof(NetFwProduct),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwProduct),
                          (void **)&product);
    if (FAILED(hr)) {
        //CoCreateInstance Failed
        wprintf(L"CoCreateInstance for INetFwProduct failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = product->put_DisplayName(displayName);
    if (FAILED(hr)) {
        //Put_displayName failed
        wprintf(L"put_DisplayName for INetFwProduct failed Error: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = product->put_RuleCategories(varCategories);
    if (FAILED(hr)) {
        //Put_rulecategories failed
        wprintf(L"put_RuleCategories failed for INetFwProduct Error: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = CoCreateInstance(__uuidof(NetFwProducts),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwProducts),
                          (void **)&products);
    if (FAILED(hr)) {
        //CoCreateInstance Failed
        wprintf(L"CoCreateInstance for INetFwProducts failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = products->Register(product, &registration);
    if (FAILED(hr)) {
        //Failed to Register Products
        wprintf(L"Register failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }

    hr = products->get_Count(&count);
    if (FAILED(hr)) {
        //Failed to get Count of Products
        wprintf(L"Get count failed: 0x%08lx\n", hr);
        goto CLEANUP;
    }
    wprintf(L"INetFwProducts_Count returned %ld.\n", count);

    wprintf(L"Hit any key to unregister.\n");

    //_getch();

CLEANUP:
    if (registration != NULL) {
        registration->Release();
    }
    if (products != NULL) {
        products->Release();
    }
    if (product != NULL) {
        product->Release();
    }
    if (comInit) {
        CoUninitialize();
    }

    SysFreeString(displayName);
    VariantClear(&varCategories);
    return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
    Copyright (C) Microsoft. All rights reserved.

    SYNOPSIS

        Sample code for 'Windows Firewall with Advanced Security' COM interfaces.

        Illustrates correct usage of following methods/properties of INetFwPolicy2 COM interface
        in the presence of multiple firewall profiles:
          - CurrentProfileTypes
          - IsRuleGroupCurrentlyEnabled
          - IsRuleGroupEnabled
          - LocalPolicyModifyState
*/


// Forward declarations
HRESULT    GetCurrentFirewallState(__in INetFwPolicy2 * pNetFwPolicy2);
HRESULT    IsRuleGroupEnabled(__in INetFwPolicy2 * pNetFwPolicy2);
HRESULT    IsRuleGroupCurrentlyEnabled(__in INetFwPolicy2 * pNetFwPolicy2);
HRESULT    GetLocalPolicyModifyState(__in INetFwPolicy2 * pNetFwPolicy2);


int __cdecl MultipleProfiles()
/*
Working with Multiple Profiles

This example shows how to perform several tasks using the Windows Firewall with Advanced Security APIs.

https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ics/working-with-multiple-profiles
*/
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;
    INetFwPolicy2 * pNetFwPolicy2 = NULL;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED(hrComInit)) {
            wprintf(L"CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr)) {
        goto Cleanup;
    }

    // Show Firewall ON/OFF state on current profiles
    GetCurrentFirewallState(pNetFwPolicy2);

    // Show status of 'File and Printer Sharing' rule group on current profiles
    IsRuleGroupCurrentlyEnabled(pNetFwPolicy2);

    // Show status of 'File and Printer Sharing' rule group on specified profiles
    IsRuleGroupEnabled(pNetFwPolicy2);

    // For the current firewall profiles display whether the changes to firewall rules 
    //  will take effect or not
    GetLocalPolicyModifyState(pNetFwPolicy2);

Cleanup:

    // Release INetFwPolicy2
    WFCOMCleanup(pNetFwPolicy2);

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}


// For the currently active firewall profiles display whether firewall is ON or OFF
HRESULT GetCurrentFirewallState(__in INetFwPolicy2 * pNetFwPolicy2)
{
    HRESULT hr = S_FALSE;
    long    CurrentProfilesBitMask = 0;
    VARIANT_BOOL bActualFirewallEnabled = VARIANT_FALSE;
    struct ProfileMapElement
    {
        NET_FW_PROFILE_TYPE2 Id;
        LPCWSTR Name;
    };
    ProfileMapElement ProfileMap[3];
    ProfileMap[0].Id = NET_FW_PROFILE2_DOMAIN;
    ProfileMap[0].Name = L"Domain";
    ProfileMap[1].Id = NET_FW_PROFILE2_PRIVATE;
    ProfileMap[1].Name = L"Private";
    ProfileMap[2].Id = NET_FW_PROFILE2_PUBLIC;
    ProfileMap[2].Name = L"Public";

    wprintf(L"\n\nCurrent Firewall State:\n");
    wprintf(L"-----------------------\n");

    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr)) {
        wprintf(L"Failed to get CurrentProfileTypes. Error: %x.\n", hr);
        goto CLEANUP;
    }

    // The returned 'CurrentProfiles' bitmask can have more than 1 bit set if multiple profiles 
    //   are active or current at the same time

    for (int i = 0; i < 3; i++) {
        if (CurrentProfilesBitMask & ProfileMap[i].Id) {
            /*Is Firewall Enabled?*/
            hr = pNetFwPolicy2->get_FirewallEnabled(ProfileMap[i].Id, &bActualFirewallEnabled);
            if (FAILED(hr)) {
                wprintf(L"Failed to get FirewallEnabled settings for %s profile. Error: %x.\n", ProfileMap[i].Name, hr);
                goto CLEANUP;
            }
            wprintf(L"On %s profile (Current) : Firewall state is %s\n",
                    ProfileMap[i].Name,
                    (bActualFirewallEnabled ? L"ON" : L"OFF"));
        }
    }


CLEANUP:
    return hr;
}


// For the currently active firewall profiles display whether the rule group is enabled or not
HRESULT IsRuleGroupCurrentlyEnabled(__in INetFwPolicy2 * pNetFwPolicy2)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL bActualEnabled = VARIANT_FALSE;
    BSTR GroupName = SysAllocString(L"File and Printer Sharing");

    wprintf(L"\n\nIs 'File and Printer Sharing' rule group currently enabled ?\n");
    wprintf(L"------------------------------------------------------------\n");

    hr = pNetFwPolicy2->get_IsRuleGroupCurrentlyEnabled(GroupName, &bActualEnabled);
    if (SUCCEEDED(hr)) {
        if (VARIANT_TRUE == bActualEnabled && S_OK == hr) {
            wprintf(L"Rule Group currently enabled on all the current profiles\n");
        } else if (VARIANT_TRUE == bActualEnabled && S_FALSE == hr) {
            wprintf(L"Rule Group currently enabled on some of the current profiles but not on all the current profiles\n");
        } else if (VARIANT_FALSE == bActualEnabled) {
            wprintf(L"Rule Group Currently not enabled on any of the current profiles\n");
        }
    } else {
        wprintf(L"Failed calling API IsRuleGroupCurrentlyEnabled. Error: 0x %x.\n", hr);
        goto Cleanup;
    }

Cleanup:
    SysFreeString(GroupName);
    return hr;
}


// For the specified firewall profiles display whether the rule group is enabled or not
HRESULT IsRuleGroupEnabled(__in INetFwPolicy2 * pNetFwPolicy2)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL bActualEnabled = VARIANT_FALSE;
    BSTR GroupName = SysAllocString(L"File and Printer Sharing");

    wprintf(L"\n\nIs 'File and Printer Sharing' rule group enabled in public and private profiles ?\n");
    wprintf(L"---------------------------------------------------------------------------------\n");

    hr = pNetFwPolicy2->IsRuleGroupEnabled(NET_FW_PROFILE2_PRIVATE | NET_FW_PROFILE2_PUBLIC, GroupName, &bActualEnabled);
    if (SUCCEEDED(hr)) {
        if (VARIANT_TRUE == bActualEnabled && S_OK == hr) {
            wprintf(L"Rule Group currently enabled on both public and private profiles\n");
        } else if (VARIANT_TRUE == bActualEnabled && S_FALSE == hr) {
            wprintf(L"Rule Group currently enabled on either public or private profile but not both\n");
        } else if (VARIANT_FALSE == bActualEnabled) {
            wprintf(L"Rule Group currently disabled on both public and private profiles\n");
        }
    } else {
        wprintf(L"Failed calling API IsRuleGroupCurrentlyEnabled. Error: 0x %x.\n", hr);
        goto Cleanup;
    }

Cleanup:
    SysFreeString(GroupName);
    return hr;
}


HRESULT GetLocalPolicyModifyState(__in INetFwPolicy2 * pNetFwPolicy2)
// For the currently active firewall profiles display whether the changes to firewall rules will take effect or not
{
    HRESULT hr;
    NET_FW_MODIFY_STATE modifystate;

    wprintf(L"\n\nWill changes to firewall rules take effect ?\n");
    wprintf(L"--------------------------------------------\n");

    hr = pNetFwPolicy2->get_LocalPolicyModifyState(&modifystate);
    if (FAILED(hr)) {
        wprintf(L"Failed calling API get_LocalPolicyModifyState. Error: %x.\n", hr);
        return hr;
    }

    if (modifystate == NET_FW_MODIFY_STATE_OK) {
        if (hr == S_OK) {
            wprintf(L"Changing or adding firewall rule (or group) to the current profiles will take effect on all current profiles.\n");
        } else if (hr == S_FALSE) {
            wprintf(L"Changing or adding firewall rule (or group) to the current profiles will take effect on only some current profiles but not all.\n");
        }
    } else if (modifystate == NET_FW_MODIFY_STATE_GP_OVERRIDE) {
        if (hr == S_OK) {
            wprintf(L"Changing or adding a firewall rule (or group) to the current profiles will not take effect because group policy overrides it on all current profiles.\n");
        } else if (hr == S_FALSE) {
            wprintf(L"Changing or adding a firewall rule (or group) to the current profiles will not take effect because group policy overrides it on some of the current profiles.\n");
        }
    } else if (modifystate == NET_FW_MODIFY_STATE_INBOUND_BLOCKED) {
        if (hr == S_OK) {
            wprintf(L"Changing or adding firewall rule (or group) to the current profiles will not take effect because unsolicited inbound traffic is not allowed on all the current profiles.\n");
        } else if (hr == S_FALSE) {
            wprintf(L"Changing or adding firewall rule (or group) to the current profiles will not take effect because unsolicited inbound traffic is not allowed on some of the current profiles.\n");
        }
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
    Copyright (c) Microsoft Corporation

    SYNOPSIS

        Sample code for the Windows Firewall COM interface.
*/


HRESULT WindowsFirewallInitialize(OUT INetFwProfile ** fwProfile)
{
    HRESULT hr = S_OK;
    INetFwMgr * fwMgr = NULL;
    INetFwPolicy * fwPolicy = NULL;

    _ASSERT(fwProfile != NULL);

    *fwProfile = NULL;

    // Create an instance of the firewall settings manager.
    hr = CoCreateInstance(__uuidof(NetFwMgr),
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwMgr),
                          (void **)&fwMgr);
    if (FAILED(hr)) {
        printf("CoCreateInstance failed: 0x%08lx\n", hr);
        goto error;
    }

    // Retrieve the local firewall policy.
    hr = fwMgr->get_LocalPolicy(&fwPolicy);
    if (FAILED(hr)) {
        printf("get_LocalPolicy failed: 0x%08lx\n", hr);
        goto error;
    }

    // Retrieve the firewall profile currently in effect.
    hr = fwPolicy->get_CurrentProfile(fwProfile);
    if (FAILED(hr)) {
        printf("get_CurrentProfile failed: 0x%08lx\n", hr);
        goto error;
    }

error:

    // Release the local firewall policy.
    if (fwPolicy != NULL) {
        fwPolicy->Release();
    }

    // Release the firewall settings manager.
    if (fwMgr != NULL) {
        fwMgr->Release();
    }

    return hr;
}


void WindowsFirewallCleanup(IN INetFwProfile * fwProfile)
{
    // Release the firewall profile.
    if (fwProfile != NULL) {
        fwProfile->Release();
    }
}


HRESULT WindowsFirewallIsOn(IN INetFwProfile * fwProfile, OUT BOOL * fwOn)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwOn != NULL);

    *fwOn = FALSE;

    // Get the current state of the firewall.
    hr = fwProfile->get_FirewallEnabled(&fwEnabled);
    if (FAILED(hr)) {
        printf("get_FirewallEnabled failed: 0x%08lx\n", hr);
        goto error;
    }

    // Check to see if the firewall is on.
    if (fwEnabled != VARIANT_FALSE) {
        *fwOn = TRUE;
        printf("The firewall is on.\n");
    } else {
        printf("The firewall is off.\n");
    }

error:

    return hr;
}


HRESULT WindowsFirewallTurnOn(IN INetFwProfile * fwProfile)
{
    HRESULT hr = S_OK;
    BOOL fwOn;

    _ASSERT(fwProfile != NULL);

    // Check to see if the firewall is off.
    hr = WindowsFirewallIsOn(fwProfile, &fwOn);
    if (FAILED(hr)) {
        printf("WindowsFirewallIsOn failed: 0x%08lx\n", hr);
        goto error;
    }

    // If it is, turn it on.
    if (!fwOn) {
        // Turn the firewall on.
        hr = fwProfile->put_FirewallEnabled(VARIANT_TRUE);
        if (FAILED(hr)) {
            printf("put_FirewallEnabled failed: 0x%08lx\n", hr);
            goto error;
        }

        printf("The firewall is now on.\n");
    }

error:

    return hr;
}


HRESULT WindowsFirewallTurnOff(IN INetFwProfile * fwProfile)
{
    HRESULT hr = S_OK;
    BOOL fwOn;

    _ASSERT(fwProfile != NULL);

    // Check to see if the firewall is on.
    hr = WindowsFirewallIsOn(fwProfile, &fwOn);
    if (FAILED(hr)) {
        printf("WindowsFirewallIsOn failed: 0x%08lx\n", hr);
        goto error;
    }

    // If it is, turn it off.
    if (fwOn) {
        // Turn the firewall off.
        hr = fwProfile->put_FirewallEnabled(VARIANT_FALSE);
        if (FAILED(hr)) {
            printf("put_FirewallEnabled failed: 0x%08lx\n", hr);
            goto error;
        }

        printf("The firewall is now off.\n");
    }

error:

    return hr;
}


HRESULT WindowsFirewallAppIsEnabled(
    IN INetFwProfile * fwProfile,
    IN const wchar_t * fwProcessImageFileName,
    OUT BOOL * fwAppEnabled
)
{
    HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL fwEnabled;
    INetFwAuthorizedApplication * fwApp = NULL;
    INetFwAuthorizedApplications * fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwAppEnabled != NULL);

    *fwAppEnabled = FALSE;

    // Retrieve the authorized application collection.
    hr = fwProfile->get_AuthorizedApplications(&fwApps);
    if (FAILED(hr)) {
        printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
        goto error;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
    if (fwBstrProcessImageFileName == NULL) {
        hr = E_OUTOFMEMORY;
        printf("SysAllocString failed: 0x%08lx\n", hr);
        goto error;
    }

    // Attempt to retrieve the authorized application.
    hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
    if (SUCCEEDED(hr)) {
        // Find out if the authorized application is enabled.
        hr = fwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr)) {
            printf("get_Enabled failed: 0x%08lx\n", hr);
            goto error;
        }

        if (fwEnabled != VARIANT_FALSE) {
            // The authorized application is enabled.
            *fwAppEnabled = TRUE;

            printf("Authorized application %lS is enabled in the firewall.\n",
                   fwProcessImageFileName);
        } else {
            printf("Authorized application %lS is disabled in the firewall.\n",
                   fwProcessImageFileName);
        }
    } else {
        // The authorized application was not in the collection.
        hr = S_OK;

        printf("Authorized application %lS is disabled in the firewall.\n",
               fwProcessImageFileName);
    }

error:

    // Free the BSTR.
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL) {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL) {
        fwApps->Release();
    }

    return hr;
}


HRESULT WindowsFirewallAddApp(
    IN INetFwProfile * fwProfile,
    IN const wchar_t * fwProcessImageFileName,
    IN const wchar_t * fwName
)
{
    HRESULT hr = S_OK;
    BOOL fwAppEnabled;
    BSTR fwBstrName = NULL;
    BSTR fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplication * fwApp = NULL;
    INetFwAuthorizedApplications * fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwName != NULL);

    // First check to see if the application is already authorized.
    hr = WindowsFirewallAppIsEnabled(fwProfile, fwProcessImageFileName, &fwAppEnabled);
    if (FAILED(hr)) {
        printf("WindowsFirewallAppIsEnabled failed: 0x%08lx\n", hr);
        goto error;
    }

    // Only add the application if it isn't already authorized.
    if (!fwAppEnabled) {
        // Retrieve the authorized application collection.
        hr = fwProfile->get_AuthorizedApplications(&fwApps);
        if (FAILED(hr)) {
            printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
            goto error;
        }

        // Create an instance of an authorized application.
        hr = CoCreateInstance(__uuidof(NetFwAuthorizedApplication),
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              __uuidof(INetFwAuthorizedApplication),
                              (void **)&fwApp);
        if (FAILED(hr)) {
            printf("CoCreateInstance failed: 0x%08lx\n", hr);
            goto error;
        }

        // Allocate a BSTR for the process image file name.
        fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
        if (fwBstrProcessImageFileName == NULL) {
            hr = E_OUTOFMEMORY;
            printf("SysAllocString failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the process image file name.
        hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
        if (FAILED(hr)) {
            printf("put_ProcessImageFileName failed: 0x%08lx\n", hr);
            goto error;
        }

        // Allocate a BSTR for the application friendly name.
        fwBstrName = SysAllocString(fwName);
        if (SysStringLen(fwBstrName) == 0) {
            hr = E_OUTOFMEMORY;
            printf("SysAllocString failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the application friendly name.
        hr = fwApp->put_Name(fwBstrName);
        if (FAILED(hr)) {
            printf("put_Name failed: 0x%08lx\n", hr);
            goto error;
        }

        // Add the application to the collection.
        hr = fwApps->Add(fwApp);
        if (FAILED(hr)) {
            printf("Add failed: 0x%08lx\n", hr);
            goto error;
        }

        printf("Authorized application %lS is now enabled in the firewall.\n",
               fwProcessImageFileName);
    }

error:

    // Free the BSTRs.
    SysFreeString(fwBstrName);
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL) {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL) {
        fwApps->Release();
    }

    return hr;
}


HRESULT WindowsFirewallPortIsEnabled(IN INetFwProfile * fwProfile,
                                     IN LONG portNumber,
                                     IN NET_FW_IP_PROTOCOL ipProtocol,
                                     OUT BOOL * fwPortEnabled
)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;
    INetFwOpenPort * fwOpenPort = NULL;
    INetFwOpenPorts * fwOpenPorts = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwPortEnabled != NULL);

    *fwPortEnabled = FALSE;

    // Retrieve the globally open ports collection.
    hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
    if (FAILED(hr)) {
        printf("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
        goto error;
    }

    // Attempt to retrieve the globally open port.
    hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);
    if (SUCCEEDED(hr)) {
        // Find out if the globally open port is enabled.
        hr = fwOpenPort->get_Enabled(&fwEnabled);
        if (FAILED(hr)) {
            printf("get_Enabled failed: 0x%08lx\n", hr);
            goto error;
        }

        if (fwEnabled != VARIANT_FALSE) {
            // The globally open port is enabled.
            *fwPortEnabled = TRUE;

            printf("Port %ld is open in the firewall.\n", portNumber);
        } else {
            printf("Port %ld is not open in the firewall.\n", portNumber);
        }
    } else {
        // The globally open port was not in the collection.
        hr = S_OK;

        printf("Port %ld is not open in the firewall.\n", portNumber);
    }

error:

    // Release the globally open port.
    if (fwOpenPort != NULL) {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL) {
        fwOpenPorts->Release();
    }

    return hr;
}


HRESULT WindowsFirewallPortAdd(IN INetFwProfile * fwProfile,
                               IN LONG portNumber,
                               IN NET_FW_IP_PROTOCOL ipProtocol,
                               IN const wchar_t * name
)
{
    HRESULT hr = S_OK;
    BOOL fwPortEnabled;
    BSTR fwBstrName = NULL;
    INetFwOpenPort * fwOpenPort = NULL;
    INetFwOpenPorts * fwOpenPorts = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(name != NULL);

    // First check to see if the port is already added.
    hr = WindowsFirewallPortIsEnabled(fwProfile, portNumber, ipProtocol, &fwPortEnabled);
    if (FAILED(hr)) {
        printf("WindowsFirewallPortIsEnabled failed: 0x%08lx\n", hr);
        goto error;
    }

    // Only add the port if it isn't already added.
    if (!fwPortEnabled) {
        // Retrieve the collection of globally open ports.
        hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
        if (FAILED(hr)) {
            printf("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
            goto error;
        }

        // Create an instance of an open port.
        hr = CoCreateInstance(__uuidof(NetFwOpenPort),
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              __uuidof(INetFwOpenPort),
                              (void **)&fwOpenPort);
        if (FAILED(hr)) {
            printf("CoCreateInstance failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the port number.
        hr = fwOpenPort->put_Port(portNumber);
        if (FAILED(hr)) {
            printf("put_Port failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the IP protocol.
        hr = fwOpenPort->put_Protocol(ipProtocol);
        if (FAILED(hr)) {
            printf("put_Protocol failed: 0x%08lx\n", hr);
            goto error;
        }

        // Allocate a BSTR for the friendly name of the port.
        fwBstrName = SysAllocString(name);
        if (SysStringLen(fwBstrName) == 0) {
            hr = E_OUTOFMEMORY;
            printf("SysAllocString failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the friendly name of the port.
        hr = fwOpenPort->put_Name(fwBstrName);
        if (FAILED(hr)) {
            printf("put_Name failed: 0x%08lx\n", hr);
            goto error;
        }

        // Opens the port and adds it to the collection.
        hr = fwOpenPorts->Add(fwOpenPort);
        if (FAILED(hr)) {
            printf("Add failed: 0x%08lx\n", hr);
            goto error;
        }

        printf("Port %ld is now open in the firewall.\n", portNumber);
    }

error:

    // Free the BSTR.
    SysFreeString(fwBstrName);

    // Release the open port instance.
    if (fwOpenPort != NULL) {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL) {
        fwOpenPorts->Release();
    }

    return hr;
}


int __cdecl Exercising(int argc, wchar_t * argv[])
/*
Exercising the Firewall using C++

12/01/2010

The following code example exercises the Windows Firewall profile;
displays the current profile, turns off the firewall, turns on the firewall, and adds an application.

关闭和开启防火墙有效，通杀XP和win10/7.

https://docs.microsoft.com/en-us/previous-versions//aa364726(v=vs.85)?redirectedfrom=MSDN
*/
{
    HRESULT hr = S_OK;
    HRESULT comInit = E_FAIL;
    INetFwProfile * fwProfile = NULL;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    // Initialize COM.
    comInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (comInit != RPC_E_CHANGED_MODE) {
        hr = comInit;
        if (FAILED(hr)) {
            printf("CoInitializeEx failed: 0x%08lx\n", hr);
            goto error;
        }
    }

    // Retrieve the firewall profile currently in effect.
    hr = WindowsFirewallInitialize(&fwProfile);
    if (FAILED(hr)) {
        printf("WindowsFirewallInitialize failed: 0x%08lx\n", hr);
        goto error;
    }

    // Turn off the firewall.
    hr = WindowsFirewallTurnOff(fwProfile);
    if (FAILED(hr)) {
        printf("WindowsFirewallTurnOff failed: 0x%08lx\n", hr);
        goto error;
    }

    // Turn on the firewall.
    hr = WindowsFirewallTurnOn(fwProfile);
    if (FAILED(hr)) {
        printf("WindowsFirewallTurnOn failed: 0x%08lx\n", hr);
        goto error;
    }

    // Add Windows Messenger to the authorized application collection.
    hr = WindowsFirewallAddApp(fwProfile,
                               L"%ProgramFiles%\\Messenger\\msmsgs.exe",
                               L"Windows Messenger");
    if (FAILED(hr)) {
        printf("WindowsFirewallAddApp failed: 0x%08lx\n", hr);
        goto error;
    }

    // Add TCP::80 to list of globally open ports.
    hr = WindowsFirewallPortAdd(fwProfile, 80, NET_FW_IP_PROTOCOL_TCP, L"WWW");
    if (FAILED(hr)) {
        printf("WindowsFirewallPortAdd failed: 0x%08lx\n", hr);
        goto error;
    }

error:

    // Release the firewall profile.
    WindowsFirewallCleanup(fwProfile);

    // Uninitialize COM.
    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
