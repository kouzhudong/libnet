#include "IpHelper.h"


void TestGetIPv4ByMac()
{
    LPCWSTR Terminator = nullptr;
    DL_EUI48 Mac;
    LONG ret = RtlEthernetStringToAddressW(L"00-0c-29-a2-27-81", &Terminator, &Mac);
    IN_ADDR IPv4{};
    bool b = GetIPv4ByMac(&Mac, &IPv4);
}
