#include "init.h"


RtlEthernetStringToAddressW_Fn EthernetStringToAddressW;
RtlEthernetAddressToStringW_Fn EthernetAddressToStringW;


void get_api_address()
{
    HMODULE ModuleHandle = GetModuleHandle(TEXT("ntdll.dll"));
    if (NULL != ModuleHandle) {
        EthernetStringToAddressW = (RtlEthernetStringToAddressW_Fn)
            GetProcAddress(ModuleHandle, "RtlEthernetStringToAddressW");
        if (NULL == EthernetStringToAddressW) {
            printf("û���ҵ�RtlEthernetStringToAddressW����\n");
        }

        EthernetAddressToStringW = (RtlEthernetAddressToStringW_Fn)
            GetProcAddress(ModuleHandle, "RtlEthernetAddressToStringW");
        if (NULL == EthernetAddressToStringW) {
            printf("û���ҵ�RtlEthernetStringToAddressW����\n");
        }
    }
}


void init()
{
    get_api_address();

}
