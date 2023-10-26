#include "init.h"


RtlEthernetStringToAddressW_Fn EthernetStringToAddressW;
RtlEthernetAddressToStringW_Fn EthernetAddressToStringW;


void get_api_address()
{
    HMODULE ModuleHandle = GetModuleHandle(TEXT("ntdll.dll"));
    if (nullptr != ModuleHandle) {
        EthernetStringToAddressW = (RtlEthernetStringToAddressW_Fn)
            GetProcAddress(ModuleHandle, "RtlEthernetStringToAddressW");
        if (nullptr == EthernetStringToAddressW) {
            printf("没有找到RtlEthernetStringToAddressW函数\n");
        }

        EthernetAddressToStringW = (RtlEthernetAddressToStringW_Fn)
            GetProcAddress(ModuleHandle, "RtlEthernetAddressToStringW");
        if (nullptr == EthernetAddressToStringW) {
            printf("没有找到RtlEthernetStringToAddressW函数\n");
        }
    }
}


void init()
{
    get_api_address();

}
