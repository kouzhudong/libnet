#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


typedef
LPTSTR(NTAPI * PRtlIpv4AddressToString)(
    __in          const IN_ADDR * Addr,
    __out         LPTSTR S
    );

typedef
LONG(NTAPI * RtlIpv4StringToAddressRoutine)(
    _In_  PCTSTR  S,
    _In_  BOOLEAN Strict,
    _Out_ LPCTSTR * Terminator,
    _Out_ IN_ADDR * Addr
    );

typedef
LPTSTR(NTAPI * PRtlIpv6AddressToString)(
    __in          const IN6_ADDR * Addr,
    __out         LPTSTR S
    );

typedef
LPTSTR(NTAPI * PRtlIpv6AddressToStringA)(
    __in          const IN6_ADDR * Addr,
    __out         LPSTR S
    );

typedef
LONG(NTAPI * RtlIpv6StringToAddressRoutine)(
    _In_  PCTSTR  S,
    _Out_ LPCTSTR * Terminator,
    _Out_ IN6_ADDR * Addr
    );


//////////////////////////////////////////////////////////////////////////////////////////////////


extern PRtlIpv4AddressToString Ipv4AddressToStringW;
extern RtlIpv4StringToAddressRoutine Ipv4StringToAddressW;

extern PRtlIpv6AddressToString Ipv6AddressToStringW;
extern PRtlIpv6AddressToStringA Ipv6AddressToStringA;
extern RtlIpv6StringToAddressRoutine Ipv6StringToAddressW;


int _cdecl special_ip();


EXTERN_C_START


__declspec(dllexport)
void WINAPI EnumIPv4ByMask(const char * ipv4, const char * mask);

__declspec(dllexport)
void WINAPI EnumIPv4ByMasks(const char * ipv4, BYTE mask);


EXTERN_C_END
