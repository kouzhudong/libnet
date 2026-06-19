#include "pch.h"
#include "IpAddr.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


int _cdecl special_ip()
{
    WCHAR IPv4[16 + 1] = {0};
    IN_ADDR test;

    printf("那些导出但未定义的特殊IPv4地址，长的啥样呢？请看：\n");

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_any, IPv4);
    printf("in4addr_any:%ls.\n", IPv4);
    test.S_un.S_addr = IN4ADDR_ANY;
    if (!IN4_ADDR_EQUAL(&in4addr_any, &test)) {
        printf("MISMATCH: in4addr_any\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_loopback, IPv4);
    printf("in4addr_loopback:%ls.\n", IPv4);
    test.S_un.S_addr = IN4ADDR_LOOPBACK;
    if (!IN4_ADDR_EQUAL(&in4addr_loopback, &test)) {
        printf("MISMATCH: in4addr_loopback\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_broadcast, IPv4);
    printf("in4addr_broadcast:%ls.\n", IPv4);
    test.S_un.S_addr = IN4ADDR_BROADCAST;
    if (!IN4_ADDR_EQUAL(&in4addr_broadcast, &test)) {
        printf("MISMATCH: in4addr_broadcast\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_allnodesonlink, IPv4);
    printf("in4addr_allnodesonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLNODESONLINK_INIT;
    if (!IN4_ADDR_EQUAL(&in4addr_allnodesonlink, &test)) {
        printf("MISMATCH: in4addr_allnodesonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_allroutersonlink, IPv4);
    printf("in4addr_allroutersonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLROUTERSONLINK_INIT;
    if (!IN4_ADDR_EQUAL(&in4addr_allroutersonlink, &test)) {
        printf("MISMATCH: in4addr_allroutersonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_alligmpv3routersonlink, IPv4);
    printf("in4addr_alligmpv3routersonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLIGMPV3ROUTERSONLINK_INIT;
    if (!IN4_ADDR_EQUAL(&in4addr_alligmpv3routersonlink, &test)) {
        printf("MISMATCH: in4addr_alligmpv3routersonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_allteredohostsonlink, IPv4);
    printf("in4addr_allteredohostsonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLTEREDONODESONLINK_INIT;
    if (!IN4_ADDR_EQUAL(&in4addr_allteredohostsonlink, &test)) {
        printf("MISMATCH: in4addr_allteredohostsonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_linklocalprefix, IPv4);
    printf("in4addr_linklocalprefix:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_LINKLOCALPREFIX_INIT;
    if (!IN4_ADDR_EQUAL(&in4addr_linklocalprefix, &test)) {
        printf("MISMATCH: in4addr_linklocalprefix\n");
        return 1;
    }

    RtlZeroMemory(IPv4, sizeof(IPv4));
    RtlIpv4AddressToStringW(&in4addr_multicastprefix, IPv4);
    printf("in4addr_multicastprefix:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_MULTICASTPREFIX_INIT;
    if (!IN4_ADDR_EQUAL(&in4addr_multicastprefix, &test)) {
        printf("MISMATCH: in4addr_multicastprefix\n");
        return 1;
    }

    printf("\n");


    //////////////////////////////////////////////////////////////////////////////////////////////////


    char IPv6[MAX_ADDRESS_STRING_LENGTH + 1] = {0};
    IN6_ADDR temp;

    printf("那些导出但未定义的特殊IPv6地址，长的啥样呢？请看：\n");


    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_any, IPv6);
    printf("in6addr_any:%s.\n", IPv6);
    temp = IN6ADDR_ANY_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_any, &temp)) {
        printf("MISMATCH: in6addr_any\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_loopback, IPv6);
    printf("in6addr_loopback:%s.\n", IPv6);
    temp = IN6ADDR_LOOPBACK_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_loopback, &temp)) {
        printf("MISMATCH: in6addr_loopback\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_allnodesonnode, IPv6);
    printf("in6addr_allnodesonnode:%s.\n", IPv6);
    temp = IN6ADDR_ALLNODESONNODE_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_allnodesonnode, &temp)) {
        printf("MISMATCH: in6addr_allnodesonnode\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_allnodesonlink, IPv6);
    printf("in6addr_allnodesonlink:%s.\n", IPv6);
    temp = IN6ADDR_ALLNODESONLINK_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_allnodesonlink, &temp)) {
        printf("MISMATCH: in6addr_allnodesonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_allroutersonlink, IPv6);
    printf("in6addr_allroutersonlink:%s.\n", IPv6);
    temp = IN6ADDR_ALLROUTERSONLINK_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_allroutersonlink, &temp)) {
        printf("MISMATCH: in6addr_allroutersonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_allmldv2routersonlink, IPv6);
    printf("in6addr_allmldv2routersonlink:%s.\n", IPv6);
    temp = IN6ADDR_ALLMLDV2ROUTERSONLINK_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_allmldv2routersonlink, &temp)) {
        printf("MISMATCH: in6addr_allmldv2routersonlink\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_teredoinitiallinklocaladdress, IPv6);
    printf("in6addr_teredoinitiallinklocaladdress:%s.\n", IPv6);
    temp = IN6ADDR_TEREDOINITIALLINKLOCALADDRESS_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_teredoinitiallinklocaladdress, &temp)) {
        printf("MISMATCH: in6addr_teredoinitiallinklocaladdress\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_linklocalprefix, IPv6);
    printf("in6addr_linklocalprefix:%s.\n", IPv6);
    temp = IN6ADDR_LINKLOCALPREFIX_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_linklocalprefix, &temp)) {
        printf("MISMATCH: in6addr_linklocalprefix\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_multicastprefix, IPv6);
    printf("in6addr_multicastprefix:%s.\n", IPv6);
    temp = IN6ADDR_MULTICASTPREFIX_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_multicastprefix, &temp)) {
        printf("MISMATCH: in6addr_multicastprefix\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_solicitednodemulticastprefix, IPv6);
    printf("in6addr_solicitednodemulticastprefix:%s.\n", IPv6);
    temp = IN6ADDR_SOLICITEDNODEMULTICASTPREFIX_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_solicitednodemulticastprefix, &temp)) {
        printf("MISMATCH: in6addr_solicitednodemulticastprefix\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_v4mappedprefix, IPv6);
    printf("in6addr_v4mappedprefix:%s.\n", IPv6);
    temp = IN6ADDR_V4MAPPEDPREFIX_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_v4mappedprefix, &temp)) {
        printf("MISMATCH: in6addr_v4mappedprefix\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_6to4prefix, IPv6);
    printf("in6addr_6to4prefix:%s.\n", IPv6);
    temp = IN6ADDR_6TO4PREFIX_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_6to4prefix, &temp)) {
        printf("MISMATCH: in6addr_6to4prefix\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_teredoprefix, IPv6);
    printf("in6addr_teredoprefix:%s.\n", IPv6);
    temp = IN6ADDR_TEREDOPREFIX_INIT;
    if (!IN6_ADDR_EQUAL(&in6addr_teredoprefix, &temp)) {
        printf("MISMATCH: in6addr_teredoprefix\n");
        return 1;
    }

    RtlZeroMemory(IPv6, sizeof(IPv6));
    RtlIpv6AddressToStringA(&in6addr_teredoprefix_old, IPv6);
    printf("in6addr_teredoprefix_old:%s.\n", IPv6);
    temp = IN6ADDR_TEREDOPREFIX_INIT_OLD;
    if (!IN6_ADDR_EQUAL(&in6addr_teredoprefix_old, &temp)) {
        printf("MISMATCH: in6addr_teredoprefix_old\n");
        return 1;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
DWORD WINAPI EnumIPv4ByMask(const char * ipv4, const char * mask)
/*
功能：枚举指定网络的IPv4(示例IPv4和子网掩码).

测试用例：
EnumIPv4ByMask("192.168.5.3", "255.255.255.0");
EnumIPv4ByMask("172.31.96.1", "255.255.240.0");
*/
{
    IN_ADDR IPv4;
    if (!InetPtonA(AF_INET, ipv4, &IPv4)) {
        return ERROR_INVALID_PARAMETER;
    }

    IN_ADDR Mask;
    if (!InetPtonA(AF_INET, mask, &Mask)) {
        return ERROR_INVALID_PARAMETER;
    }

    ULONG z = ntohl(Mask.S_un.S_addr);

    //////////////////////////////////////////////////////////////////////////////////////////////
    //计算起始地址。

    IN_ADDR base;
    base.S_un.S_addr = IPv4.S_un.S_addr & Mask.S_un.S_addr;

    wchar_t Base[INET_ADDRSTRLEN] = {0};
    InetNtop(AF_INET, &base, Base, _ARRAYSIZE(Base));

    //////////////////////////////////////////////////////////////////////////////////////////////
    //扫描子网掩码中（后面的）0的个数（按位计算），以便于计算子网掩码。

    char bits = 0;

    for (char x = 0; x < 32; x++) {
        ULONG t = 1 << x;
        if (z & t) {
            bits = x;
            break;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //打印信息。

    ULONG numbers = 1 << bits;
    printf("IPv4的子网掩码位数：%d\n", 32 - bits);
    printf("IPv4地址个数（包括特殊地址）：%lu\n", numbers);

    for (ULONG i = 0; i < numbers; i++) {
        IN_ADDR start;

        start.S_un.S_addr = base.S_un.S_addr + ntohl(i);

        wchar_t buf[INET_ADDRSTRLEN] = {0};
        InetNtop(AF_INET, &start, buf, _ARRAYSIZE(buf));

        printf("%ls\n", buf);
    }

    return ERROR_SUCCESS;
}


EXTERN_C
DLLEXPORT
DWORD WINAPI EnumIPv4ByMasks(const char * ipv4, BYTE mask)
/*
功能：枚举指定网络的IPv4(示例IPv4和子网掩码).

测试用例：
EnumIPv4ByMasks("192.168.5.3", 24);
EnumIPv4ByMasks("172.31.96.1", 20);
EnumIPv4ByMasks("1.2.3.5", 0);
EnumIPv4ByMasks("1.2.3.5", 1);
EnumIPv4ByMasks("1.2.3.5", 31);
EnumIPv4ByMasks("1.2.3.5", 32);
*/
{
    if (mask > 32) {
        return ERROR_INVALID_PARAMETER;
    }

    IN_ADDR IPv4;
    if (!InetPtonA(AF_INET, ipv4, &IPv4)) {
        return ERROR_INVALID_PARAMETER;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    /*
    （根据mask）生成子网掩码。
    生成的步骤/思路：
    1.把子网掩码后面的0变为1.
    2.再取反。
    3.再转换为网络序。
    */

    IN_ADDR Mask;
    Mask.S_un.S_addr = 0;

    for (char x = 0; x < (32 - mask); x++) {
        ULONG t = 1 << x;
        Mask.S_un.S_addr |= t;
    }

    Mask.S_un.S_addr = ~Mask.S_un.S_addr;

    Mask.S_un.S_addr = ntohl(Mask.S_un.S_addr);

    wchar_t buffer[INET_ADDRSTRLEN] = {0};
    InetNtop(AF_INET, &Mask, buffer, _ARRAYSIZE(buffer));
    printf("mask:%ls\n", buffer);

    //////////////////////////////////////////////////////////////////////////////////////////////
    //计算起始/基础地址。

    IN_ADDR base;
    base.S_un.S_addr = IPv4.S_un.S_addr & Mask.S_un.S_addr;

    wchar_t Base[INET_ADDRSTRLEN] = {0};
    InetNtop(AF_INET, &base, Base, _ARRAYSIZE(Base));
    printf("BaseAddr:%ls\n", Base);

    //////////////////////////////////////////////////////////////////////////////////////////////
    //打印信息。

    UINT64 numbers = (UINT64)1 << (32 - mask);

    printf("IPv4的子网掩码位数：%u\n", mask);
    printf("IPv4地址个数（包括特殊地址）：%I64u\n", numbers);
    printf("\n");

    for (UINT64 i = 0; i < numbers; i++) {
        IN_ADDR temp;

        temp.S_un.S_addr = base.S_un.S_addr + ntohl(static_cast<ULONG>(i));

        wchar_t buf[INET_ADDRSTRLEN] = {0};
        InetNtop(AF_INET, &temp, buf, _ARRAYSIZE(buf));

        printf("%ls\n", buf);
    }

    return ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI AddIPv4Address(int argc, char ** argv)
/*
The following example retrieves the IP address table to determine the interface index for the first adapter, 
then adds the IP address specified on command line to the first adapter. 
The IP address that was added is then deleted.

https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-addipaddress
*/
{
    /* Variables used by GetIpAddrTable */
    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IN_ADDR IPAddr;
    DWORD ifIndex;

    /* IPv4 address and subnet mask we will be adding */
    UINT iaIPAddress;
    UINT iaIPMask;

    /* Variables where handles to the added IP are returned */
    ULONG NTEContext = 0;
    ULONG NTEInstance = 0;

    /* Variables used to return error message */
    LPVOID lpMsgBuf;

    if (argc != 3) {
        printf("usage: %s IPAddress SubnetMask\n", argv[0]);
        return 1;
    }

    iaIPAddress = inet_addr(argv[1]);
    if (iaIPAddress == INADDR_NONE) {
        printf("usage: %s IPAddress SubnetMask\n", argv[0]);
        return 1;
    }

    iaIPMask = inet_addr(argv[2]);
    if (iaIPMask == INADDR_NONE) {
        printf("usage: %s IPAddress SubnetMask\n", argv[0]);
        return 1;
    }

    pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(sizeof(MIB_IPADDRTABLE));
    if (pIPAddrTable == nullptr) {
        printf("Error allocating memory needed to call GetIpAddrTable\n");
        return 1;
    } else {
        dwSize = 0;
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(dwSize);
        }
        if (pIPAddrTable == nullptr) {
            printf("Memory allocation failed for GetIpAddrTable\n");
            return 1;
        }
    }

    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) == NO_ERROR) {
        ifIndex = pIPAddrTable->table[0].dwIndex;
        printf("\n\tInterface Index:\t%ld\n", ifIndex);
        char addrBuf[INET_ADDRSTRLEN]{};
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[0].dwAddr;
        inet_ntop(AF_INET, &IPAddr, addrBuf, sizeof(addrBuf));
        printf("\tIP Address:       \t%s (%lu)\n", addrBuf, pIPAddrTable->table[0].dwAddr);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[0].dwMask;
        inet_ntop(AF_INET, &IPAddr, addrBuf, sizeof(addrBuf));
        printf("\tSubnet Mask:      \t%s (%lu)\n", addrBuf, pIPAddrTable->table[0].dwMask);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[0].dwBCastAddr;
        inet_ntop(AF_INET, &IPAddr, addrBuf, sizeof(addrBuf));
        printf("\tBroadCast Address:\t%s (%lu)\n", addrBuf, pIPAddrTable->table[0].dwBCastAddr);
        printf("\tReassembly size:  \t%lu\n\n", pIPAddrTable->table[0].dwReasmSize);
    } else {
        printf("Call to GetIpAddrTable failed with error %d.\n", dwRetVal);
        if (pIPAddrTable)
            FREE(pIPAddrTable);
        return 1;
    }

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = nullptr;
    }

    if ((dwRetVal = AddIPAddress(iaIPAddress, iaIPMask, ifIndex, &NTEContext, &NTEInstance)) == NO_ERROR) {
        printf("\tIPv4 address %s was successfully added.\n", argv[1]);
    } else {
        printf("AddIPAddress failed with error: %d\n", dwRetVal);

        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          nullptr,
                          dwRetVal,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR)&lpMsgBuf,
                          0,
                          nullptr)) {
            printf("\tError: %ls", (LPCWSTR)lpMsgBuf);
            LocalFree(lpMsgBuf);
            return 1;
        }
    }

    if ((dwRetVal = DeleteIPAddress(NTEContext)) == NO_ERROR) {
        printf("\tIPv4 address %s was successfully deleted.\n", argv[1]);
    } else {
        printf("\tDeleteIPAddress failed with error: %d\n", dwRetVal);
        return 1;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI SetStaticIPv4(PCWSTR Ipv4, PCWSTR SubnetMask)
/*
grok 3 AI 生成
*/
{
    // Initialize COM
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        printf("Failed to initialize COM: %ld\n", hres);
        return 1;
    }

    // Initialize COM security
    hres = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hres)) {
        printf("Failed to initialize COM security: %ld\n", hres);
        CoUninitialize();
        return 1;
    }

    // Connect to WMI
    IWbemLocator * pLoc = nullptr;
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
    if (FAILED(hres)) {
        printf("Failed to create IWbemLocator: %ld\n", hres);
        CoUninitialize();
        return 1;
    }

    IWbemServices * pSvc = nullptr;
    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0, 0, 0, 0, &pSvc);
    if (FAILED(hres)) {
        printf("Failed to connect to WMI: %ld\n", hres);
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // Set security levels on the proxy
    hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hres)) {
        printf("Failed to set proxy blanket: %ld\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // Query for network adapters
    IEnumWbemClassObject * pEnumerator = nullptr;
    hres = pSvc->ExecQuery(_bstr_t("WQL"),
                           _bstr_t("SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = TRUE"),
                           WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                           nullptr,
                           &pEnumerator);
    if (FAILED(hres)) {
        printf("Query failed: %ld\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // Iterate through adapters
    IWbemClassObject * pclsObj = nullptr;
    ULONG uReturn = 0;
    while (pEnumerator) {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0)
            break;

        VARIANT vtProp;
        VariantInit(&vtProp);

        // Get the EnableStatic method
        IWbemClassObject * pClass = nullptr;
        hres = pSvc->GetObject(_bstr_t("Win32_NetworkAdapterConfiguration"), 0, nullptr, &pClass, nullptr);
        if (FAILED(hres)) {
            printf("Failed to get class object: %ld\n", hres);
            pclsObj->Release();
            continue;
        }

        IWbemClassObject * pInParamsDefinition = nullptr;
        hres = pClass->GetMethod(_bstr_t("EnableStatic"), 0, &pInParamsDefinition, nullptr);
        if (FAILED(hres)) {
            printf("Failed to get method: %ld\n", hres);
            pClass->Release();
            pclsObj->Release();
            continue;
        }

        IWbemClassObject * pInParams = nullptr;
        hres = pInParamsDefinition->SpawnInstance(0, &pInParams);
        if (FAILED(hres)) {
            printf("Failed to spawn instance: %ld\n", hres);
            pInParamsDefinition->Release();
            pClass->Release();
            pclsObj->Release();
            continue;
        }

        // Set IP address and subnet mask
        VARIANT vIP, vSubnet;
        VariantInit(&vIP);
        VariantInit(&vSubnet);
        SAFEARRAY * ipArray = SafeArrayCreateVector(VT_BSTR, 0, 1);
        SAFEARRAY * subnetArray = SafeArrayCreateVector(VT_BSTR, 0, 1);
        LONG index = 0;
        _bstr_t ipAddress = Ipv4;
        _bstr_t subnetMask = SubnetMask;
        (void)SafeArrayPutElement(ipArray, &index, &ipAddress);
        (void)SafeArrayPutElement(subnetArray, &index, &subnetMask);
        vIP.vt = VT_ARRAY | VT_BSTR;
        vIP.parray = ipArray;
        vSubnet.vt = VT_ARRAY | VT_BSTR;
        vSubnet.parray = subnetArray;

        hres = pInParams->Put(_bstr_t("IPAddress"), 0, &vIP, 0);
        hres = pInParams->Put(_bstr_t("SubnetMask"), 0, &vSubnet, 0);

        // Execute the EnableStatic method
        IWbemClassObject * pOutParams = nullptr;
        hres = pSvc->ExecMethod(_bstr_t(pclsObj), _bstr_t("EnableStatic"), 0, nullptr, pInParams, &pOutParams, nullptr);
        if (FAILED(hres)) {
            printf("Failed to execute EnableStatic: %ld\n", hres);
        } else {
            printf("IP address set successfully.\n");
        }

        // Cleanup
        VariantClear(&vIP);
        VariantClear(&vSubnet);
        pInParams->Release();
        pInParamsDefinition->Release();
        pClass->Release();
        pclsObj->Release();
        if (pOutParams)
            pOutParams->Release();
    }

    // Cleanup
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//一下代码由grok 3 AI 生成。


bool ParseCIDR(const std::string & cidr, std::string & ip, int & prefix)
{
    // IPv4 CIDR 的正则表达式
    // 匹配格式：xxx.xxx.xxx.xxx/yy
    // xxx: 0-255, yy: 0-32
    const std::regex cidr_regex("^(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})/(\\d{1,2})$");

    std::smatch match;
    if (std::regex_match(cidr, match, cidr_regex)) {
        // 提取 IP 地址的四个部分
        int octets[4];
        for (int i = 1; i <= 4; ++i) {
            octets[i - 1] = std::stoi(match[i].str());
            // 验证每个八位字节在 0-255 范围内
            if (octets[i - 1] < 0 || octets[i - 1] > 255) {
                return false;
            }
        }
        // 提取前缀长度
        prefix = std::stoi(match[5].str());
        // 验证前缀长度在 0-32 范围内
        if (prefix < 0 || prefix > 32) {
            return false;
        }
        // 拼接 IP 地址
        ip = match[1].str() + "." + match[2].str() + "." + match[3].str() + "." + match[4].str();
        return true;
    }
    return false;
}

int TestParseCIDR()
{
    std::string cidr;
    std::cout << "请输入 IPv4 CIDR 地址 (如 192.168.1.0/24): ";
    std::getline(std::cin, cidr);

    std::string ip;
    int prefix;

    if (ParseCIDR(cidr, ip, prefix)) {
        std::cout << "解析结果:\n";
        std::cout << "IP 地址: " << ip << "\n";
        std::cout << "子网掩码位数: " << prefix << "\n";
    } else {
        std::cout << "无效的 CIDR 格式!\n";
    }

    return 0;
}


bool ParseIPv6CIDR(const std::string & cidr, std::string & ip, int & prefix)
{
    // IPv6 CIDR 正则表达式
    // 匹配格式：xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx/yy
    // 支持压缩格式（如 ::）和省略前导零
    const std::regex ipv6_cidr_regex("^((?:[0-9a-fA-F]{1,4}:){0,7}[0-9a-fA-F]{1,4}::?|"
                                     "(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})/(\\d{1,3})$");

    std::smatch match;
    if (std::regex_match(cidr, match, ipv6_cidr_regex)) {
        // 提取 IP 地址
        ip = match[1].str();
        // 提取前缀长度
        prefix = std::stoi(match[2].str());
        // 验证前缀长度在 0-128 范围内
        if (prefix < 0 || prefix > 128) {
            return false;
        }
        // 验证 IP 地址格式（简单检查，确保不是空或无效）
        if (ip.empty() || ip == "::") {
            ip = "::"; // 特殊处理全零地址
            return true;
        }
        // 确保 IP 地址是有效的 IPv6 地址（进一步检查可添加）
        return true;
    }
    return false;
}

int TestParseIPv6CIDR()
{
    std::string cidr;
    std::cout << "请输入 IPv6 CIDR 地址 (如 2001:0db8::/32): ";
    std::getline(std::cin, cidr);

    std::string ip;
    int prefix;

    if (ParseIPv6CIDR(cidr, ip, prefix)) {
        std::cout << "解析结果:\n";
        std::cout << "IP 地址: " << ip << "\n";
        std::cout << "前缀长度: " << prefix << "\n";
    } else {
        std::cout << "无效的 IPv6 CIDR 格式!\n";
    }

    return 0;
}


std::string prefixToIPv4Mask(int prefix)
// 将 IPv4 前缀长度转换为子网掩码字符串
{
    if (prefix < 0 || prefix > 32) {
        return "Invalid prefix";
    }

    uint32_t mask = (prefix == 0) ? 0 : (0xffffffff << (32 - prefix));
    std::vector<int> octets(4);
    octets[0] = (mask >> 24) & 0xff;
    octets[1] = (mask >> 16) & 0xff;
    octets[2] = (mask >> 8) & 0xff;
    octets[3] = mask & 0xff;

    std::stringstream ss;
    ss << octets[0] << "." << octets[1] << "." << octets[2] << "." << octets[3];
    return ss.str();
}

std::string prefixToIPv6Mask(int prefix)
// 将 IPv6 前缀长度转换为子网掩码字符串
{
    if (prefix < 0 || prefix > 128) {
        return "Invalid prefix";
    }

    std::vector<uint16_t> segments(8, 0);
    int full_segments = prefix / 16;
    int remaining_bits = prefix % 16;

    for (int i = 0; i < full_segments; ++i) {
        segments[i] = 0xffff;
    }
    if (remaining_bits > 0 && full_segments < 8) {
        segments[full_segments] = (0xffff << (16 - remaining_bits)) & 0xffff;
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 8; ++i) {
        if (i > 0)
            ss << ":";
        ss << std::setw(4) << segments[i];
    }

    std::string result = ss.str();
    if (prefix == 0) {
        return "::";
    }
    size_t pos = result.find("0000:0000");
    if (pos != std::string::npos && result.find("0000:0000", pos + 1) != std::string::npos) {
        result.replace(pos, result.length(), "::");
    }

    return result;
}

int TestprefixToMask()
{
    int prefix;
    std::string ip_version;

    std::cout << "请输入 IP 版本 (IPv4 或 IPv6): ";
    std::getline(std::cin, ip_version);
    std::cout << "请输入前缀长度: ";
    std::cin >> prefix;
    std::cin.ignore(); // 清理输入缓冲区

    if (ip_version == "IPv4") {
        std::string mask = prefixToIPv4Mask(prefix);
        std::cout << "IPv4 子网掩码: " << mask << "\n";
    } else if (ip_version == "IPv6") {
        std::string mask = prefixToIPv6Mask(prefix);
        std::cout << "IPv6 子网掩码: " << mask << "\n";
    } else {
        std::cout << "无效的 IP 版本!\n";
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
