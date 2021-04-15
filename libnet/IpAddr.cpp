#include "pch.h"
#include "IpAddr.h"


PRtlIpv4AddressToString Ipv4AddressToStringW;
RtlIpv4StringToAddressRoutine Ipv4StringToAddressW;

PRtlIpv6AddressToString Ipv6AddressToStringW;
PRtlIpv6AddressToStringA Ipv6AddressToStringA;
RtlIpv6StringToAddressRoutine Ipv6StringToAddressW;


//////////////////////////////////////////////////////////////////////////////////////////////////


int _cdecl special_ip()
{
    HMODULE ntdll = GetModuleHandle(TEXT("Ntdll"));
    _ASSERTE(ntdll);

    Ipv4AddressToStringW = (PRtlIpv4AddressToString)GetProcAddress(ntdll, "RtlIpv4AddressToStringW");
    _ASSERTE(Ipv4AddressToStringW);
    Ipv4StringToAddressW = (RtlIpv4StringToAddressRoutine)GetProcAddress(ntdll, "RtlIpv4StringToAddressW");
    _ASSERTE(Ipv4StringToAddressW);

    Ipv6AddressToStringW = (PRtlIpv6AddressToString)GetProcAddress(ntdll, "RtlIpv6AddressToStringW");
    _ASSERTE(Ipv6AddressToStringW);
    Ipv6StringToAddressW = (RtlIpv6StringToAddressRoutine)GetProcAddress(ntdll, "RtlIpv6StringToAddressW");
    _ASSERTE(Ipv6StringToAddressW);

    Ipv6AddressToStringA = (PRtlIpv6AddressToStringA)GetProcAddress(ntdll, "RtlIpv6AddressToStringA");
    _ASSERTE(Ipv6AddressToStringA);


    //////////////////////////////////////////////////////////////////////////////////////////////////


    WCHAR IPv4[16 + 1] = {0};
    IN_ADDR test;

    printf("那些导出但未定义的特殊IPv4地址，长的啥样呢？请看：\n");

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_any, IPv4);
    printf("in4addr_any:%ls.\n", IPv4);
    test.S_un.S_addr = IN4ADDR_ANY;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_any, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_loopback, IPv4);
    printf("in4addr_loopback:%ls.\n", IPv4);
    test.S_un.S_addr = IN4ADDR_LOOPBACK;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_loopback, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_broadcast, IPv4);
    printf("in4addr_broadcast:%ls.\n", IPv4);
    test.S_un.S_addr = IN4ADDR_BROADCAST;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_broadcast, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_allnodesonlink, IPv4);
    printf("in4addr_allnodesonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLNODESONLINK_INIT;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_allnodesonlink, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_allroutersonlink, IPv4);
    printf("in4addr_allroutersonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLROUTERSONLINK_INIT;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_allroutersonlink, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_alligmpv3routersonlink, IPv4);
    printf("in4addr_alligmpv3routersonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLIGMPV3ROUTERSONLINK_INIT;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_alligmpv3routersonlink, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_allteredohostsonlink, IPv4);
    printf("in4addr_allteredohostsonlink:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_ALLTEREDONODESONLINK_INIT;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_allteredohostsonlink, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_linklocalprefix, IPv4);
    printf("in4addr_linklocalprefix:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_LINKLOCALPREFIX_INIT;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_linklocalprefix, &test));

    RtlZeroMemory(IPv4, sizeof(IPv4));
    Ipv4AddressToStringW(&in4addr_multicastprefix, IPv4);
    printf("in4addr_multicastprefix:%ls.\n", IPv4);
    test.S_un.S_un_b = IN4ADDR_MULTICASTPREFIX_INIT;
    _ASSERTE(IN4_ADDR_EQUAL(&in4addr_multicastprefix, &test));

    printf("\n");


    //////////////////////////////////////////////////////////////////////////////////////////////////


    char IPv6[MAX_ADDRESS_STRING_LENGTH + 1] = {0};
    IN6_ADDR temp;

    printf("那些导出但未定义的特殊IPv6地址，长的啥样呢？请看：\n");


    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_any, IPv6);
    printf("in6addr_any:%s.\n", IPv6);
    temp = IN6ADDR_ANY_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_any, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_loopback, IPv6);
    printf("in6addr_loopback:%s.\n", IPv6);
    temp = IN6ADDR_LOOPBACK_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_loopback, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_allnodesonnode, IPv6);
    printf("in6addr_allnodesonnode:%s.\n", IPv6);
    temp = IN6ADDR_ALLNODESONNODE_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_allnodesonnode, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_allnodesonlink, IPv6);
    printf("in6addr_allnodesonlink:%s.\n", IPv6);
    temp = IN6ADDR_ALLNODESONLINK_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_allnodesonlink, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_allroutersonlink, IPv6);
    printf("in6addr_allroutersonlink:%s.\n", IPv6);
    temp = IN6ADDR_ALLROUTERSONLINK_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_allroutersonlink, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_allmldv2routersonlink, IPv6);
    printf("in6addr_allmldv2routersonlink:%s.\n", IPv6);
    temp = IN6ADDR_ALLMLDV2ROUTERSONLINK_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_allmldv2routersonlink, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_teredoinitiallinklocaladdress, IPv6);
    printf("in6addr_teredoinitiallinklocaladdress:%s.\n", IPv6);
    temp = IN6ADDR_TEREDOINITIALLINKLOCALADDRESS_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_teredoinitiallinklocaladdress, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_linklocalprefix, IPv6);
    printf("in6addr_linklocalprefix:%s.\n", IPv6);
    temp = IN6ADDR_LINKLOCALPREFIX_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_linklocalprefix, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_multicastprefix, IPv6);
    printf("in6addr_multicastprefix:%s.\n", IPv6);
    temp = IN6ADDR_MULTICASTPREFIX_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_multicastprefix, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_solicitednodemulticastprefix, IPv6);
    printf("in6addr_solicitednodemulticastprefix:%s.\n", IPv6);
    temp = IN6ADDR_SOLICITEDNODEMULTICASTPREFIX_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_solicitednodemulticastprefix, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_v4mappedprefix, IPv6);
    printf("in6addr_v4mappedprefix:%s.\n", IPv6);
    temp = IN6ADDR_V4MAPPEDPREFIX_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_v4mappedprefix, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_6to4prefix, IPv6);
    printf("in6addr_6to4prefix:%s.\n", IPv6);
    temp = IN6ADDR_6TO4PREFIX_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_6to4prefix, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_teredoprefix, IPv6);
    printf("in6addr_teredoprefix:%s.\n", IPv6);
    temp = IN6ADDR_TEREDOPREFIX_INIT;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_teredoprefix, &temp));

    RtlZeroMemory(IPv6, sizeof(IPv6));
    Ipv6AddressToStringA(&in6addr_teredoprefix_old, IPv6);
    printf("in6addr_teredoprefix_old:%s.\n", IPv6);
    temp = IN6ADDR_TEREDOPREFIX_INIT_OLD;
    _ASSERTE(IN6_ADDR_EQUAL(&in6addr_teredoprefix_old, &temp));

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
