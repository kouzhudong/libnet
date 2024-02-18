// NetMon.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "notify.h"
#include "NetworkListManager.h"


void DeRegisterNotify()
{
    DeregisterNotifyIpInterfaceChange();
    DeregisterNotifyRouteChange2();
    DeregisterNotifyStableUnicastIpAddressTable();
    DeregisterNotifyTeredoPortChange();
    DeregisterNotifyUnicastIpAddressChange();

}


void RegistersNotify()
{    
    RegistersNotifyNetworkConnectivityHintChange();
    RegistersNotifyIpInterfaceChange();
    RegistersNotifyRouteChange2();
    RegistersNotifyStableUnicastIpAddressTable();
    RegistersNotifyTeredoPortChange();
    RegistersNotifyUnicastIpAddressChange();

}


int main()
{
    RegistersNotify();

    (void)getchar();

    DeRegisterNotify();
}
